#ifndef QUEUE_H
#define QUEUE_H

#include <unistd.h>
#include "gui/pages.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int queue_size;

struct queue_head {
  struct queue_head *next;
  struct queue_head *prev;
  int action;
  int mark;
  int milli;
  double time;
  int selected;
  double amt;
  char *key;
  char *cmd;
};

struct queue_root *ALLOC_QUEUE_ROOT();
void INIT_QUEUE_HEAD(struct queue_head *head);

static inline struct queue_head* new_qhead() {
  struct queue_head* head = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(head);
  return head;
}

void queue_put(struct queue_head *new,
               struct queue_root *root, size_t *len);

struct queue_head *queue_get(struct queue_root *root, size_t *len);
struct queue_head *queue_get_wtimeout(struct queue_root *root, size_t *len, useconds_t usecs);

// queue.pages.c
struct queue_root *m_queue_pages[MAX_PAGES];
size_t *m_queue_len[MAX_PAGES];
void queueInitPageQueue();
void queueAddPageQueue(struct queue_head *new, int ePage);
struct queue_head *queueGetPageQueue(int ePage);


// queue.thread.c
int m_bQueueThreadStop; //  = 0; // Stopping mpvRpcSocket
int m_bQueueThreadRunning; // = 0; // Running flag for Mpv RPC

int queueThreadStart();
void queueThreadStop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // QUEUE_H
