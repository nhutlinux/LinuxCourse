#include "type_def.h"
#include "const_def.h"



/* Current peer information */
peer_t curPeer;

/* Incoming peers information */
peer_t incomingPeers[MAX_PEER_NUM];
int incomingPeersCount;

/* Outgoing peers information */
peer_t outgoingPeers[MAX_PEER_NUM];
int outgoingPeersCount;

/* Accept thread ID */
pthread_t acceptThreadId;

/* Peer mutex */
pthread_mutex_t peersMutex = PTHREAD_MUTEX_INITIALIZER;
