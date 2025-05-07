#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define THRESHOLD   10

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int count = 0;

/* Threads handler */
static void *thr_handler(void *args)
{
    pthread_mutex_lock(&lock);
    while(count < THRESHOLD){
        printf("Count = %d\n", count);
        count++;
        sleep(1);
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
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

    /* Conditional variable */
    pthread_mutex_lock(&lock);
    pthread_cond_wait(&cond, &lock); /* pthread_mutex_unlock -> wait -> wakeup by pthread_cond_signal -> pthread_mutex_lock */
    printf("Count = %d. Counter stopped.\n", count);
    // while(1){
    //     pthread_cond_wait(&cond, &lock); /* pthread_mutex_unlock -> wait -> wakeup by pthread_cond_signal -> pthread_mutex_lock */
    //     printf("Count = %d. Counter stopped.\n", count);
    //     count = 0;
    //     break;
    // }
    pthread_mutex_unlock(&lock);

    /* Join thread */
    if (pthread_join(tid, NULL)) {
        printf("Fail to join thread\n");
        return -1;
    }
    
}