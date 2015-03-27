/*
 * Multi-threaded bubble sorting
 *
 * http://cse.unl.edu/~ylu/csce451/Homework/PA3/
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/* The number of threads */
int nthreads = 40;

/* The array we sort */
int *array;
int array_length = 2000;

/* The states of each piece */
int *state;
#define SORTED 1
#define UNSORTED 2

pthread_mutex_t *lock;
pthread_cond_t *sorted_cond;

int poll = 0;
pthread_mutex_t poll_lock;
pthread_cond_t poll_cond;

int check(int start, int end){
    while(start<end){
        if(array[start] < array[start+1]){
            return 0;
        }
        start++;
    }
    printf("Correct!!!!\n");
    return 1;
}

pthread_mutex_t printlock;
void print_array(int start, int end){
    int i;
    for(i = start; i<=end; i++){
        printf("%d ", array[i]);
    }
    puts("");
}

/* descending */
void bubble(int start, int end){

    int left=start;
    while(left<end){
        int right = end;
        for(;left<right;right--){
            if(array[right] > array[right-1]){
                int swap = array[right];
                array[right] = array[right-1];
                array[right-1] = swap;
            }
        }
        left++;
    }
}

void* sort(void* data){
    int id = *(int*)data;

    int start = id*array_length/nthreads;
    int end = (id+1)*array_length/nthreads - 1;

    while(1){

        /*printf("id %d, loop %d\n", id, loop);*/
        pthread_mutex_lock(&lock[id]);
        if(state[id] == UNSORTED){

            pthread_mutex_unlock(&lock[id]);

            pthread_mutex_lock(&printlock);
            printf("==== sort %d ====\n", id);
            print_array(start, end);
            pthread_mutex_unlock(&printlock);

            /* TO DO sorting */
            bubble(start, end);

            pthread_mutex_lock(&printlock);
            print_array(start, end);
            printf("==== end %d ====\n", id);
            pthread_mutex_unlock(&printlock);

            pthread_mutex_lock(&lock[id]);

            pthread_mutex_lock(&poll_lock);
            poll=0;
            pthread_cond_broadcast(&poll_cond);
            pthread_mutex_unlock(&poll_lock);

            state[id] = SORTED;
            pthread_cond_signal(&sorted_cond[id]);
        }
        pthread_mutex_unlock(&lock[id]);

        /* if not the last thread */
        if(id < nthreads-1){
            pthread_mutex_lock(&lock[id+1]);

            while(state[id+1] != SORTED){
                pthread_mutex_lock(&printlock);
                printf("==== %d waiting sorted %d ====\n",id, id+1);
                pthread_mutex_unlock(&printlock);
                pthread_cond_wait(&sorted_cond[id+1], &lock[id+1]);
            }

            if(array[end] < array[end+1]){
                /* TO DO swap */
                int swap = array[end+1];
                array[end+1] = array[end];
                array[end] = swap;

                state[id+1] = UNSORTED;
                pthread_mutex_lock(&lock[id]);
                state[id] = UNSORTED;
                pthread_mutex_unlock(&lock[id]);

            }
            pthread_mutex_unlock(&lock[id+1]);
        }

        pthread_mutex_lock(&lock[id]);
        /* not the first thread */
        if(state[id] == SORTED){
            pthread_mutex_lock(&poll_lock);
            pthread_mutex_unlock(&lock[id]);
            poll++;
            if(poll<nthreads){

                pthread_mutex_lock(&printlock);
                printf("==== %d waiting poll ====\n", id);
                pthread_mutex_unlock(&printlock);

                pthread_cond_wait(&poll_cond, &poll_lock);

            }
            if(poll == nthreads){
                pthread_cond_broadcast(&poll_cond);
                pthread_mutex_unlock(&poll_lock);
                return;
            }
            pthread_mutex_unlock(&poll_lock);
        }else
            pthread_mutex_unlock(&lock[id]);
    }
}

void main(){
    /* init the array */
    int i;
    array = malloc(array_length * sizeof(int));
    srand(time(NULL));
    for(i = 0; i<array_length; i++){
        array[i] = rand()%(array_length*2);
    }
    print_array(0, array_length-1);


    /* init the threads */
    lock = malloc(sizeof(pthread_mutex_t)*nthreads);
    sorted_cond = malloc(sizeof(pthread_cond_t)*nthreads);

    pthread_mutex_init(&printlock, NULL);
    pthread_mutex_init(&poll_lock, NULL);
    pthread_cond_init(&poll_cond, NULL);
    for(i = 0; i<nthreads; i++){
        pthread_mutex_init(&lock[i], NULL);
        pthread_cond_init(&sorted_cond[i], NULL);
    }

    state = malloc(nthreads*sizeof(int));
    for(i = 0; i<nthreads;i++){
        state[i] = UNSORTED;
    }

    pthread_t tid[nthreads];
    for(i = 0; i<nthreads;i++){
        int *j = malloc(sizeof(j));
        *j = i;
        pthread_create(&tid[i], NULL, sort, j);
    }
    for(i = 0; i<nthreads;i++){
        pthread_join(tid[i], NULL);
    }

    print_array(0, array_length-1);
    check(0,array_length-1);
}
