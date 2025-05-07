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
    int portNo;
    ssize_t recvByteCount;
    socklen_t len;
    char readBuff[BUFF_SIZE];
    char clientAddrBuff[INET_ADDRSTRLEN];

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
    serverFd = socket(AF_INET, SOCK_DGRAM, 0);
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

    /* Start listening */
    printf("Server is listening at port: %d \n ... \n", portNo);

    while (1) {
        len = sizeof(struct sockaddr_in);
        recvByteCount = recvfrom(serverFd, readBuff, BUFF_SIZE, 0, (struct sockaddr*)&clientAddr, &len);
        if (recvByteCount == -1) {
            return 1;
        }

        if (inet_ntop(AF_INET, &clientAddr.sin_addr, clientAddrBuff, INET_ADDRSTRLEN) == NULL) {
            printf("Cannot convert client address to string\n");
        } else {
            printf("Server received %ld bytes from (%s, %u) \n", (long)recvByteCount, clientAddrBuff, ntohs(clientAddr.sin_port));
            printf("Data from client: %s\n", readBuff);
        }

        if (sendto(serverFd, "Hello from server\n", sizeof("Hello from server\n"), 0, (struct sockaddr*)&clientAddr, len) != sizeof("Hello from server\n")) {
            printf("sento error\n");
        }
    }

    close(serverFd);
    return 0;
}