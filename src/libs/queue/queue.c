#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

#include "queue.h"
#include "libs/shared.h"             /* m_bQuit */

// #define QUEUE_DEBUG_P

// original source @ https://idea.popcount.org/2012-09-11-concurrent-queue-in-c/

#define QUEUE_POISON1 ((void*)0xCAFEBAB5)

struct queue_root {
  struct queue_head head;     /* base of doubly-linked circular list */
  pthread_mutex_t lock;
  pthread_cond_t not_empty_notify;
};

struct queue_root *ALLOC_QUEUE_ROOT()
{
  struct queue_root *root =   malloc(sizeof(struct queue_root));

  pthread_mutex_init(&root->lock, NULL);
  pthread_cond_init(&root->not_empty_notify, NULL);

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

    // If corruption causes a cycle that does not pass through head,
    // quit here and the special value 100001 should be a clue.
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

  if (new->next == head) {
    assert(*len == 1);
    pthread_cond_signal(&root->not_empty_notify);
  }
  else {
    assert(*len > 1);
  }

  pthread_mutex_unlock(&root->lock);
}


struct queue_head *queue_get(struct queue_root *root, size_t *len) {
  return queue_get_wtimeout(root,len,0);
}

#ifdef QUEUE_DEBUG_P
static useconds_t tselapsed(struct timespec* start, struct timespec* end) {
  return (end->tv_nsec - start->tv_nsec) / 1000 + (end->tv_sec - start->tv_sec) * 1000000;
}
#endif

struct queue_head *queue_get_wtimeout(struct queue_root *root, size_t *len, useconds_t ut)
{
  if (m_bQuit) { return NULL; }

  pthread_mutex_lock(&root->lock);

  struct queue_head *head = &root->head;
  struct queue_head *result = NULL;

  // If the queue is empty and a non-zero timeout is requested...
  if (head->prev == head && ut > 0) {

    assert(*len == 0);

    // Compute target time first
    long ut_seconds = ut / 1000000;
    long ut_ns = (ut % 1000000) * 1000;

    struct timespec ts_orig,ts;
    clock_gettime(CLOCK_REALTIME, &ts_orig);
    ts = ts_orig;
    ts.tv_sec += ut_seconds;
    ts.tv_nsec += ut_ns;
    if (ts.tv_nsec >= 1000000000) {
      ts.tv_nsec -= 1000000000;
      ts.tv_sec += 1;
    }

    int tw;

  again:
    tw = pthread_cond_timedwait(&root->not_empty_notify, &root->lock, &ts);
    if (tw == 0) {
      if (head->prev == head)
        // Maybe another consumer grabbed it already...
        goto again;
    }
    else switch(errno) {
      case EAGAIN:
      case ETIMEDOUT:
        // The timeout happened - so continue on and most likely return NULL.
#if QUEUE_DEBUG_P
        clock_gettime(CLOCK_REALTIME, &ts);
        printf("pthread_cond_timedwait returned after timing out: elapsed time %d usecs\n",
               tselapsed(&ts_orig, &ts));
#endif
        break;
      default:
        printf("pthread_cond_timedwait returned with an error %s\n", strerror(errno));
        abort();
      }
  }

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
