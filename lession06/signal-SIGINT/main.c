#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int sigIntCount;

/* Handle signal SIGINT */
void handleSIGINT(int num)
{
    printf("\nSIGINT Received: %d\n", ++sigIntCount);
    if (sigIntCount == 3) {
        printf("Stop\n");
        exit(EXIT_SUCCESS);
    } else {
        printf("Continue\n");
    }
}

int main()
{
    sigIntCount = 0;

    /* Register signal handler */
    if (signal(SIGINT, handleSIGINT) == SIG_ERR) {
        fprintf(stderr, "Cannot handle SIGINT\n");
        exit(EXIT_FAILURE);
    }

    printf("process ID: %d\n", getpid());
    while (1) {
        printf("Say hello every 2 seconds\n");
        sleep(2);
    }

    return 0;
}