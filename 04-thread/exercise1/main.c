#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_t tid1, tid2;

typedef struct {
    char name[30];
    char msg[30];
} thr_data_t;

// static void *thr_handler1(void *args) 
// {
// 	printf("Hello from thread 1\n");
// }

// static void *thr_handler2(void *args)
// {
//     thr_data_t *data = (thr_data_t *)args;
// 	printf("Hello from thread 2\n");
//     printf("Hello %s, welcome to join %s\n", data->name, data->msg);
// }

/* Threads handler */
static void *thr_handler(void *args)
{
    pthread_t tid = pthread_self();

    if (pthread_equal(tid, tid1)) {     /* Thread 1 */
        printf("Hello from thread 1\n");
    } else {                            /* Thread 2 */
        thr_data_t *data = (thr_data_t *)args;
        printf("Hello from thread 2\n");
        printf("Hello %s, welcome to join %s\n", data->name, data->msg);
    }
}

int main(int argc, char const *argv[])
{
    /* code */
    int ret;
    thr_data_t data = {0};

    strncpy(data.name, "devlinux", sizeof(data.name));                 
    strncpy(data.msg, "Linux programming", sizeof(data.msg));

    if (ret = pthread_create(&tid1, NULL, &thr_handler, NULL)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    }

    sleep(2);

    if (ret = pthread_create(&tid2, NULL, &thr_handler, &data)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    }

    // if (ret = pthread_create(&tid1, NULL, &thr_handler1, NULL)) {
    //     printf("pthread_create() error number=%d\n", ret);
    //     return -1;
    // }

    // sleep(2);

    // if (ret = pthread_create(&tid2, NULL, &thr_handler2, &data)) {
    //     printf("pthread_create() error number=%d\n", ret);
    //     return -1;
    // }
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    sleep(5);
    printf("Hi, I'm main thread\n");

    return 0;
}
