#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "msg.h"
#include "global_variables.h"
#include "utils.h"

/* Send message to remote peer */
int sendMsgToRemotePeer(peer_t *peer, const char *msg)
{
    size_t msgLen;
    int fd = -1;
    bool active = false;
    ssize_t sendCount = 0;

    if (peer == NULL) {
        handleError("Invalid remote peer\n");
        return -1;
    }

    if (msg == NULL) {
        handleError("Invalid message\n");
        return -1;
    }

    msgLen = strlen(msg);
    if (msgLen > MAX_MSG_LEN) {
        handleError("Message length (%d) is over the limitation\n");
        return -1;
    }

    pthread_mutex_lock(&peersMutex);
    fd = peer->fd;
    active = peer->isActive;
    pthread_mutex_unlock(&peersMutex);

    if ((fd < 0) || (active == false)) {
        handleError("Remote peer is not reachable\n");
        return -1;
    }

    sendCount = write(fd, msg, msgLen);
    if (sendCount < 0) {
        handleError("Cannot send message\n");
        return -1;
    }

    if ((size_t)sendCount < msgLen) {
        handleError("Message was corrupted\n");
        return -1;
    }

    return 0;
}

/* Handle message received event */
void *handleMsgRecv(void *args)
{
    char msgBuff[MAX_MSG_LEN + 1];
    int fd = -1;
    char ip[MAX_IP_LEN];
    int port = -1;
    bool found = false;
    int i = 0;

    peer_t *peer = (peer_t *)args;
    if (peer == NULL) {
        handleError("Invalid peer from receive handler's input parameter\n");
        return NULL;
    }

    fd = peer->fd;
    port = peer->portNo;
    strncpy(ip, peer->ip, MAX_IP_LEN);
    ip[MAX_MSG_LEN - 1] = '\0';

    free(peer);

    while (1) {
        memset(msgBuff, 0, sizeof(msgBuff));

        ssize_t readCount = read(fd, msgBuff, MAX_MSG_LEN);
        if (readCount <= 0) {
            if ((readCount < 0) && (errno == EINTR)) {
                continue;
            }
    
            pthread_mutex_lock(&peersMutex);
    
            for (i = 0; i < incomingPeersCount; i++) {
                peer = &incomingPeers[i];
                if (peer->fd == fd) {
                    peer->isActive = false;
                    peer->fd = -1;
                    found = true;
                    incomingPeersCount--;
                    break;
                }
            }
    
            if (found == false) {
                for (i = 0; i < outgoingPeersCount; i++) {
                    peer = &outgoingPeers[i];
                    if (peer->fd == fd) {
                        peer->isActive = false;
                        peer->fd = -1;
                        found = true;
                        outgoingPeersCount--;
                        break;
                    }
                }
            }
    
            pthread_mutex_unlock(&peersMutex);
            
            if (found == true) {
                printf("\nRemote peer: %s:%d has been disconnected\n", ip, port);
            } else {
                printf("\nUnrecognized remote peer\n");
            }
    
            printf("\nEnter command: ");
            fflush(stdout);
    
            close(fd);
            break;
        }
        
        msgBuff[readCount] = '\0';

        printf("\n");
        printf("*** Received new message from peer: %s:%d\n", ip, port);
        printf("*** Content:                        %s\n", msgBuff);
        printf("\nEnter command: ");
        fflush(stdout);
    }

    return NULL;
}

