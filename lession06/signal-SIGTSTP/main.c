#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

/* Handle SIGTSTP signal */
void handleSIGTSTP()
{
    printf("\nSIGTSTP ignored\n");
}

int main()
{
    struct sigaction sa;

    /* Set up the structure to specify the new action */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handleSIGTSTP;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /* Register the signal handler for SIGTSTP */
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        fprintf(stderr, "Cannot handle SIGTSTP\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Running ... (Press Ctrl+Z to send SIGTSTP)\n");
    while(1){
        sleep(2);
        printf("Say hello every 2 seconds\n");
    }
    
    return 0;
}

// int main()
// {
//     /* Register SIGTSTP signal handle */
//     if (signal(SIGTSTP, handleSIGTSTP) == SIG_ERR) {
//         fprintf(stderr, "Cannot handle SIGTSTP\n");
//         exit(EXIT_FAILURE);
//     }

//     while(1){
//         sleep(2);
//         printf("Say hello every 2 seconds\n");
//     }

//     return 0;
// }