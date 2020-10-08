#ifndef _SQLITE3_WRAPPER_H_
#define _SQLITE3_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum {
  E_SQLITE3_TYPE_NULL,
  E_SQLITE3_TYPE_INT,
  E_SQLITE3_TYPE_DOUBLE,
  E_SQLITE3_TYPE_TEXT,
  E_SQLITE3_TYPE_BLOB,
  E_SQLITE3_TYPE_POINTER,

  E_SQLITE3_MAX_SQL_TYPES
};

struct sqlite3WrapperBinding {
  int datatype;

  int integer;
  double floating;
  char *str;
};

struct sqlite3WrapperData {
  char* sql;
  int (*callback)(void*, int, char**, char**);
  int bindingLen;
  struct sqlite3WrapperBinding *binding;
};

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

char *SQLITE3_PATH;

void sqlite3Version();
void sqlite3_wrapper_init();
int sqlite3_wrapper_start();
void sqlite3_wrapper_stop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _SQLITE3_WRAPPER_H_