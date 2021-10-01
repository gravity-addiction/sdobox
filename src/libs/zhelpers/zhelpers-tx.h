/*  =====================================================================
    zhelpers.h

    Helper header file for example applications.
    =====================================================================
*/

#ifndef __ZHELPERS_TX_H_INCLUDED__
#define __ZHELPERS_TX_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <zmq.h>

#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if (!defined (WIN32))
#   include <sys/time.h>
#endif

#if (defined (WIN32))
#   include <windows.h>
#endif


//  On some version of Windows, POSIX subsystem is not installed by default.
//  So define srandom and random ourself.
#if (defined (WIN32))
#   define srandom srand
#   define random rand
#endif

//  Provide random number from 0..(num-1)
#define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))

void *zerocontext;

//  Receive 0MQ string from socket and convert into C string
//  Caller must free returned string. Returns NULL if the context
//  is being terminated.
static char *
s_recv (void *socket) {
    enum { cap = 256 };
    char buffer [cap];
    int size = zmq_recv (socket, buffer, cap - 1, 0);
    if (size == -1)
        return NULL;
    buffer[size < cap ? size : cap - 1] = '\0';

#if (defined (WIN32))
    return strdup (buffer);
#else
    return strndup (buffer, sizeof(buffer) - 1);
#endif

    // remember that the strdup family of functions use malloc/alloc for space for the new string.  It must be manually
    // freed when you are done with it.  Failure to do so will allow a heap attack.
}

//  Convert C string to 0MQ string and send to socket
static int
s_send (void *socket, char *string) {
    int size = zmq_send (socket, string, strlen (string), 0);
    return size;
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif  //  __ZHELPERS_TX_H_INCLUDED__
