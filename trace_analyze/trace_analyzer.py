#!/usr/bin/python
import getopt
from sys import argv

sector_size = 512

def print_table_in_order(table):
    keys = []
    for key in table.keys():
        keys.append(key)
    keys.sort()

    for key in keys:
        print key, table[key]

# 8KB aligned
def addr_insert(address_table, op_type, offset, size):
    offset_aligned = offset/sector_size*sector_size
    while size > 0:
        if offset_aligned not in address_table:
            address_table[offset_aligned] = [op_type, 0, 0]
        if op_type == "r":
            address_table[offset_aligned][1] += 1
        elif op_type == "w":
            address_table[offset_aligned][2] += 1
        else:
            print "Error!\n"
            exit()
            
        size -= sector_size
        offset_aligned += sector_size

def usage_of_rw(addr_table, cache_unit):
    addr_bitmap = {}
    for addr in addr_table.keys():
        value = addr_table[addr]
        if value[1] >= 1 and value[2] >= 1:
            block_no = addr / cache_unit
            subblock_no = (addr % cache_unit) / sector_size
            if block_no not in addr_bitmap:
                addr_bitmap[block_no] = 0
            addr_bitmap[block_no] |= 1 << subblock_no 

    avg_usage = 0
    block_count = 0.0
    usage_table = {}
    for block in addr_bitmap.keys():
        bitmap = addr_bitmap[block]
        bitmap_size = cache_unit/sector_size
        count = 0
        while bitmap != 0:
            if (bitmap & 1) == 1:
                count += 1
            bitmap = bitmap >> 1
        usage = 1.0*count/bitmap_size

        if usage not in usage_table:
            usage_table[usage] = 0
        usage_table[usage] += 1

        avg_usage += usage
        block_count += 1

    avg_usage /= block_count
    #print usage_table
    print "avg_usage =", avg_usage

def usage_of_write_only(addr_table, cache_unit):
    addr_bitmap = {}
    for addr in addr_table.keys():
        value = addr_table[addr]
        if value[1] == 0 and value[2] > 1:
            block_no = addr / cache_unit
            subblock_no = (addr % cache_unit) / sector_size
            if block_no not in addr_bitmap:
                addr_bitmap[block_no] = 0
            addr_bitmap[block_no] |= 1 << subblock_no 

    avg_usage = 0
    block_count = 0.0
    usage_table = {}
    for block in addr_bitmap.keys():
        bitmap = addr_bitmap[block]
        bitmap_size = cache_unit/sector_size
        count = 0
        while bitmap != 0:
            if (bitmap & 1) == 1:
                count += 1
            bitmap = bitmap >> 1
        usage = 1.0*count/bitmap_size

        if usage not in usage_table:
            usage_table[usage] = 0
        usage_table[usage] += 1

        avg_usage += usage
        block_count += 1

    avg_usage /= block_count
    #print usage_table
    print "avg_usage =", avg_usage

def usage_of_read_only(addr_table, cache_unit):
    addr_bitmap = {}
    for addr in addr_table.keys():
        value = addr_table[addr]
        if value[1] > 1 and value[2] == 0:
            block_no = addr / cache_unit
            subblock_no = (addr % cache_unit) / sector_size
            if block_no not in addr_bitmap:
                addr_bitmap[block_no] = 0
            addr_bitmap[block_no] |= 1 << subblock_no 

    avg_usage = 0
    block_count = 0.0
    usage_table = {}
    for block in addr_bitmap.keys():
        bitmap = addr_bitmap[block]
        bitmap_size = cache_unit/sector_size
        count = 0
        while bitmap != 0:
            if (bitmap & 1) == 1:
                count += 1
            bitmap = bitmap >> 1
        usage = 1.0*count/bitmap_size

        if usage not in usage_table:
            usage_table[usage] = 0
        usage_table[usage] += 1

        avg_usage += usage
        block_count += 1

    avg_usage /= block_count
    #print usage_table
    print "avg_usage =", avg_usage

