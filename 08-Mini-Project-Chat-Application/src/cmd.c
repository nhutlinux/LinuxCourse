#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "cmd.h"
#include "connect.h"
#include "msg.h"
#include "global_variables.h"
#include "utils.h"

/* List of commands */
const char *cmdTbl[] = {
    "help",         /* Print help content */
    "localip",      /* Print local IP */
    "localport",    /* Print local port */
    "connect",      /* Connect to remote peer */
    "list",         /* Print list of connected peers */
    "terminate",    /* Termitate connection from / to a peer */
    "send",         /* Send message to remote peer */
    "exit"          /* Exit app */
};

/* Check command */
void checkCmd(const char *cmd)
{
    char msgBuff[MAX_MSG_LEN + 1];
    char ip[MAX_IP_LEN];
    char cmdParams[256];
    peer_t *peer;
    int port = 0;
    int ret = 0;
    cmd_codes_t cmdCode = UNKNOWN;
    bool duplicate = false;
    int idx = -1;
    int id = -1;
    int i = 0;

    if (cmd == NULL) {
        return;
    }

    if (*cmd == '\0') {
        return;
    }

    memset(cmdParams, 0, sizeof(cmdParams));
    ret = sscanf(cmd, "%255s", cmdParams);
    if (ret != 1) {
        printf("Error: Invalid command !\n");
        return;
    }

    cmdCode = parseCmdCodes((const char*)cmdParams);
    switch (cmdCode) {
        case HELP:
            outputCmdList();
            break;

        case LOCALIP:
            outputLocalIp(curPeer.ip);
            break;

        case LOCALPORT:
            outputLocalPort(curPeer.portNo);
            break;

        case CONNECT:
            if (sscanf(cmd, "%*s %49s %d", ip, &port) < 2) {
                handleError("Invalid input. Format: connect <ip> <port>");
                break;
            }

            if (validateIp(ip) == false) {
                handleError("Invalid IP address\n");
                break;
            }

            if ((port <= 0) || (port > 65535)) {
                handleError("Invalid port number\n");
                break;
            }

            if (!strcmp(ip, curPeer.ip) && (port == curPeer.portNo)) {
                printf("Unable to connect to the same peer");
                break;
            }

            duplicate = false;
            idx = -1;
            pthread_mutex_lock(&peersMutex);

            for (i = 0; i < outgoingPeersCount; i++) {
                peer = &outgoingPeers[i];
                if ((peer->fd > 0) && (peer->isActive == true) && (!strcmp(peer->ip, ip)) && (peer->portNo == port)) {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate == false) {
                for (i = 0; i < MAX_PEER_NUM; i++) {
                    peer = &outgoingPeers[i];
                    if (peer->isActive == false) {
                        idx = i;
                        break;
                    }
                }
            }

            pthread_mutex_unlock(&peersMutex);

            if (duplicate) {
                handleError("Remote peer already connected\n");
                break;
            }

            if (idx == -1) {
                handleError("Maximum peer number is reached\n");
                break;
            }

            if (connectRemotePeer(&outgoingPeers[idx], ip, port, idx) == 0) {
                pthread_mutex_lock(&peersMutex);
                outgoingPeers[idx].isActive = true;
                outgoingPeersCount++;
                pthread_mutex_unlock(&peersMutex);
            }

            break;

        case LIST:
            outputPeersList();
            break;

        case SEND:
            if (sscanf(cmd, "%*s %d %256[^\n]", &id, msgBuff) < 2) {
                handleError("Invalid input. Format: send <id> <message>");
                break;
            }

            if (strlen(msgBuff) > MAX_MSG_LEN) {
                handleError("Message length is over the limitation");
                break;
            }

            peer = NULL;

            pthread_mutex_lock(&peersMutex);

            if (id >= MAX_PEER_NUM) {
                handleError("The remote peer is invalid\n");
                break;
            } else {
                for (i = 0; i < MAX_PEER_NUM; i++) {
                    if ((outgoingPeers[i].id == id) && (outgoingPeers[i].isActive == true)) {
                        peer = &outgoingPeers[i];
                        break;
                    }
                }
            }

            pthread_mutex_unlock(&peersMutex);

            if (peer == NULL) {
                handleError("The remote peer is invalid\n");
                break;
            }

            if (sendMsgToRemotePeer(peer, msgBuff) == 0) {
                printf("<Notify> Message was sent to the remote peer: %d\n", id);
            } else {
                handleError("Failed to send message");
            }

            break;

        case TERMINATE:
            if (sscanf(cmd, "%*s %d", &id) < 1) {
                handleError("Invalid input. Format: terminate <id>");
                break;
            }

            pthread_mutex_lock(&peersMutex);
            
            if (id >= MAX_PEER_NUM) {
                handleError("The remote peer is invalid\n");
                break;
            } else {
                for (i = 0; i < MAX_PEER_NUM; i++) {
                    if ((outgoingPeers[i].id == id) && (outgoingPeers[i].isActive == true)) {
                        peer = &outgoingPeers[i];
                        break;
                    }
                }
            }

            pthread_mutex_unlock(&peersMutex);

            if (peer == NULL) {
                printf("The remote peer: %d is invalid or terminated\n", id);
                break;
            }

            if (disconnectRemotePeer(peer) == 0) {
                printf("The remote peer: %d is invalid or terminated\n", id);
            } else {
                printf("Disconnect failed\n");
            }

            break;

        case EXIT:
            printf("Exiting application...\n");
            exit(EXIT_SUCCESS);
            break;

        default:
            printf("Error: Invalid command !\n");
            printf("Type [help] for list of valid commands.\n");
            break;
    }
}

/* Output list commands */
void outputCmdList()
{
    printf("*******************************Command List**********************************\n");
    printf("* help                         : Display all commands                       *\n");
    printf("* localip                      : Display local IP                           *\n");
    printf("* localport                    : Display local port                         *\n");
    printf("* connect <ip> <port_num>      : Connect to remote peer with IP and port    *\n");
    printf("* list                         : Display all connected peers                *\n");
    printf("* send <id> <message>          : Send message to peer with id               *\n");
    printf("* terminate <id>               : Disconnect with peer with id               *\n");
    printf("* exit                         : Exit application                           *\n");
    printf("*****************************************************************************\n");
}

/* Parse command codes */
cmd_codes_t parseCmdCodes(const char *cmd)
{
    cmd_codes_t ret = UNKNOWN;
    int i = 0;

    if (cmd == NULL) {
        return ret;
    }

    if (*cmd == '\0') {
        return ret;
    }

    while(1){
        if (cmdTbl[i] != NULL) {
            if (!strcmp(cmd, cmdTbl[i])) {
                ret = (cmd_codes_t)(i + 1);
                break;
            } else {
                i++;
            }
        } else {
            break;
        }
    }

    return ret;
}


