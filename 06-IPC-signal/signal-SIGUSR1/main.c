#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int sendCount;

/* Handle SIGUSR1 signal */
void handleSIGUSR1()
{
    printf("Received SIGUSR1 signal from parent: %d\n", ++sendCount);
}

int main()
{
    pid_t pid;
    int status;
    int retVal;

    pid = fork();
    if (pid == 0) {             /* Child process */
        printf("This is the child process, my PID is: %d\n", getpid());
        sendCount = 0;
        /* Register SIGUSR1 handler */
        if (signal(SIGUSR1, handleSIGUSR1) == SIG_ERR) {
            fprintf(stderr, "Cannot handle SIGUSR1\n");
            exit(EXIT_FAILURE);
        }

        while(sendCount < 5);

        printf("Child process stop\n");
        exit(0);

    } else if (pid > 0) {       /* Parent process */
        printf("This is the parent process, my PID is: %d\n", getpid());
        printf("The child process PID is: %d\n", pid);
        int count = 0;

        while (count < 5) {
            sleep(2);
            printf("sendCount: %d\n", ++count);
            kill(pid, SIGUSR1);
        }

        retVal = wait(&status);
        if (retVal == -1) {
            printf("wait() failed !\n");
        }

        if (WIFEXITED(status)) {
            printf("The child process was normally termintated, status = %d\n", WIFEXITED(status));
        } else if (WIFSIGNALED(status)) {
            printf("The child process was killed by signal, status = %d\n", WIFSIGNALED(status));
        }
    } else {                    /* Error catch */
        printf("fork() failed !\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
