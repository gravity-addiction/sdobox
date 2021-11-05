/*  =====================================================================
    zhelpers.h

    Helper header file for example applications.
    =====================================================================
*/

#include <zmq.h>

#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void *zerocontext = NULL;

void libzhelpers_conn_init(void) {
  zerocontext = zmq_ctx_new();
  printf("Set Context\n");
}

void libzhelpers_conn_destroy(void) {
  if (zerocontext != NULL) {
    zmq_ctx_destroy(zerocontext);
  }
}

void* libzhelpers_context(void) {
  if (zerocontext == NULL) {
    libzhelpers_conn_init();
  }
  return zerocontext;
}

// Incoming thread for mpv video time-pos
int zmq_connect_socket(void **sock, char* url, int sockType) {
  if (!url) {
    return -1;
  }
  int rc = 0;
  *sock = zmq_socket(libzhelpers_context(), sockType);
  int timeoutsock = 2500;
  if (rc > -1) {
    rc = zmq_setsockopt(*sock, ZMQ_CONNECT_TIMEOUT, &timeoutsock, sizeof(timeoutsock));
  }
  if (rc > -1) {
    rc = zmq_setsockopt(*sock, ZMQ_LINGER, &timeoutsock, sizeof(timeoutsock));
  }
  if (rc > -1) {
    rc = zmq_connect (*sock, url); // "tcp://flittermouse.local:5555");
  }
  printf("Connected: %d: %s\n", rc, url);
  return rc;
}
