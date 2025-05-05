#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define MSG_SIZE 100

char *msg = "hello world !";

/* Handle SIGCHLD signal */
void handleSIGCHLD(int sigNo)
{
    wait(NULL);
    printf("Child process termination\n");
}

/* Main routine */
int main(int argc, char const argv[])
{
    char inBuff[MSG_SIZE];
    int readNo = 0;
    int fds[2];
    int fds2[2];
    pid_t pid, pid2;

    /* Initialize pipe */
    if (pipe(fds) < 0) {
        handleError("pipe() fds");
        exit(EXIT_FAILURE);
    }
    if (pipe(fds2) < 0) {
        handleError("pipe() fds2");
        exit(EXIT_FAILURE);
    }

    /* Fork process */
    pid = fork();
    if (pid < 0) {
        handleError("fork()");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) { /* Child process */
        /* Read from pipe */
        printf("I am reader of pipe fds\n");
        if (close(fds[1]) == -1) {
            handleError("close() fds[1]");
            exit(EXIT_FAILURE);
        }
        
        /* Write to pipe */
        printf("I am writer of pipe fds2\n");
        if (close(fds2[0]) == -1) {
            handleError("close() fds2[0]");
            exit(EXIT_FAILURE);
        }

        while(1){
            readNo = read(fds[0], inBuff, MSG_SIZE);
            if (readNo == -1) {
                handleError("read() fds[0]");
                exit(EXIT_FAILURE);
            } else if (readNo == 0) {
                printf("pid1 pipe end-of-pipe\n");
                if (close(fds[0]) == -1) {
                    handleError("close() fds[0]");
                    exit(EXIT_FAILURE);
                }
                break;
            } else {
                printf("pid2 msg: %s\n", inBuff);
                /* Write into pipe */
                // write(fds2[1], inBuff, strlen(inBuff));
                strncat(inBuff, "Hello from child 1\n", sizeof("Hello from child 1\n"));
                // write(fds2[1], "Hello from child 1\n", sizeof("Hello from child 1\n"));
                write(fds2[1], inBuff, strlen(inBuff));
                if (close(fds2[1]) == -1) {
                    handleError("close() fds2[1]");
                    exit(EXIT_FAILURE);
                }
            }
        }

        exit(0);
    }

    pid2 = fork();
    if (pid2 < 0) {
        handleError("fork()");
        exit(EXIT_FAILURE);
    }
    else if (pid2 == 0) { /* Child process */
        if (close(fds[0]) == -1) {
            handleError("close() fds[0]");
            exit(EXIT_FAILURE);
        }
        if (close(fds[1]) == -1) {
            handleError("close() fds[1]");
            exit(EXIT_FAILURE);
        }

        /* Read from pipe */
        printf("I am reader of pipe fds2\n");
        if (close(fds2[1]) == -1) {
            handleError("close() fds2[1]");
            exit(EXIT_FAILURE);
        }
        
        while(1){
            readNo = read(fds2[0], inBuff, MSG_SIZE);
            if (readNo == -1) {
                handleError("read() fds2[0]");
                exit(EXIT_FAILURE);
            } else if (readNo == 0) {
                printf("pid2 pipe end-of-pipe\n");
                if (close(fds2[0]) == -1) {
                    handleError("close() fds2[0]");
                    exit(EXIT_FAILURE);
                }
                break;
            } else {
                printf("pid2 msg: %s\n", inBuff);  
            }
        }

        sleep(1);
        exit(0);
    }

    /* Parent process */
    /* Register SIGCHLD signal handler to prevent zombie process */
    signal(SIGCHLD, handleSIGCHLD);

    /* Write to pipe */
    printf("I am writer of pipe fds\n");
    if (close(fds[0]) == -1) {
        handleError("close() fds[0]");
        exit(EXIT_FAILURE);
    }

    /* Close read / write of pipe fds2 */
    if (close(fds2[0]) == -1) {
        handleError("close() fds2[0]");
        exit(EXIT_FAILURE);
    }
    if (close(fds2[1]) == -1) {
        handleError("close() fds2[1]");
        exit(EXIT_FAILURE);
    }

    /* Write into pipe */
    // write(fds[1], msg, MSG_SIZE);
    write(fds[1], "Hello from parent\n", sizeof("Hello from parent\n"));

    /* Close write fds */
    if (close(fds[1]) == -1) {
        handleError("close() fds[1]");
        exit(EXIT_FAILURE);
    }

    while(1);

    return 0;
}