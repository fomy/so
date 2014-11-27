#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>

/*
 * See
 * http://stackoverflow.com/questions/27161412/does-fork-use-copy-on-write#
 */

void printMemStat(){
    struct sysinfo si;
    sysinfo(&si);
    printf("===\n");
    printf("Total: %llu\n", si.totalram);
    printf("Free: %llu\n", si.freeram);
}

int main(){
    long len = 200000000;
    long *array = malloc(len*sizeof(long));
    long i = 0;
    for(; i<len; i++){
        array[i] = i;
    }

    printMemStat();
    if(fork()==0){
        /*child*/
        printMemStat();

        /* the first modification phase */
        i = 0;
        for(; i<len/2; i++){
            array[i] = i+1;
        }

        /* these codes will produce different results. */
        /*i = 0;*/
        /*for(; i<len/2; i++){*/
            /*array[i*2] = i+1;*/
        /*}*/

        printMemStat();

        /* the second modification phase */
        i = 0;
        for(; i<len; i++){
            array[i] = i+1;
        }

        printMemStat();

    }else{
        /*parent*/
        int times=10;
        while(times-->0){
            printf("sleep in parent %d\n", times);
            sleep(1);
        }
    }
    return 0;
}

