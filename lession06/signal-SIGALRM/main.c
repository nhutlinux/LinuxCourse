#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int timeCount;

/* Handle SIGALRM signal */
void handleSIGALRM()
{
    printf("Timer: %d seconds\n", ++timeCount);
    if (timeCount == 10) {
        printf("Stop\n");
        exit(EXIT_SUCCESS);
    } else {
        /* Restart timer */
        alarm(1);
    }
}

int main()
{
    timeCount = 0;

    /* Register SIGALRM signal handler */
    if (signal(SIGALRM, handleSIGALRM) == SIG_ERR) {
        fprintf(stderr, "Cannot hande SIGALRM\n");
        exit(EXIT_FAILURE);
    }

    alarm(1);
    while(1) {}

    return 0;
}