#!/usr/bin/python
import getopt
from sys import argv

# format:
#   timestamp | pid | tid | r/w | offset | size | latency | checksum | md5 | iodepth
def mapping(tracefile, unit):

    fd = open(tracefile, "r")
    for line in fd:
        elems = line.split(",")
        
        offset = int(elems[4])
        print offset/unit

    fd.close()

# input is iomlog
# output is a log of addresses for parda
if __name__ == "__main__":
    script, tracefile, unit = argv
    mapping(tracefile, int(unit))
