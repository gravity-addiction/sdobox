#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>             /* malloc */
#include <unistd.h>             /* useconds_t */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
// This is a queue element, populated with varied useful fields.
//
typedef union {
  void* ptr;
  int integer;
  unsigned uinteger;
  double floating;
} queue_any_u;

struct queue_head {
  struct queue_head *next;
  struct queue_head *prev;
  int action;
  int mark;
  int milli;
  double time;
  int selected;
  double amt;
  char *cmd;
  queue_any_u u1;
  queue_any_u u2;
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

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // QUEUE_H
