/*  =====================================================================
    zhelpers.h

    Helper header file for example applications.
    =====================================================================
*/

#ifndef __ZHELPERS_CONN_H__
#define __ZHELPERS_CONN_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void libzhelpers_conn_init(void);
void libzhelpers_conn_destroy(void);
void* libzhelpers_context(void);

// Incoming thread for mpv video time-pos
int zmq_connect_socket(void **sock, char* url, int sockType);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif  //  __ZHELPERS_CONN_H__
