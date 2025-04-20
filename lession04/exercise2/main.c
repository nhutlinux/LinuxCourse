#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_OF_THREADS          3
#define INCREMENT_PER_THREAD    1000000

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int count = 0;

/* Threads handler */
static void *thr_handler(void *args)
{
    int i;

    pthread_mutex_lock(&lock);
    /* Critical section */
    for(i = 0; i < INCREMENT_PER_THREAD; i++){
        ++count;
    }
    pthread_mutex_unlock(&lock);
}

int main(int argc, char const *argv[])
{
    pthread_t tid[NUM_OF_THREADS];
    int i;

    /* Create threads */
    for(i = 0; i < NUM_OF_THREADS; i++){
        if (pthread_create(&tid[i], NULL, &thr_handler, NULL)) {
            printf("Fail to create thread\n");
            return -1;
        }
    }

    /* Join threads */
    for(i = 0; i < NUM_OF_THREADS; i++){
        if (pthread_join(tid[i], NULL)) {
            printf("Fail to join thread\n");
            return -1;
        }
    }

    /* Print result */
    printf("Expected count = %d\n", NUM_OF_THREADS * INCREMENT_PER_THREAD);
    printf("Actual count = %d\n", count);

    /* Normally end (return 0) */
    return 0;
}