#ifndef _SHARED_H_
#define _SHARED_H_


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define	COUNT_KEY	0

#define JSMN_HEADER
#include "jsmn.h" // JSON Parsing

#define DEBUG 0
#define CLEAR(x, y) memset(x,'\0',y)


int m_bQuit;
int fbcp_running;
// Debugging Printout helper
// #ifdef DEBUG
// #define debug_print printf
//#else
// #define // debug_print(fmt, args...)
//#endif

// String helper mainly for CURL
struct string {
  char *ptr;
  size_t len;
};


#ifndef HAVE_STRLCAT
size_t strlcat(char *dst, const char *src, size_t size);
#endif /* !HAVE_STRLCAT */

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dst, const char *src, size_t size);
#endif /* !HAVE_STRLCPY */


void init_string(struct string *s);
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);
int fd_is_valid(int fd);
int setnonblock(int sock);
void *malloc(unsigned int size);
int sgetline(int fd, char ** out);
void sgetlines_withcb(char *buf, size_t len, void (*function)(char *, size_t sz, size_t cnt));

int cmp_strcmp(const void *lhs, const void *rhs);
int cmp_atoi(const void *a, const void *b);
int time_to_secs(char* timestamp);
void secs_to_time(int millisecs, char *retFormat, int retLen);
int jsoneq(const char *json, jsmntok_t *tok, const char *s);
void ta_json_parse(char *json, char* prop, char ** ret_var);
int parseTabbedData(const char *s, char *data[], size_t n);

size_t file_list(const char *path, char ***ls);
size_t folder_list(const char *path, char ***ls);

char * calculateSize(uint64_t size);
// void run_system_cmd(char *fullpath);
// int run_system_cmd_with_return(char *fullpath, char *ret, int retsize);
// void fbcp_start();
// void fbcp_stop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SHARED_H
