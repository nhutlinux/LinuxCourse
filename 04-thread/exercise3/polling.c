#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define THRESHOLD   10

int count = 0;

/* Threads handler */
static void *thr_handler(void *args)
{
    while(count < THRESHOLD){
        printf("Count = %d\n", count);
        count++;
        sleep(1);
    }
}

int main(int argc, char const *argv[])
{
    pthread_t tid;

    /* Reset count */
    count = 0;

    /* Create thread */
    if (pthread_create(&tid, NULL, &thr_handler, NULL)) {
        printf("Fail to create thread\n");
        return -1;
    }

    /* Polling count */
    while(1){
        sleep(1);
        printf("Check count\n");
        if (count >= THRESHOLD) {
            printf("Count = %d. Counter stopped.\n", count);
            break;
        }
    }

    /* Join thread */
    if (pthread_join(tid, NULL)) {
        printf("Fail to join thread\n");
        return -1;
    }
    
}