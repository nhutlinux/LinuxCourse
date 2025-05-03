#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LISTEN_BACKLOG  30
#define BUFF_SIZE 256
#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddr, clientAddr;
    int serverFd, newSocketFd;
    int portNo, len;

    /* Command line argument: port number */
    if (argc < 2) {
        printf("Error: Server port is not specified.\nCommand: ./server <port number>\n");
        exit(EXIT_FAILURE);
    } else {
        portNo = atoi(argv[1]);
    }
    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    memset(&clientAddr, 0, sizeof(struct sockaddr_in));

    /* 01 - Create socket */
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        handleError("socket()");
    }

    /* 01_1 - Initialize server information */
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNo);
    serverAddr.sin_addr.s_addr = INADDR_ANY;    //inet_addr("192.168.1.203");

    /* 02 - Binding */
    if (bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        handleError("bind()");
    }

    /* 03 - Listening */
    if (listen(serverFd, LISTEN_BACKLOG) == -1) {
        handleError("listen()");
    }

    /* Calculate size of clientAddr */
    len = sizeof(clientAddr);
    while (1) {
        printf("Server is listening at port: %d \n ... \n", portNo);
        /* 04 - Accept */
        newSocketFd = accept(serverFd, (struct sockaddr*)&clientAddr, (socklen_t *)&len);
        if (newSocketFd == -1) {
            handleError("accept()");
        }

        printf("Server: got connection \n");
        write(newSocketFd, "hello\n", sizeof("hello\n"));
        close(newSocketFd);
    }
    close(serverFd);
    return 0;
}