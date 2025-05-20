#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "global_variables.h"
#include "utils.h"

#define MIN_IP_LEN  16

/* Handle error */
void handleError(const char *msg)
{
    perror(msg); 
    // exit(EXIT_FAILURE);
}

/* Get local IP address */
bool getLocalIp(char *dPos, size_t buffLen)
{
    size_t len = 0;

    if (dPos == NULL) {
        return false;
    }

    if (buffLen < MIN_IP_LEN) {
        return false;
    }

    FILE *pipe = popen("hostname -I | awk '{print $1}'", "r");
    if (!pipe) {
        handleError("popen()");
        return false;
    }

    if (fgets(dPos, buffLen, pipe) != NULL) {
        len = strlen(dPos);
        if ((len > 0) && (*(dPos + len - 1) == '\n')){
            *(dPos + len - 1) = '\0';
        }
    } else {
        len = strlen("127.0.0.1");
        strncpy(dPos, "127.0.0.1", len);
        *(dPos + len) = '\0';
    }

    pclose(pipe);

    return true;
}

/* Output local IP address */
void outputLocalIp(char *sPos)
{
    if (sPos == NULL) {
        return;
    }

    if (*sPos == '\0') {
        return;
    }

    printf("Local IP is: %s\n", sPos);
}

/* Ouput local port */
void outputLocalPort(int port)
{
    printf("Local port is: %d\n", port);
}

/* Valicate IP address */
bool validateIp(char *sPos)
{
    struct sockaddr_in addr;

    if (sPos == NULL) {
        return false;
    }

    if (*sPos == '\0') {
        return false;
    }

    if (inet_pton(AF_INET, sPos, &(addr.sin_addr)) == 1) {
        return true;
    }

    return false;
}

