#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "global_variables.h"
#include "connect.h"
#include "msg.h"
#include "utils.h"

/* Initialize socket */
int initSocket(peer_t *peer, int port)
{
    int socketOption = -1;

    if (peer == NULL) {
        printf("Invalid remote peer\n");
        return -1;
    }

    peer->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (peer->fd == -1) {
        handleError("socket()");
        return -1;
    }

    if (setsockopt(peer->fd, SOL_SOCKET, SO_REUSEADDR, &socketOption, sizeof(socketOption)) != 0) {
        handleError("setsockopt()");
        close(peer->fd);
        return -1;
    }

    peer->portNo = port;
    peer->addr.sin_family = AF_INET;
    peer->addr.sin_addr.s_addr = INADDR_ANY;
    peer->addr.sin_port = htons(peer->portNo);
    peer->isActive = true;

    if (bind(peer->fd, (struct sockaddr*)&peer->addr, sizeof(peer->addr)) == -1) {
        handleError("bind()");
        close(peer->fd);
        return -1;
    }

    if (listen(peer->fd, MAX_PEER_NUM) == -1) {
        handleError("listen()");
        close(peer->fd);
        return -1;
    }

    printf("Listening on port: %d\n", peer->portNo);

    return 0;
}

/* Connect to remote peer */
int connectRemotePeer(peer_t *remotePeer, char *remoteIp, int remotePort, int remoteId)
{
    struct sockaddr_in addr;
    pthread_t recvThread;
    int fd = -1;

    if (remotePeer == NULL) {
        printf("Invalid remote peer\n");
        return -1;
    }

    if (remoteIp == NULL) {
        printf("Invalid remote IP\n");
        return -1;
    }

    if (*remoteIp == '\0') {
        printf("Invalid remote IP value\n");
        return -1;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        handleError("socket()");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(remotePort);

    if (inet_pton(AF_INET, remoteIp, &addr.sin_addr.s_addr) <= 0) {
        handleError("Invalid remote IP address");
        close(fd);
        return -1;
    }

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        handleError("connect()");
        close(fd);
        return -1;
    } else {
        remotePeer->fd = fd;
        remotePeer->id = remoteId;
        remotePeer->portNo = remotePort;
        strncpy(remotePeer->ip, remoteIp, MAX_IP_LEN - 1);
        remotePeer->ip[MAX_IP_LEN - 1] = '\0';
        remotePeer->addr = addr;

        printf("<Notify> Connected to remote IP: %s, remote port: %d\n", remoteIp, remotePort);

        peer_t *peer = malloc(sizeof(peer_t));
        if (peer == NULL) {
            handleError("Cannot allocate memory for remote peer\n");
            close(fd);
            return -1;
        }

        memcpy(peer, remotePeer, sizeof(peer_t));

        if (pthread_create(&recvThread, NULL, handleMsgRecv, peer)) {
            handleError("Cannot create thread to receive message from remote peer\n");
            close(fd);
            free(peer);
            return -1;
        }

        remotePeer->recvThread = recvThread;
        pthread_detach(recvThread);
    }

    return 0;
}

/* Disconnect from remote peer */
int disconnectRemotePeer(peer_t *remotePeer)
{
    char msgBuff[MAX_MSG_LEN];

    if (remotePeer == NULL) {
        printf("Invalid remote peer\n");
        return -1;
    }

    if (remotePeer->fd < 0) {
        printf("Invalid remote peer value\n");
        return -1;
    }

    if (remotePeer->isActive == false) {
        printf("Remote peer is not actived\n");
        return -1;
    }

    snprintf(msgBuff, sizeof(msgBuff), "Remote peer terminated the connection !");
    sendMsgToRemotePeer(remotePeer, msgBuff);

    close(remotePeer->fd);
    remotePeer->fd = -1;
    remotePeer->isActive = false;

    return 0;
}

