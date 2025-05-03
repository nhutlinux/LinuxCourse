#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <ctype.h>
#include <unistd.h>

#define LISTEN_BACKLOG 20
#define SOCKET_PATH "./dgram_socket"
#define BUFF_SIZE 256
#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(int argc, char *argv[])
{
    struct sockaddr_un serverAddr, clientAddr;
    int serverFd;
    ssize_t recvByteCount;
    socklen_t len;
    char readBuff[BUFF_SIZE];

    /* 01 - Create socket */
    serverFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (serverFd == -1) {
        handleError("socket()");
    }

    /* 01_1 - Initialize server information */
    memset(&serverAddr, 0, sizeof(struct sockaddr_un));
    memset(&clientAddr, 0, sizeof(struct sockaddr_un));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, SOCKET_PATH, sizeof(serverAddr.sun_path) - 1);

    /* 02 - Binding */
    if (bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_un)) == -1) {
        handleError("bind()");
    }

    /* Start listening */
    printf("Server is listening at path: %s \n ... \n", SOCKET_PATH);

    while (1) {
        len = sizeof(struct sockaddr_un);
        recvByteCount = recvfrom(serverFd, readBuff, BUFF_SIZE, 0, (struct sockaddr*)&clientAddr, &len);
        if (recvByteCount == -1) {
            handleError("recvfrom()");
        }

        printf("Server received %ld bytes from %s \n", (long)recvByteCount, clientAddr.sun_path);
        printf("Data from client: %s\n", readBuff);

        if (sendto(serverFd, "Hello from server\n", sizeof("Hello from server\n"), 0, (struct sockaddr*)&clientAddr, len) != sizeof("Hello from server\n")) {
            printf("sento error\n");
        }
    }

    close(serverFd);
    // remove(SOCKET_PATH);

    return 0;
}