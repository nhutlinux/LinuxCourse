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
#define SOCKET_PATH "./stream_socket"
#define BUFF_SIZE 256
#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(int argc, char *argv[])
{
    struct sockaddr_un serverAddr;
    int serverFd, newSocketFd;
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

    /* 02 - Binding */
    if (bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_un)) == -1) {
        handleError("bind()");
    }

    /* 03 - Listening */
    if (listen(serverFd, LISTEN_BACKLOG) == -1) {
        handleError("listen()");
    }

    /* 04 - Accept */
    while (1) {
        printf("Server is listening at path: %s \n ... \n", SOCKET_PATH);
        newSocketFd = accept(serverFd, (struct sockaddr*)&serverAddr, (socklen_t *)&len);
        if (newSocketFd == -1) {
            handleError("accept()");
        }
        printf("Server: got connection \n");
        write(newSocketFd, "Hello from server\n", sizeof("Hello from server\n"));
        close(newSocketFd);
    }
    close(serverFd);
    // remove(SOCKET_PATH);

    return 0;
}