#include <stdlib.h>
#include <sqlite3.h> // SQLite3 Database

#include "shared.h"
#include "skydiveorbust_sql.h"
#include "sqlite3-wrapper/sqlite3-wrapper.h"
#include "queue/queue.h"
#include "dbg/dbg.h"

int pg_sdobSqlCb_debug(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i < argc; i++) {
     dbgprintf(DBG_DEBUG, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   dbgprintf(DBG_DEBUG, "\n");
   return 0;
}


void pg_sdobSqlSetup() {
  pg_sdobSql_createTables();
}


void pg_sdobSql_createTables() {
  struct sqlite3WrapperData *data = (struct sqlite3WrapperData *)malloc(sizeof(struct sqlite3WrapperData));
  // data->sql = "CREATE TABLE IF NOT EXISTS sqlite_sequence(name,seq); "
  data->sql = "CREATE TABLE IF NOT EXISTS video_info (id INTEGER PRIMARY KEY AUTOINCREMENT, id_rules INTEGER, video_hash TEXT NOT NULL UNIQUE, video_tags TEXT NOT NULL DEFAULT '', video_fps REAL NOT NULL DEFAULT 0, created_at REAL);" \
    "CREATE TABLE IF NOT EXISTS video_marks (id INTEGER PRIMARY KEY AUTOINCREMENT, id_video INTEGER NOT NULL, mark_type INTEGER NOT NULL DEFAULT '', video_time REAL NOT NULL DEFAULT 0, working_time REAL NOT NULL DEFAULT 0);" \
    "CREATE TABLE IF NOT EXISTS rule_info (id INTEGER PRIMARY KEY AUTOINCREMENT, rule_tags TEXT NOT NULL DEFAULT '', rule_json TEXT, rule_version INTEGER);" \
    "CREATE TABLE IF NOT EXISTS scorecards (id INTEGER PRIMARY KEY AUTOINCREMENT, id_video INTEGER, id_rule INTEGER, id_judge INTEGER, notes TEXT, created_at TEXT)";

  struct queue_head *item = new_qhead();
  item->action = E_SQLITE3_EXECUTE;
  item->data = data;
  queue_put(item, sqlite3WrapperQueue, &sqlite3WrapperQueueLen);
}

void pg_sdobSql_deleteTables() {
  struct sqlite3WrapperData *data = (struct sqlite3WrapperData *)malloc(sizeof(struct sqlite3WrapperData));
  data->sql = "DELETE FROM video_info;" \
    "DELETE FROM video_marks;" \
    "DELETE FROM rule_info;" \
    "DELETE FROM scorecards;";

  struct queue_head *item = new_qhead();
  item->action = E_SQLITE3_EXECUTE;
  item->data = data;
  queue_put(item, sqlite3WrapperQueue, &sqlite3WrapperQueueLen);
}

void pg_sdobSql_dropTables() {
  struct sqlite3WrapperData *data = (struct sqlite3WrapperData *)malloc(sizeof(struct sqlite3WrapperData));
  data->sql = "DROP TABLE video_info;" \
    "DROP TABLE video_marks;" \
    "DROP TABLE rule_info;" \
    "DROP TABLE scorecards;";

  struct queue_head *item = new_qhead();
  item->action = E_SQLITE3_EXECUTE;
  item->data = data;
  queue_put(item, sqlite3WrapperQueue, &sqlite3WrapperQueueLen);
}





int pg_sdobSql_markInsertCb(void *NotUsed, int argc, char **argv, char **azColName) {
  dbgprintf(DBG_DEBUG, "Mark CB\n");

  return 1;
}

void pg_sdobSql_markInsert(int id_video, int mark_type, char* video_time, char* working_time) {
  struct sqlite3WrapperData *data = (struct sqlite3WrapperData*)malloc(sizeof(struct sqlite3WrapperData));
  data->sql = "INSERT INTO video_marks (id, id_video, mark_type, video_time, working_time) VALUES (?, ?, ?, ?, ?);";
  data->callback = pg_sdobSql_markInsertCb;
  data->bindingLen = 5;
  data->binding = (struct sqlite3WrapperBinding *)malloc(data->bindingLen * sizeof(struct sqlite3WrapperBinding));
  // Autonumbering ID
  data->binding[0].datatype = E_SQLITE3_TYPE_NULL;

  // Video ID from video_info.id
  data->binding[1].datatype = E_SQLITE3_TYPE_INT;
  data->binding[1].integer = id_video;

  // Video Mark enum
  data->binding[2].datatype = E_SQLITE3_TYPE_INT;
  data->binding[2].integer = mark_type;

  // Video Mark Time for video timeline
  data->binding[3].datatype = E_SQLITE3_TYPE_TEXT;
  data->binding[3].str = video_time;

  // Video Mark Time from start of working time
  data->binding[4].datatype = E_SQLITE3_TYPE_TEXT;
  data->binding[4].str = working_time;

  struct queue_head *item = new_qhead();
  item->action = E_SQLITE3_EXECUTE;
  item->data = data;
  queue_put(item, sqlite3WrapperQueue, &sqlite3WrapperQueueLen);
}

/*
int db_video_score_count(int id_video) {
  sqlite3_stmt* stmt = 0;
  sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM video_marks where id_video = ?", -1, &stmt, 0);
  sqlite3_bind_int(stmt, 1, id_video);

  int iMarks = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    iMarks = sqlite3_column_int(stmt, 0);
  }

  //  Step, Clear and Reset the statement for the next loop
  sqlite3_step(stmt);
  sqlite3_clear_bindings(stmt);
  sqlite3_reset(stmt);
  sqlite3_finalize(stmt);

  return iMarks;
}

int db_video_scores(int id_video) {

  sqlite3_stmt* stmt = 0;
  int rc = sqlite3_prepare_v2(db, "SELECT \"_rowid_\", * FROM video_marks where id_video = ? ORDER BY video_time", -1, &stmt, 0);
  rc = sqlite3_bind_int(stmt, 1, id_video);

  int iMark = 0;

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    scorecard->arrScorecardId[iMark] = sqlite3_column_int(stmt, 1);
    scorecard->arrScorecardPoints[iMark] = sqlite3_column_int(stmt, 3);
    scorecard->arrScorecardTimes[iMark] = sqlite3_column_double(stmt, 4);
    scorecard->arrScorecardMilli[iMark] = -1;
    iMark++;
  }
  scorecard->size = iMark;
  if (scorecard->selected > iMark) {
    scorecard->selected = iMark - 1;
  }

  //  Step, Clear and Reset the statement for the next loop
  rc = sqlite3_step(stmt);
  rc = sqlite3_clear_bindings(stmt);
  rc = sqlite3_reset(stmt);
  rc = sqlite3_finalize(stmt);
  return rc;

  return 0;
}

*/