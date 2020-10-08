#ifndef _LIBSDOBSOCKET_H_
#define _LIBSDOBSOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int libSdobSocketThreadKill;
int libSdobSocketThreadRunning;

int libSdobSocket_fd;
int libSdobSocket_buf_size;
char* libSdobSocket_socket_path;

struct queue_root *libSdobSocket_WriteQueue;
size_t libSdobSocket_WriteQueueLen;

int libSdobSocketThreadStart();
void libSdobSocketThreadStop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _LIBSDOBSOCKET_H_