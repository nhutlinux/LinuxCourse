#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define MSG_SIZE 14

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
    pid_t pid;

    /* Initialize pipe */
    if (pipe(fds) < 0) {
        handleError("pipe()");
        exit(EXIT_FAILURE);
    }

    /* Fork process */
    pid = fork();
    if (pid >= 0) {
        if (pid == 0) {     /* Child process */
            /* Read from pipe */
            printf("I am reader\n");
            if (close(fds[1]) == -1) {
                handleError("close() fds[1]");
                exit(EXIT_FAILURE);
            }
            while(1){
                readNo = read(fds[0], inBuff, MSG_SIZE);
                if (readNo == -1) {
                    handleError("read()");
                    exit(EXIT_FAILURE);
                } else if (readNo == 0) {
                    printf("pipe end-of-pipe\n");
                    break;
                } else {
                    printf("msg: %s\n", inBuff);  
                }
            }
            exit(0);

        } else {            /* Parent process */
            /* Register SIGCHLD signal handler to prevent zombie process */
            signal(SIGCHLD, handleSIGCHLD);

            /* Write to pipe */
            printf("I am writer\n");
            if (close(fds[0]) == -1) {
                handleError("close() fds[0]");
                exit(EXIT_FAILURE);
            }

            /* Write into pipe */
            write(fds[1], msg, MSG_SIZE);

            /* Close write fds */
            if (close(fds[1]) == -1) {
                handleError("close() fds[0]");
                exit(EXIT_FAILURE);
            }

            while(1);
        }
    } else {
        handleError("fork()");
        exit(EXIT_FAILURE);
    }

    return 0;
}