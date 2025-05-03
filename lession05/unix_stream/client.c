#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <ctype.h>
#include <unistd.h>

#define SOCKET_PATH "./stream_socket"
#define BUFF_SIZE 256
#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(int argc, char *argv[])
{
    struct sockaddr_un serverAddr;
    int serverFd;
    ssize_t recvByteCount;
    socklen_t len;
    char readBuff[BUFF_SIZE];

    /* 01 - Create socket */
    serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd == -1) {
        handleError("socket()");
    }

    /* 01_1 - Initialize server information */
    memset(&serverAddr, 0, sizeof(struct sockaddr_un));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, SOCKET_PATH, sizeof(serverAddr.sun_path) - 1);

    /* Connect */
    if (connect(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        handleError("connect()");
    }

    read(serverFd, readBuff, BUFF_SIZE);
    printf("Data from server: %s\n", readBuff);

    while(1);
    
    return 0;
}
