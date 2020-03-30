// System Headers
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <wiringPi.h> // Gordons Wiring Pi
#include <pthread.h>

#include "libs/shared.h"

#include "mpv_events.h"
#include "mpv.h"


struct libMpvEventThreadCbData * LIBMPV_EVENTS_INIT_DATA()
{
  struct libMpvEventThreadCbData *threads = (struct libMpvEventThreadCbData*)malloc(sizeof(struct libMpvEventThreadCbData));
  threads->max = 16;
  threads->len = 0;
  threads->cbs = (struct libMpvEventThreadCb**)malloc(threads->max * sizeof(struct libMpvEventThreadCb));
  for (int c = 0; c < threads->max; ++c) {
    threads->cbs[c] = NULL;
  }
  return threads;
};

void LIBMPV_EVENTS_DESTROY_DATA(struct libMpvEventThreadCbData *threads) {// free cb
  libMpvCallbackClean(threads);
  free(threads->cbs);
  free(threads);
}

void libMpvCallbackClean(struct libMpvEventThreadCbData *threads) {
  for (size_t i = 0; i < threads->len; ++i) {
    free(threads->cbs[i]);
    threads->cbs[i] = NULL;
  }
  threads->len = 0;
}


void* libMpvCallbackFunc(void* targ) {
  // struct libMpvEventThreadData *data;
  // data = (struct libMpvEventThreadData *) targ;
  printf("Here! %s\n", (char*)targ);
  // data->cb(data->event);
  // free(data);
  pthread_exit(NULL);
}


int libMpvCallbackAppend(void (*function)(char*)) {
  if (libMpvEventThreads != NULL &&
      libMpvEventThreads->len < libMpvEventThreads->max
  ) {
    /*
    struct libMpvEventThreadCb cbs; // = (struct libMpvEventThreadCb*)malloc(sizeof(struct libMpvEventThreadCb));
    cbs.id = libMpvEventThreads->len;
    cbs.cb = function;
    libMpvEventThreads->cbs[libMpvEventThreads->len] = &cbs;
    */
    return libMpvEventThreads->len++;
  }
  return -1;
}

// ------------------------
// MPV Socket Thread
// ------------------------
PI_THREAD (libMpvSocketThread)
{
  if (libMpvSocketThreadRunning) {
    // debug_print("%s\n", "Not Starting MPV Event Thread, Already Started");
    return NULL;
  }
  libMpvSocketThreadRunning = 1;

  if (libMpvSocketThreadKill) {
    // debug_print("%s\n", "Not Starting MPV Event Thread, Stop Flag Set");
    libMpvSocketThreadRunning = 0;
    return NULL;
  }

  int socket_try = 0;
  // Wait for socket to arrive
  while (!libMpvSocketThreadKill && socket_try < 10 && access(mpv_socket_path, R_OK) == -1) {
    // debug_print("Waiting to See %s\n", mpv_socket_path);
    socket_try++;
    usleep(1000000);
  }

  int mpv_fd;
  if ((mpv_fd = mpv_socket_conn()) == -1) {
    // debug_print("%s\n", "MPV Socket Error");
    libMpvSocketThreadKill = 1;
  }

  // Grab MPV Events, sent in JSON format
  while(!libMpvSocketThreadKill) {
    if (!fd_is_valid(mpv_fd)) {
      // dbgprintf(DBG_MPV, "FD Re-Connect: %d, %d\n", mpv_fd_timer, millis());
      // try closing fd
      if (mpv_fd) { close(mpv_fd); }
      // reconnect fd
      if ((mpv_fd = mpv_socket_conn()) == -1) {
        // MPV Connect Error
        libMpvSocketThreadKill = 1;
      }
    }

// Grab Next Socket Line
    char* mpv_event_ret; // = malloc(128);
    // int rc = getline(&mpv_event_ret, 256, mpv_fd);
    int rc = sgetline(mpv_fd, &mpv_event_ret);
    if (rc > 0) {

      char* json_event;// = malloc(128);
      int rcE = ta_json_parse(mpv_event_ret, "event", &json_event);
      printf("MPV Event: -%s-, Parsed: %s Len: %d\n", mpv_event_ret, json_event, rcE);
      free(mpv_event_ret);
      if (rcE == 0) { continue; }

      // for (int tI = 0; tI < libMpvEventThreads->len; ++tI) {
        // pthread_t threads;
        // struct libMpvEventThreadData threadsData; // = (struct libMpvEventThreadData*)malloc(sizeof(struct libMpvEventThreadData));
        // threadsData.event = json_event;
        // threadsData.cb = libMpvEventThreads->cbs[tI]->cb;
        // pthread_create(&threads, NULL, libMpvCallbackFunc, (void *)&json_event);
      // }
      usleep(100);
    }

  // No Work needed done
    else {
      // Nothing to Do, Sleep for a moment
      usleep(200000);
    }
  }
  // close
  // debug_print("%s\n", "Closing MPV RPC");
  libMpvSocketThreadRunning = 0;
  return NULL;
}


int libMpvSocketThreadStart() {
  // debug_print("%s\n", "libMpvSocketThreadStart()");
  if (libMpvSocketThreadRunning) { return 0; }

  // Create callback struct
  libMpvEventThreads = LIBMPV_EVENTS_INIT_DATA();

  // debug_print("SkydiveOrBust MPV Socket Thread Spinup: %d\n", libMpvSocketThreadRunning);
  libMpvSocketThreadKill = 0;
  return piThreadCreate(libMpvSocketThread);
}

void libMpvSocketThreadStop() {
  // debug_print("%s\n", "libMpvSocketThreadStop()");
  // Shutdown MPV Socket Thread
  if (libMpvSocketThreadRunning) {
    libMpvSocketThreadKill = 1;
    int shutdown_cnt = 0;
    while (libMpvSocketThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }

    LIBMPV_EVENTS_DESTROY_DATA(libMpvEventThreads);
    // debug_print("SkydiveOrBust MPV Socket Thread Shutdown %d\n", shutdown_cnt);
  }
}


