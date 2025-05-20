#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include "const_def.h"
#include "global_variables.h"
#include "cmd.h"
#include "msg.h"
#include "connect.h"
#include "utils.h"

/* Handler for signal */
void handleSignal(int sig)
{
    if (sig == SIGINT) {
        printf("\nApplication terminated...\n");
        printf("Bye\n");
        exit(EXIT_SUCCESS);
    }
}

/* Chat application main routine */
int main(int argc, char *argv[])
{
    char cmd[256];
    peer_t *peer;
    pid_t pid;
    int retVal;
    int status;
    int port;
    int i = 0;

    if (argc != 2) {
        printf("Command format: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < MAX_PEER_NUM; i++) {
        peer = &incomingPeers[i];
        peer->fd = -1;
        peer->isActive = false;

        peer = &outgoingPeers[i];
        peer->fd = -1;
        peer->isActive = false;
    }
    incomingPeersCount = 0;
    outgoingPeersCount = 0;

    if (signal(SIGINT, handleSignal) == SIG_ERR) {
        handleError("signal()");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);
    if ((port <=0) || (port > 65535)) {
        handleError("Invalid port numer");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == 0) {
        execl("/usr/bin/figlet", "figlet", "Chat Application", NULL);
        sleep(1);
        exit(0);
    } else if (pid > 0) {
        retVal = wait(&status);
        if (retVal == -1) {
            handleError("wait()");
            return -1;
        }

        if (WIFSIGNALED(status)) {
            printf("The child process was killed by signal, status = %d\n", WIFSIGNALED(status));
        }
    } else {
        handleError("fork()");
        return -1;
    }

    outputCmdList();
    
    if (initSocket(&curPeer, port) == -1) {
        handleError("Cannot initialize socket");
        exit(EXIT_FAILURE);
    }

    if (getLocalIp(curPeer.ip, MAX_IP_LEN) == false) {
        handleError("Cannot get curret local IP");
        exit(EXIT_FAILURE);
    } 

    if (pthread_create(&acceptThreadId, NULL, handlePeerConnect, NULL)) {
        handleError("Cannot create thread for accepting new devices");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter command: ");
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            break;
        }

        checkCmd(cmd);
    }

    return 0;
}