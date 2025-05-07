#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_OF_RD_THREADS   5
#define NUM_OF_WR_THREADS   2
#define THRESHOLD 10

pthread_rwlock_t rwLock = PTHREAD_RWLOCK_INITIALIZER;
pthread_t rdTid[NUM_OF_RD_THREADS];
pthread_t wrTid[NUM_OF_WR_THREADS];

int count = 0;

/* Read threads handler */
static void *rd_thr_handler(void *args)
{
    int i;
    pthread_t tid = pthread_self();

    for(i = 0; i < NUM_OF_RD_THREADS; i++){
        if (!pthread_equal(tid, rdTid[i])) {
            // printf("This is read thread %d, count is %d\n", i + 1, count);
            // sleep(5);
            // printf("Thread %d exit.\n", i + 1);
            pthread_exit(NULL);
            break;
        }
    }
    pthread_exit(NULL);
}

/* Write threads handler */
static void *wr_thr_handler(void *args)
{
    pthread_t tid = pthread_self();
    int no;
    int i;

    pthread_rwlock_wrlock(&rwLock);
    for(i = 0; i < NUM_OF_WR_THREADS; i++){
        if (!pthread_equal(tid, wrTid[i])) {
            int j;
            for(j = 0; j < THRESHOLD / NUM_OF_WR_THREADS; j++){
                if (count < THRESHOLD) {
                    printf("Write thread1, count = %d\n", count);
                    count++;
                    // sleep(1);
                }
            }
            no = i + 1;
        } else {
            int j;
            for(j = 0; j < THRESHOLD / NUM_OF_WR_THREADS; j++){
                if (count < THRESHOLD) {
                    printf("Write thread2, count = %d\n", count);
                    count++;
                    // sleep(1);
                }
            }
            no = i + 1;
        }
    }
    pthread_rwlock_unlock(&rwLock);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int i;

    /* Create thread */
    for(i = 0; i < NUM_OF_RD_THREADS; i++){
        if (pthread_create(&rdTid[i], NULL, &rd_thr_handler, NULL)) {
            printf("Fail to create thread\n");
            return -1;
        }
    }

    sleep(1);

    for(i = 0; i < NUM_OF_WR_THREADS; i++){
        if (pthread_create(&wrTid[i], NULL, &wr_thr_handler, NULL)) {
            printf("Fail to create thread\n");
            return -1;
        }
    }

    while(1){
        if (count >= THRESHOLD) {
            printf("Count = %d. Counter stopped.\n", count);
            break;
        }
    }

    // /* Join thread */
    // for(i = 0; i < NUM_OF_RD_THREADS; i++){
    //     if (pthread_join(rdTid[i], NULL)) {
    //         printf("Fail to join thread\n");
    //         return -1;
    //     }
    // }
    // for(i = 0; i < NUM_OF_WR_THREADS; i++){
    //     if (pthread_join(wrTid[i], NULL)) {
    //         printf("Fail to join thread\n");
    //         return -1;
    //     }
    // }
    
    /* Normally end */
    return 0;
}