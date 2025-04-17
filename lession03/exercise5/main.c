#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    pid_t pid;
    int retVal;
    int status;

    // /* Orphan process */
    // pid = fork();
    // if (pid == 0) {                         /* Child process */
    //     printf("This is the child process, my PID is: %d\n", getpid());
    //     sleep(30);
    //     exit(0);
    // } else if (pid > 0) {                   /* Parent process*/
    //     printf("This is the parent process, my PID is: %d\n", getpid());
    //     printf("Parent process terminates after 2 seconds\n");
    //     sleep(2);
    //     exit(0);
    // } else {
    //     printf("fork() failed !\n");
    // }

    /* Zombie process */
    pid = fork();
    if (pid == 0) {                         /* Child process */
        printf("This is the child process, my PID is: %d\n", getpid());
        printf("Child process terminates after 2 seconds\n");
        sleep(2);
        exit(0);
    } else if (pid > 0) {                   /* Parent process*/
        printf("This is the parent process, my PID is: %d\n", getpid());
        sleep(20);
        retVal = wait(&status);
        if (retVal == -1) {
            printf("wait() failed !\n");
        }

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