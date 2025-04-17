#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

/* Declare siguser1 processing routine */
void sigusr1();
// static void *sigusr1(void* args);

/* Main routine */
int main(int argc, char const *argv[])
{
    pid_t pid;
    int childPID;
    int retVal;
    int status;
 
    pid = fork();
    if (pid == 0) {                         /* Child process */
        childPID = getpid();
        signal(SIGUSR1, sigusr1);
        // signal(SIGUSR1, sigusr1(&childPID));
        while(1);
        // exit(0);
    } else if (pid > 0) {                   /* Parent process*/
        printf("This is the parent process, my PID is: %d\n", getpid());
        printf("Send SIGUSR1 to child process (PID = %d) after 5 seconds\n", pid);
        sleep(5);
        kill(pid, SIGUSR1);
        // retVal = wait(&status);
        // if (WIFEXITED(status)) {
        //     printf("The child process was normally termintated, status = %d\n", WIFEXITED(status));
        // } else if (WIFSIGNALED(status)) {
        //     printf("The child process was killed by signal, status = %d\n", WIFSIGNALED(status));
        // }
    } else {
        printf("fork() failed !\n");
    }

    return 0;
}

/* Define siguser1 processing routine */
void sigusr1()
{

    printf("This is the child process\n");

}
// static void *sigusr1(void* args)
// {
//     int pid;
    
//     pid = *((int*)args);
//     printf("This is the child process, my PID is: %d\n", pid);

// }


