#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    pid_t pid;
    int retVal;
    int status;

    pid = fork();
    if (pid == 0) {                         /* Child process */
        printf("This is the child process, my PID is: %d\n", getpid());
        // sleep(20);
        // while(1);
        exit(0);
    } else if (pid > 0) {                   /* Parent process*/
        retVal = wait(&status);
        printf("This is the parent process, my child process's PID is: %d\n", retVal);

        if (WIFEXITED(status)) {
            printf("The child process was normally termintated, status = %d\n", WIFEXITED(status));
        } else if (WIFSIGNALED(status)) {
            printf("The child process was killed by signal, status = %d\n", WIFSIGNALED(status));
        }
    } else {
        printf("fork() failed !\n");
    }

    return 0;
}