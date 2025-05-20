#ifndef _GLOBAL_VARIABLES_H_
#define _GLOBAL_VARIABLES_H_

#include "type_def.h"
#include "const_def.h"

/* Current peer information */
extern peer_t curPeer;

/* Incoming peers information */
extern peer_t incomingPeers[MAX_PEER_NUM];
extern int incomingPeersCount;

/* Outgoing peers information */
extern peer_t outgoingPeers[MAX_PEER_NUM];
extern int outgoingPeersCount;

/* Accept thread ID */
extern pthread_t acceptThreadId;

/* Peer mutex */
extern pthread_mutex_t peersMutex;

#endif