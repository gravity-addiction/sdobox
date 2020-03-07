#include <stdlib.h>
#include <sqlite3.h> // SQLite3 Database

#include "shared.h"
#include "skydiveorbust.h"
#include "sqlite3-wrapper/sqlite3-wrapper.h"
#include "queue/queue.h"
#include "dbg/dbg.h"

static int pg_sdobSqlCb_checkVersion(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i < argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void pg_sdobSqlSetup() {
  sqlite3WrapperData *data = (sqlite3WrapperData*)malloc(sizeof(sqlite3WrapperData));
  data->sql = "select sqlite_version();";
  data->callback = pg_sdobSqlCb_checkVersion;

  struct queue_head *item = new_qhead();
  item->action = E_SQLITE3_EXECUTE;
  item->data = data;
  queue_put(item, sqlite3WrapperQueue, &sqlite3WrapperQueueLen);
}

