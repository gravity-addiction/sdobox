#ifndef _SDOBOX_SDOB_SQL_H_
#define _SDOBOX_SDOB_SQL_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int pg_sdobSqlCb_debug(void *NotUsed, int argc, char **argv, char **azColName);

void pg_sdobSqlSetup();
void pg_sdobSqlVersion();
void pg_sdobSql_createTables();
void pg_sdobSql_deleteTables();
void pg_sdobSql_dropTables();

int pg_sdobSql_markInsertCb(void *NotUsed, int argc, char **argv, char **azColName);
void pg_sdobSql_markInsert(int id_video, int mark_type, char* video_time, char* working_time);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _SDOBOX_SDOB_SQL_H_