#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 256
#define handleError(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddr;
    char readBuff[BUFF_SIZE];
    int serverFd;
    int portNo;
    size_t msgLen;
    ssize_t recvByteCount;

    /* Command line argument: server address, port number */
    if (argc < 3) {
        printf("Error: Server address and / or server port is not specified.\nCommand: ./client <server address> <port number>\n");
        exit(EXIT_FAILURE);
    } else {
        portNo = atoi(argv[2]);
    }
    memset(&serverAddr, 0, sizeof(struct sockaddr_in));

    /* 00 - Initialize server information */
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNo);
    if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) == -1) {
        handleError("inet_pton()");
    }

    /* 01 - Create socket */
    serverFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverFd == -1) {
        handleError("socket()");
    }

    msgLen = strlen("Hello from client\n");
    if (sendto(serverFd, "Hello from client\n", msgLen, 0, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) != msgLen) {
        return 1;
    }
    recvByteCount = recvfrom(serverFd, readBuff, BUFF_SIZE, 0, NULL, NULL);
    if (recvByteCount == -1) {
        return 1;
    } else {
        printf("Data from server: %s\n", readBuff);
    }

    // while(1);
    
    return 0;
}