def analyze_addr_table(addr_table):
    read_only = 0 # read only by many times
    write_only = 0 # write only by many times
    read_only_once = 0 # read by only once
    write_only_once = 0 # written by only once
    read_after_write = 0 # read after write
    rw_hot_data = 0 # read and written by many times

    # read by only once
    # write by only once
    # read-only by many times
    # write-only by many times
    # read after write
    # read and written by many times
    stat_addr_count = [0, 0, 0, 0, 0, 0]
    stat_io_count = [0, 0, 0, 0, 0, 0]
    addr_count = 0
    io_count = 0

    for addr in addr_table.values():
        if addr[1] == 1 and addr[2] == 0:
            stat_addr_count[0] += 1
            stat_io_count[0] += 1
            io_count += 1
            addr_count += 1
        elif addr[1] == 0 and addr[2] == 1:
            stat_addr_count[1] += 1
            stat_io_count[1] += 1
            io_count += 1
            addr_count += 1
        elif addr[1] > 1 and addr[2] == 0:
            stat_addr_count[2] += 1
            stat_io_count[2] += addr[1] 
            io_count += addr[1]
            addr_count += 1
        elif addr[1] == 0 and addr[2] > 1:
            stat_addr_count[3] += 1
            stat_io_count[3] += addr[2] 
            io_count += addr[2]
            addr_count += 1
        elif addr[1] > 0 and addr[2] == 1 and addr[0] == "w":
            stat_addr_count[4] += 1
            stat_io_count[4] += addr[1] + 1 
            io_count += addr[1] + 1 
            addr_count += 1
        elif addr[1] >= 1 and addr[2] >= 1:
            stat_addr_count[5] += 1
            stat_io_count[5] += addr[1] + addr[2] 
            io_count += addr[1] + addr[2] 
            addr_count += 1

    print "table size, read_only_once, write_only_once, read_only, write_only, read_after_write, rw_hot_data"
    print len(addr_table)
    print addr_count, stat_addr_count 
    print [1.0*x/addr_count for x in stat_addr_count]
    print io_count, stat_io_count 
    print [1.0*x/io_count for x in stat_io_count]

# format:
#   timestamp | pid | tid | r/w | offset | size | latency | checksum | md5 | iodepth
def analyze(tracefile):

    read_count = 0
    write_count = 0
    total_count = 0

    size_table = {}
    iodepth_table = {}

    read_size_table = {}
    write_size_table = {}

    # dict = address : [first_type, read_count, write_count]
    address_table = {} 

    fd = open(tracefile, "r")
    for line in fd:
        elems = line.split(",")
        
        size = int(elems[5])
        if size not in size_table:
            size_table[size] = 0
        size_table[size] = size_table[size] + 1

        total_count = total_count + 1
        if elems[3] == "r":
            read_count = read_count + 1
            if size not in read_size_table:
                read_size_table[size] = 0
            read_size_table[size] = read_size_table[size] + 1
        else:
            write_count = write_count + 1
            if size not in write_size_table:
                write_size_table[size] = 0
            write_size_table[size] = write_size_table[size] + 1

        iodepth = int(elems[-1])
        if iodepth not in iodepth_table:
            iodepth_table[iodepth] = 0
        iodepth_table[iodepth] = iodepth_table[iodepth] + 1

        addr_insert(address_table, elems[3], int(elems[4]), int(elems[5]))

    print "read/write count:", total_count, read_count, write_count
    print "read/write percentile:", 1.0*read_count/total_count, 1.0*write_count/total_count
    print "------size table--------"
    print_table_in_order(size_table)
    print "------read size table--------"
    print_table_in_order(read_size_table)
    print "------write size table--------"
    print_table_in_order(write_size_table)
    print "------iodepth table--------"
    print_table_in_order(iodepth_table)
    print "------r/w pattern analyzation------"
    analyze_addr_table(address_table);
    print "-----usage analyzation of read-only------"
    usage_of_read_only(address_table, 1024*1024)
    usage_of_read_only(address_table, 512*1024)
    usage_of_read_only(address_table, 256*1024)
    usage_of_read_only(address_table, 128*1024)
    usage_of_read_only(address_table, 64*1024)
    usage_of_read_only(address_table, 32*1024)
    usage_of_read_only(address_table, 16*1024)
    print "-----usage analyzation of write-only------"
    usage_of_write_only(address_table, 1024*1024)
    usage_of_write_only(address_table, 512*1024)
    usage_of_write_only(address_table, 256*1024)
    usage_of_write_only(address_table, 128*1024)
    usage_of_write_only(address_table, 64*1024)
    usage_of_write_only(address_table, 32*1024)
    usage_of_write_only(address_table, 16*1024)
    print "-----usage analyzation of rw------"
    usage_of_rw(address_table, 1024*1024)
    usage_of_rw(address_table, 512*1024)
    usage_of_rw(address_table, 256*1024)
    usage_of_rw(address_table, 128*1024)
    usage_of_rw(address_table, 64*1024)
    usage_of_rw(address_table, 32*1024)
    usage_of_rw(address_table, 16*1024)

    fd.close()

if __name__ == "__main__":
    script, tracefile = argv
    print "running", script, "to analyze", tracefile
    analyze(tracefile)
