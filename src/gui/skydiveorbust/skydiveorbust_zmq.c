#include <stdlib.h>

#include "./skydiveorbust_zmq.h"
#include "libs/zhelpers/zhelpers-tx.h"
#include "libs/zhelpers/zhelpers.h"
#include "libs/queue/queue.h"

// Add Queue Item to SDOB Worker Inproc 0MQ
void pg_sdob_add_action(struct queue_head **head) {
  zmq_send(pg_sdobWorker, head, sizeof(head), 0);
}
