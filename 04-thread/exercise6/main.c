#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE   12//1000000
#define NUM_OF_THREADS   3

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t tid[NUM_OF_THREADS];
int buff[BUFFER_SIZE];
int sum = 0;
int pos = 0;

/* Threads handler */
static void *thr_handler(void *args)
{
    pthread_t id = pthread_self();
    int i;

    // pthread_mutex_lock(&lock);
    // for(i = 0; i < BUFFER_SIZE / NUM_OF_THREADS; i++){
    //     if (pos < BUFFER_SIZE) {
    //         sum += buff[pos++];
    //     }
    // }
    // pthread_mutex_unlock(&lock);
    // pthread_exit(NULL);

    // pthread_mutex_lock(&lock);
    for (i = 0; i < NUM_OF_THREADS; i++) {
        if (!pthread_equal(id, tid[i])) {
            int subSum = 0;
            int j;
            for(j = 0; j < BUFFER_SIZE / NUM_OF_THREADS; j++){
                subSum += buff[i * BUFFER_SIZE / NUM_OF_THREADS + j];
            }
            pthread_mutex_lock(&lock);
            /* Critical section */
            sum += subSum;
            printf("Thread %d sum: %d\n", i + 1, subSum);
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
            break;
        }
    }
    // pthread_mutex_unlock(&lock);
    // pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int i;
    int total = 0;

    /* Create data buffer */
    srand(time(NULL));          /* Create seed for rand() function */
    printf("Buffer = [ ");
    for(i = 0; i < BUFFER_SIZE; i++){
        buff[i] = (rand() % 100) + 1;
        if (i < (BUFFER_SIZE - 1)) {
            printf("%d, ", buff[i]);
        } else {
            printf("%d", buff[i]);
        }
        total += buff[i];
    }
    printf("]\n");
    printf("Total = %d\n", total);

    /* Create thread */
    for(i = 0; i < NUM_OF_THREADS; i++){
        if (pthread_create(&tid[i], NULL, &thr_handler, NULL)) {
            printf("Fail to create thread\n");
            return -1;
        }
        // sleep(1);
    }

    /* Join thread */
    for(i = 0; i < NUM_OF_THREADS; i++){
        if (pthread_join(tid[i], NULL)) {
            printf("Fail to join thread\n");
            return -1;
        }
    }
    
    /* Print result */
    printf("Sum of buffer: %d\n", sum);

    /* Normally end */
    return 0;
}