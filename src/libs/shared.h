#ifndef _SHARED_H_
#define _SHARED_H_


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define	COUNT_KEY	0

#define DEBUG 0
#define CLEAR(x, y) memset(x,'\0',y)

// Videos Folder, config videos_path
char* VIDEOS_BASEPATH;

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

struct fileStruct {
  char *name;
  char *path;
  int mode;
  size_t size;
  size_t atime;
  size_t mtime;
  size_t ctime;
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
int fd_is_connected(int fd);
int setnonblock(int sock);
//? void *malloc(unsigned int size);
void clearsocket(int fd);
int sgetline(int fd, char ** out);
void sgetlines_withcb(char *buf, size_t len, void (*function)(char *, size_t sz, size_t cnt));

int time_to_secs(char* timestamp);
void secs_to_time(int millisecs, char *retFormat, int retLen);
int parseTabbedData(const char *s, char *data[], size_t n);

int cstring_cmp(const void *a, const void *b);
int cint_cmp(const void *a, const void *b);
char *file_ext(char *filename);
int file_exists(char *filename);
int fileStruct_cmpName(const void *a, const void *b);
size_t file_list(const char *path, struct fileStruct ***ls, int type);
size_t file_list_w_up(const char *path, struct fileStruct ***ls, int type);

char * calculateSize(uint64_t size);
// void run_system_cmd(char *fullpath);
// int run_system_cmd_with_return(char *fullpath, char *ret, int retsize);
//-/ void fbcp_start();
//-/ void fbcp_stop();

#define MIN(a,b) \
 ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#define MAX(a,b) \
 ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SHARED_H
