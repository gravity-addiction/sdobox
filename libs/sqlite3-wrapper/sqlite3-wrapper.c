
#include <stdio.h>
#include <sqlite3.h> // SQLite3 Database
#include <wiringPi.h> // Gordons Wiring Pi

#include "shared.h"
#include "queue/queue.h"
#include "sqlite3-wrapper/sqlite3-wrapper.h"
#include "dbg/dbg.h"


// ------------------------
// SQLite Wrapper Thread
// ------------------------
PI_THREAD (sqlite3WrapperThread)
{
  if (sqlite3WrapperThreadRunning) {
    dbgprintf(DBG_ERROR, "Not Starting sqlite3 Thread, Already Started\n");
    return NULL;
  }
  sqlite3WrapperThreadRunning = 1;

  if (sqlite3WrapperThreadKill) {
    dbgprintf(DBG_ERROR, "Not Starting sqlite3 Thread, Stop Flag Set\n");
    sqlite3WrapperThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_INFO, "Starting SQLite Wrapper Thread\n");

  sqlite3* sqlDb = NULL;
  sqlite3_stmt* sqlQuery = NULL;
  char *zErrMsg = 0;

  int rc = sqlite3_open_v2(SQLITE3_PATH, &sqlDb, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (rc) {
    dbgprintf(DBG_ERROR, "Can't open database: %s\n", sqlite3_errmsg(sqlDb));
    return NULL;
  } else {
    dbgprintf(DBG_INFO, "Opened database successfully\n");
  }

  // debug_print("%s\n", "Starting FBCP Thread");
  while (!sqlite3WrapperThreadKill) {
    if (sqlite3WrapperQueueLen > 0) {
      // Fetch Items to deal with in queue
      struct queue_head *item = queue_get(sqlite3WrapperQueue, &sqlite3WrapperQueueLen);
      if (item) {
        sqlite3WrapperData *data = (sqlite3WrapperData *)item->data;
        switch (item->action) {
          case E_SQLITE3_EXECUTE:
            printf("EXCUTE SQL\n%s\n\n", data->sql);
            rc = sqlite3_exec(sqlDb, data->sql, data->callback, data, &zErrMsg);
            if(rc != SQLITE_OK) {
              fprintf(stderr, "SQL error: %s\n", zErrMsg);
              sqlite3_free(zErrMsg);
            } else {
              fprintf(stdout, "Operation done successfully\n");
            }

          break;
          case E_SQLITE3_SELECT:
            printf("SELECT SQL\n");
/*
if (SQLITE_OK != (ret = sqlite3_prepare_v2(pDb, "SELECT 2012", -1, &query, NULL)))
  {
      printf("Failed to prepare insert: %d, %s\n", ret, sqlite3_errmsg(pDb));
      break;
  }
  // step to 1st row of data
  if (SQLITE_ROW != (ret = sqlite3_step(query))) // see documentation, this can return more values as success
  {
      printf("Failed to step: %d, %s\n", ret, sqlite3_errmsg(pDb));
      break;
  }
  // ... and print the value of column 0 (expect 2012 here)
  printf("Value from sqlite: %s", sqlite3_column_text(query, 0));
*/
          break;
          case E_SQLITE3_INSERT:
            printf("INSERT SQL\n");

          break;
          case E_SQLITE3_UPDATE:
            printf("UPDATE SQL\n");

          break;
          case E_SQLITE3_DELETE:
            printf("DELETE SQL\n");

          break;

        default:
          printf("SQLite3 Undetected item action: %d\n", item->action);
          abort();
          break;
        }
        if (item->data != NULL) { free(item->data); }
        free(item);
      }
    }
    usleep(16666); // double desired framerate (1 / 60) * 1000000
  }

  dbgprintf(DBG_INFO, "Closing SQLite Wrapper Thread\n");
  // cleanup
  if (NULL != sqlQuery) sqlite3_finalize(sqlQuery);
  if (NULL != sqlDb) sqlite3_close(sqlDb);
  sqlite3_shutdown();

  sqlite3WrapperThreadRunning = 0;
  return NULL;
}


void sqlite3_wrapper_init() {
  sqlite3WrapperQueue = ALLOC_QUEUE_ROOT();
  sqlite3WrapperQueueLen = 0;
  int rc;
  if ((rc = sqlite3_initialize()) != SQLITE_OK)
  {
    dbgprintf(DBG_ERROR, "Failed to initialize library: %d\n", rc);
  }
}

int sqlite3_wrapper_start() {
  // printf("%s\n", "sqlite3WrapperThreadStart()");
  if (sqlite3WrapperThreadRunning) { return 0; }

  sqlite3WrapperThreadKill = 0;
  return piThreadCreate(sqlite3WrapperThread);
}

void sqlite3_wrapper_stop() {
  // printf("%s\n", "sqlite3WrapperThreadStop()");
  if (sqlite3WrapperThreadRunning && !sqlite3WrapperThreadKill) {
    sqlite3WrapperThreadKill = 1;
    int shutdown_cnt = 0;
    while (sqlite3WrapperThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    sqlite3WrapperThreadKill = 0;
  }
}
