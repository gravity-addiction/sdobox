#ifndef _SDOBOX_SDOB_ZMQ_H_
#define _SDOBOX_SDOB_ZMQ_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "libs/queue/queue.h"

void *pg_sdobWorker; // ZMQ sdobQueue replacement `zmq_send(pg_sdobWorker, head, sizeof(head), 0);`

// Add Queue Item to SDOB Worker Inproc 0MQ
void pg_sdob_add_action(struct queue_head **head);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _SDOBOX_SDOB_ZMQ_H_