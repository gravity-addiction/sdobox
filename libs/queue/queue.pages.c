#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "shared.h"
#include "queue/queue.h"


void queueInitPageQueue(int ePage) {
  if (ePage < MAX_PAGES) {
    m_queue_pages[ePage] = ALLOC_QUEUE_ROOT();
    m_queue_len[ePage] = 0;
  }
}

void queueInitPageQueuesAll() {
  for (size_t ePage = 0; ePage < MAX_PAGES; ePage++) {
    queueInitPageQueue(ePage);
  }
}

void queueAddPageQueue(struct queue_head *new, int ePage) {
  if (ePage < MAX_PAGES) {
    queue_put(new, m_queue_pages[ePage], m_queue_len[ePage]);
  } else {
    // debug_print("Cannot Add to Queue, MAX_PAGES: %d, not long enough for ePage: %d\n", MAX_PAGES, ePage);
  }
}

struct queue_head *queueGetPageQueue(int ePage) {
  if (ePage < MAX_PAGES) {
    return queue_get(m_queue_pages[ePage], m_queue_len[ePage]);
  } else {
    // debug_print("Cannot Get from Queue, MAX_PAGES: %d, not long enough for ePage: %d\n", MAX_PAGES, ePage);
    struct queue_head *ret = malloc_aligned(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(ret);
    return ret;
  }
}
