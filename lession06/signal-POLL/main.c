#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <poll.h>

#define BUFF_SIZE   256
#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

volatile sig_atomic_t sigIntReceived = 0;

/* Handle SIGINT signal */
void handleSIGINT()
{
    sigIntReceived = 1;
}

/* Main */
int main()
{
    struct pollfd fds[1];
    char inputBuff[BUFF_SIZE];
    int activity;

    /* Register SIGINT signal handler */
    if (signal(SIGINT, handleSIGINT) == SIG_ERR) {
        handleError("signal()");
        exit(EXIT_FAILURE);
    }

    printf("Running ... (Press Ctrl+C to send SIGINT, press other key to input)\n");

    /* Initialize pollfd structure */
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    /* Main loop */
    while (1) {
        /* Wait for input or signal */
        activity = poll(fds, 1, -1); /* Wait indefinitely */
        if ((activity < 0) && (errno != EINTR)) {
            handleError("poll()");
            exit(EXIT_FAILURE);
        }

        /* Process input */
        if (activity >= 0) {
            if (fds[0].events & POLLIN) {
                if (fgets(inputBuff, sizeof(inputBuff), stdin) != NULL) {
                    /* Remove the newline character from input string */
                    inputBuff[strcspn(inputBuff, "\n")] = 0;
                    printf("You entered: %s\n", inputBuff);
                } else {
                    handleError("fgets()");
                    exit(EXIT_FAILURE);
                }
            }
        }

        /* Process signal */
        if (sigIntReceived) {
            printf("\nSIGINT received\n");
            sigIntReceived = 0;
        }
    }
    return 0;
}
