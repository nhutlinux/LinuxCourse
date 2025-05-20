#ifndef _UTILS_H_
#define _UTILS_H_

#include <unistd.h>
#include <stdbool.h>

/* Handle error */
void handleError(const char *msg);

/* Get local IP address */
bool getLocalIp(char *dPos, size_t buffLen);

/* Output local IP address */
void outputLocalIp(char *sPos);

/* Ouput local port */
void outputLocalPort(int port);

/* Valicate IP address */
bool validateIp(char *sPos);

#endif