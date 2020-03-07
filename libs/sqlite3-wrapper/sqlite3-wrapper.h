#ifndef _SQLITE3_WRAPPER_H_
#define _SQLITE3_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
  char* sql;
  void* callback;
} sqlite3WrapperData;

// Thread
int sqlite3WrapperThreadKill;
int sqlite3WrapperThreadRunning;

/////////////////////
// Queue
enum {
  E_SQLITE3_EXECUTE,
  E_SQLITE3_SELECT,
  E_SQLITE3_INSERT,
  E_SQLITE3_UPDATE,
  E_SQLITE3_DELETE,

  E_SQLITE3_MAX_QUEUE_TYPES
};

struct queue_root *sqlite3WrapperQueue;
size_t sqlite3WrapperQueueLen;

char* SQLITE3_PATH;


void sqlite3_wrapper_init();
int sqlite3_wrapper_start();
void sqlite3_wrapper_stop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _SQLITE3_WRAPPER_H_