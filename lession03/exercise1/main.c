#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    pid_t pid;

    pid = fork();
    if (pid == 0) {                         /* Child process */
        printf("This is the child process, my PID is: %d\n", getpid());
    } else if (pid > 0) {                   /* Parent process*/
        printf("This is the parent process, my PID is: %d\n", getpid());
    } else {
        printf("fork() failed !\n");
    }

    return 0;
}