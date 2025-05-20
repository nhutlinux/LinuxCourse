#ifndef _MSG_H_
#define _MSG_H_

#include "type_def.h"

/* Send message to remote peer */
int sendMsgToRemotePeer(peer_t *peer, const char *msg);

/* Handle message received event */
void *handleMsgRecv(void *args);

#endif