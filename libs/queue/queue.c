#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "queue.h"

// original source @ https://idea.popcount.org/2012-09-11-concurrent-queue-in-c/

#define QUEUE_POISON1 ((void*)0xCAFEBAB5)

// int queue_size = 0;

struct queue_root {
    struct queue_head head;     /* base of doubly-linked circular list */
    pthread_mutex_t lock;
};

struct queue_root *ALLOC_QUEUE_ROOT()
{
    struct queue_root *root =   malloc(sizeof(struct queue_root));

    pthread_mutex_init(&root->lock, NULL);

    // Make the head point to itself in both directions
    root->head.next = &root->head;
    root->head.prev = &root->head;

    return root;
}

void INIT_QUEUE_HEAD(struct queue_head *head)
{
    head->next = QUEUE_POISON1;
    head->prev = QUEUE_POISON1;
    head->action = -1;
    head->mark = -1;
    head->selected = -1;
    head->milli = -1;
    head->time = -1.0;
    head->amt = 0.0;
//  head->key = calloc(64, sizeof(char));
//  head->cmd = calloc(256, sizeof(char));
}

// #define QUEUE_DEBUG_P

#ifdef QUEUE_DEBUG_P
static bool on_queue_p(struct queue_root* root, struct queue_head* x) {
  struct queue_head* p = root->head.next;
  while(p != &root->head) {
    if (p == x)
      return true;
    p = p->next;
  }
  return false;
}

static unsigned queue_length(struct queue_root* root) {
  struct queue_head* p = root->head.next;
  unsigned count = 0;
  while(p != &root->head) {
    ++count;
    p = p->next;
    if (count > 100000)
      return count;
  }
  return count;
}
#endif

void queue_put(struct queue_head *new,
               struct queue_root *root, size_t *len)
{
  if (m_bQuit) { return; }

  pthread_mutex_lock(&root->lock);

#ifdef QUEUE_DEBUG_P
  if (on_queue_p(root, new)) {
    printf("\nqueue_put: point already on queue! %p\n\n", new);
    pthread_mutex_unlock(&root->lock);
    return;
  }
#endif

  struct queue_head *head = &root->head;

  new->prev = head;
  new->next = head->next;
  head->next = new;
  new->next->prev = new;

  (*len)++;

#ifdef QUEUE_DEBUG_P
  if (*len != queue_length(root))
    printf("\nqueue_put: QUEUE LENGTH INCORRECT: %u != %u\n\n", *len, queue_length(root));
#endif

  pthread_mutex_unlock(&root->lock);
}


struct queue_head *queue_get(struct queue_root *root, size_t *len)
{
  if (m_bQuit) { return NULL; }

  pthread_mutex_lock(&root->lock);

  struct queue_head *head = &root->head;
  struct queue_head *result = NULL;
  if (head->prev != head) {
    result = head->prev;
    head->prev = result->prev;
    head->prev->next = head;
    result->next = QUEUE_POISON1;
    result->prev = QUEUE_POISON1;
    (*len)--;

#ifdef QUEUE_DEBUG_P
        if (*len != queue_length(root))
            printf("\nqueue_get: QUEUE LENGTH INCORRECT: %u != %u\n\n", *len, queue_length(root));
        if (on_queue_p(root, result)) {
            printf("queue_get, after removal, result is still on queue! %p\n\n", result);
        }
        if (*len > 0)
            // somewhat interesting to see cases where events pile up
            printf("queue_get: upon returning item, %u items remain\n", *len);
#endif
    }
    pthread_mutex_unlock(&root->lock);
    return result;
}
