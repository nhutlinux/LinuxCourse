#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define BUFFER_SIZE   100
#define NUM_OF_THREADS   2

pthread_t tid[NUM_OF_THREADS];
int buff[BUFFER_SIZE];
int oddCount = 0;
int evenCount = 0;

/* Threads handler */
static void *thr_handler(void *args)
{
    pthread_t id = pthread_self();
    int i;

    if (!pthread_equal(id, tid[0])) {       /* Count number of even values */
        for(i = 0; i < BUFFER_SIZE; i++){
            if (!(buff[i] % 2)) {
                evenCount++;
            }
        }
    } else {                                /* Count number of odd values */
        for(i = 0; i < BUFFER_SIZE; i++){
            if (buff[i] % 2) {
                oddCount++;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    int i;

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
    }
    printf("]\n");

    /* Create thread */
    for(i = 0; i < NUM_OF_THREADS; i++){
        if (pthread_create(&tid[i], NULL, &thr_handler, NULL)) {
            printf("Fail to create thread\n");
            return -1;
        }
    }

    /* Join thread */
    for(i = 0; i < NUM_OF_THREADS; i++){
        if (pthread_join(tid[i], NULL)) {
            printf("Fail to join thread\n");
            return -1;
        }
    }
    
    /* Print result */
    printf("Number of odd values: %d\n", oddCount);
    printf("Number of even values: %d\n", evenCount);

    /* Normally end */
    return 0;
}