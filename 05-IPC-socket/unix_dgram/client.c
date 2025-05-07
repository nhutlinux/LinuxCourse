#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <ctype.h>
#include <unistd.h>

#define SOCKET_PATH "./dgram_socket"
#define BUFF_SIZE 256
#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(int argc, char *argv[])
{
    struct sockaddr_un serverAddr;
    int serverFd, optVal;
    ssize_t recvByteCount;
    socklen_t len;
    char readBuff[BUFF_SIZE];
    int msgLen;

    /* 01 - Create socket */
    serverFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (serverFd == -1) {
        handleError("socket()");
    }

    /* 01_1 - Initialize server information */
    memset(&serverAddr, 0, sizeof(struct sockaddr_un));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, SOCKET_PATH, sizeof(serverAddr.sun_path) - 1);

    /* Set socket option */
    optVal = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_PASSCRED, &optVal, sizeof(optVal)) == -1) {
        handleError("setsockopt()");
    }

    msgLen = strlen("Hello from client\n");
    if (sendto(serverFd, "Hello from client\n", msgLen, 0, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_un)) != msgLen) {
        handleError("sendto()");
    }
    recvByteCount = recvfrom(serverFd, readBuff, BUFF_SIZE, 0, NULL, NULL);
    if (recvByteCount == -1) {
        handleError("recvfrom()");
    } else {
        printf("Data from server: %s\n", readBuff);
    }
    
    // while(1);

    return 0;
}
