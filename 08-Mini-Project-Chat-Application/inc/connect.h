#ifndef _CONNECT_H_
#define _CONNECT_H_

#include "type_def.h"

/* Initialize socket */
int initSocket(peer_t *peer, int port);

/* Connect to remote peer */
int connectRemotePeer(peer_t *remotePeer, char *remoteIp, int remotePort, int remoteId);

/* Disconnect from remote peer */
int disconnectRemotePeer(peer_t *remotePeer);

/* Output list of peers */
void outputPeersList();

/* Disconnect from all of peers */
void disconnectAllPeers();

/* Handle peer connect events */
void *handlePeerConnect(void *args);

#endif