#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include "const_def.h"

/* Peer's information structure variable */
typedef struct {
    int fd;
    int id;
    int portNo;
    char ip[MAX_IP_LEN];
    struct sockaddr_in addr;
    pthread_t recvThread;
    bool isActive; 
} peer_t;

/* Command codes definition */
typedef enum {
    HELP = 1,
    LOCALIP,
    LOCALPORT,
    CONNECT,
    LIST,
    TERMINATE,
    SEND,
    EXIT,
    UNKNOWN = -1
} cmd_codes_t;

#endif