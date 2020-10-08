
#include <stdio.h>
#include <sqlite3.h> // SQLite3 Database
#include <wiringPi.h> // Gordons Wiring Pi

#include "libs/shared.h"
#include "libs/queue/queue.h"
#include "sqlite3-wrapper.h"
#include "libs/dbg/dbg.h"


void sqlite3Version() {
  sqlite3 *memdb;
  sqlite3_stmt *res;

  int rc = sqlite3_open(":memory:", &memdb);
  if (rc != SQLITE_OK) {
    dbgprintf(DBG_ERROR, "Cannot open database: %s\n", sqlite3_errmsg(memdb));
    sqlite3_close(memdb);
    return;
  }

  rc = sqlite3_prepare_v2(memdb, "SELECT SQLITE_VERSION()", -1, &res, 0);
  if (rc != SQLITE_OK) {
    dbgprintf(DBG_ERROR, "Failed to fetch data: %s\n", sqlite3_errmsg(memdb));
    sqlite3_close(memdb);
    return;
  }
  rc = sqlite3_step(res);
  if (rc == SQLITE_ROW) {
    dbgprintf(DBG_DEBUG, "SQLite Version: %s\n", sqlite3_column_text(res, 0));
  }

  sqlite3_finalize(res);
  sqlite3_close(memdb);
}

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
        size_t ptrLen;
        struct sqlite3WrapperData *data = item->data;
        switch (item->action) {
          case E_SQLITE3_EXECUTE:

            dbgprintf(DBG_INFO, "EXCUTE SQL\n%s\n", data->sql);

            if (data->bindingLen > 0) {
              rc = sqlite3_prepare_v2(sqlDb, data->sql, -1, &sqlQuery, 0);
              int rcb;
//              char **datachar = (char**)malloc(data->bindingLen * sizeof(char*));
              for (int b = 0; b < data->bindingLen; ++b) {
                int sqlI = b + 1;
                switch (data->binding[b].datatype) {
                  case E_SQLITE3_TYPE_NULL:
                    rcb = sqlite3_bind_null(sqlQuery, sqlI);
//                    datachar[sqlI] = NULL;
                  break;
                  case E_SQLITE3_TYPE_INT:
                    rcb = sqlite3_bind_int(sqlQuery, sqlI, data->binding[b].integer);
//                    ptrLen = snprintf(NULL, 0, "%d", data->binding[b].integer) + 1;
//                    datachar[sqlI] = (char*)malloc(ptrLen * sizeof(char));
//                    snprintf(datachar[sqlI], ptrLen, "%d", data->binding[b].integer);
                  break;
                  case E_SQLITE3_TYPE_DOUBLE:
                    rcb = sqlite3_bind_double(sqlQuery, sqlI, data->binding[b].floating);
//                    ptrLen = snprintf(NULL, 0, "%f", data->binding[b].floating) + 1;
//                    datachar[sqlI] = (char*)malloc(ptrLen * sizeof(char));
//                    snprintf(datachar[sqlI], ptrLen, "%f", data->binding[b].floating);
                  break;
                  case E_SQLITE3_TYPE_TEXT:
                    rcb = sqlite3_bind_text(sqlQuery, sqlI, data->binding[b].str, -1, NULL);
//                    ptrLen = snprintf(NULL, 0, "%s", data->binding[b].str) + 1;
//                    datachar[sqlI] = (char*)malloc(ptrLen * sizeof(char));
//                    snprintf(datachar[sqlI], ptrLen, "%s", data->binding[b].str);
                  break;
                  default:
                    rcb = SQLITE_OK;
                }
                if(rcb != SQLITE_OK) {
                  dbgprintf(DBG_ERROR, "Sqlite3 Binding: (%d) %s\n", sqlI, sqlite3_errmsg(sqlDb));
                }
              }

              int sStep = sqlite3_step(sqlQuery);
              if (sStep != SQLITE_DONE && sStep != SQLITE_OK && sStep != SQLITE_ROW) {
                dbgprintf(DBG_ERROR, "Sqlite3 Error: %s\n", sqlite3_errmsg(sqlDb));

              } else {
                while (sqlite3_step(sqlQuery) != SQLITE_DONE) {
                  dbgprintf(DBG_DEBUG, "Stepping\n");
                  int sqlI;
                  int num_cols = sqlite3_column_count(sqlQuery);
                  char **dataset = (char**)malloc(num_cols * sizeof(char*));
                  for (sqlI = 0; sqlI < num_cols; ++sqlI) {
                    switch (sqlite3_column_type(sqlQuery, sqlI)) {
                    case (SQLITE3_TEXT):
                      ptrLen = snprintf(NULL, 0, "%s", sqlite3_column_text(sqlQuery, sqlI)) + 1;
                      dataset[sqlI] = (char*)malloc(ptrLen * sizeof(char));
                      snprintf(dataset[sqlI], ptrLen, "%s", sqlite3_column_text(sqlQuery, sqlI));
                      break;
                    case (SQLITE_INTEGER):
                      ptrLen = snprintf(NULL, 0, "%d", sqlite3_column_int(sqlQuery, sqlI)) + 1;
                      dataset[sqlI] = (char*)malloc(ptrLen * sizeof(char));
                      snprintf(dataset[sqlI], ptrLen, "%d", sqlite3_column_int(sqlQuery, sqlI));
                      break;
                    case (SQLITE_FLOAT):
                      ptrLen = snprintf(NULL, 0, "%f", sqlite3_column_double(sqlQuery, sqlI)) + 1;
                      dataset[sqlI] = (char*)malloc(ptrLen * sizeof(char));
                      snprintf(dataset[sqlI], ptrLen, "%f", sqlite3_column_double(sqlQuery, sqlI));
                      break;
                    default:
                      dataset[sqlI] = (char*)malloc(1 * sizeof(char));
                      break;
                    }
                  }
                  if (data->callback != NULL) {
                    data->callback(NULL, num_cols, NULL, dataset);
                  }
                  for (sqlI = 0; sqlI < num_cols; ++sqlI) {
                    free(dataset[sqlI]);
                  }
                  free(dataset);
                }
              }

              sqlite3_finalize(sqlQuery);

            // No bindings just execute string
            } else {
              rc = sqlite3_exec(sqlDb, data->sql, data->callback, data, &zErrMsg);
            }

            if(rc != SQLITE_OK) {
              dbgprintf(DBG_ERROR, "SQL error: %s\n", zErrMsg);
              sqlite3_free(zErrMsg);
            } else {
              dbgprintf(DBG_INFO, "SQLite3 Operation done successfully\n");
            }

          break;

        default:
          dbgprintf(DBG_INFO, "SQLite3 Undetected item action: %d\n", item->action);
          abort();
          break;
        }


        if (item->data != NULL) { free(item->data); }
        free(item);
      }
    } else {
      usleep(50000); // double desired framerate (1 / 60) * 1000000
    }
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
  dbgprintf(DBG_INFO, "%s\n", "sqlite3WrapperThreadStart()");
  if (sqlite3WrapperThreadRunning) { return 0; }

  sqlite3WrapperThreadKill = 0;
  return piThreadCreate(sqlite3WrapperThread);
}

void sqlite3_wrapper_stop() {
  dbgprintf(DBG_INFO, "%s\n", "sqlite3WrapperThreadStop()");
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
