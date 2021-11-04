#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <jansson.h>

#include "libs/shared.h"
#include "libs/dbg/dbg.h"
#include "libs/zhelpers/zhelpers-tx.h"
#include "libs/zhelpers/zhelpers.h"

#include "sdob-zmq.h"

void *libsdob_zmq_scoring = NULL;
void *libsdob_zmq_events = NULL;

int libsdob_zmq_init() {
  dbgprintf(DBG_MPV_WRITE, "SDOB ZMQ Init\n");
  return 1;
}

void libsdob_zmq_destroy() {
  if (libsdob_zmq_scoring != NULL) {
    zmq_close(libsdob_zmq_scoring);
    libsdob_zmq_scoring = NULL;
  }
  if (libsdob_zmq_events != NULL) {
    zmq_close(libsdob_zmq_events);
    libsdob_zmq_events = NULL;
  }
}

// Scoring Push
int libsdob_zmq_scoring_init() {
  printf("Attempting Scoring 4010\n");
  libsdob_zmq_scoring = zmq_socket (zerocontext, ZMQ_REQ);
  int rc = zmq_connect (libsdob_zmq_scoring, "tcp://flittermouse.local:4010");
  printf("Scoring 4010: %d\n", rc);
  return rc;
}

// Incoming thread for sdob events
void * libsdob_zmq_events_init() {
  printf("Attempting Events 4011\n");
  void *sdobEvents = zmq_socket (zerocontext, ZMQ_SUB);
  int rc = zmq_connect (sdobEvents, "tcp://flittermouse.local:4011");
  if (rc == 0) {
    printf("Events 4011: %d\n", rc);
    const char *filterevents = "";
    rc = zmq_setsockopt (sdobEvents, ZMQ_SUBSCRIBE, filterevents, strlen (filterevents));
  }
  return sdobEvents;
}


int libsdob_zmq_scoring_send(char* question, char** response) {
  int rc;
  if (libsdob_zmq_scoring == NULL) {
    dbgprintf(DBG_MPV_WRITE, "%s\n", "Connecting For Scoring");
    rc = libsdob_zmq_scoring_init();
  }

  rc = s_send (libsdob_zmq_scoring, question);
  free(question);
  if (rc < 0) {
    return -1;
  }

  char *string = s_recv (libsdob_zmq_scoring);
  if (response != NULL) {
    *response = string;
  } else {
    free (string);
    return -1;
  }
  return rc;
}