/* Output list of peers */
void outputPeersList()
{
    peer_t *peer;
    int i = 0;

    pthread_mutex_lock(&peersMutex);

    printf("********************* List of incoming peers *******************\n");
    printf("*   ID   |            IP address            |    Port number   *\n");
    printf("*********|**********************************|*******************\n");
    
    for (i = 0; i < incomingPeersCount; i++) {
        peer = &incomingPeers[i];
        if ((peer->fd > 0) && (peer->isActive == true)) {
            printf("*   %02d   |    %25s     |    %d\n", peer->id, peer->ip, peer->portNo);
        }
    }

    printf("****************************************************************\n");
    printf("\n\n");
    printf("********************* List of outgoing peers *******************\n");
    printf("*   ID   |            IP address            |    Port number   *\n");
    printf("*********|**********************************|*******************\n");
    
    for (i = 0; i < outgoingPeersCount; i++) {
        peer = &outgoingPeers[i];
        if ((peer->fd > 0) && (peer->isActive == true)) {
            printf("*   %02d   |    %25s     |    %d\n", peer->id, peer->ip, peer->portNo);
        }
    }

    printf("****************************************************************\n");

    pthread_mutex_unlock(&peersMutex);
}

/* Disconnect from all of peers */
void disconnectAllPeers()
{
    peer_t *peer;
    int i = 0;

    pthread_mutex_lock(&peersMutex);

    for (i = 0; i < incomingPeersCount; i++) {
        peer = &incomingPeers[i];
        if ((peer->fd > 0) && (peer->isActive == true)) {
            close(peer->fd);
            peer->fd = -1;
            peer->isActive = false;
        }
    }

    incomingPeersCount = 0;

    for (i = 0; i < outgoingPeersCount; i++) {
        peer = &outgoingPeers[i];
        if ((peer->fd > 0) && (peer->isActive == true)) {
            close(peer->fd);
            peer->fd = -1;
            peer->isActive = false;
        }
    }

    outgoingPeersCount = 0;

    pthread_mutex_unlock(&peersMutex);
}

/* Handle peer connect events */
void *handlePeerConnect(void *args)
{
    struct sockaddr_in remoteAddr;
    socklen_t remoteLen = sizeof(remoteAddr);
    peer_t *peer = &curPeer;
    int fd = -1;
    int idx = -1;
    int i = 0;

    while (1) {
        peer = &curPeer;
        fd = accept(peer->fd, (struct sockaddr *)&remoteAddr, &remoteLen);
        if (fd == -1) {
            handleError("accept(): Remote peer connection failed");
            continue;
        }

        idx = -1;
        pthread_mutex_lock(&peersMutex);

        for (i = 0; i < MAX_PEER_NUM; i++) {
            if (incomingPeers[i].isActive == false) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            pthread_mutex_unlock(&peersMutex);
            close(fd);
            handleError("Maximum incoming peers reached\n");
            continue;
        }

        peer = &incomingPeers[idx];
        peer->fd = fd;
        peer->id = idx;
        peer->addr = remoteAddr;
        peer->portNo = ntohs(remoteAddr.sin_port);
        peer->isActive = true;
        inet_ntop(AF_INET, &remoteAddr.sin_addr.s_addr, peer->ip, MAX_IP_LEN);

        peer = malloc(sizeof(peer_t));
        if (peer == NULL) {
            peer = &incomingPeers[idx];
            peer->isActive = false;
            pthread_mutex_unlock(&peersMutex);
            close(fd);
            handleError("Cannot allocate memory for remote peer\n");
            continue;
        }

        memcpy(peer, &incomingPeers[idx], sizeof(peer_t));
        incomingPeersCount++;

        pthread_mutex_unlock(&peersMutex);

        pthread_t recvThread;
        if (pthread_create(&recvThread, NULL, handleMsgRecv, peer) != 0) {
            handleError("Cannot create thread to receive message from remote peer\n");
            free(peer);

            pthread_mutex_lock(&peersMutex);
            incomingPeers[idx].isActive = false;
            incomingPeersCount--;
            pthread_mutex_unlock(&peersMutex);

            close(fd);
            continue;
        }

        incomingPeers[idx].recvThread = recvThread;
        pthread_detach(recvThread);

        peer = &incomingPeers[idx];
        printf("\n<Notify> New connection from %s:%d\n", peer->ip, peer->portNo);
        printf("Enter command: ");
        fflush(stdout);
    }

    return NULL;
}


