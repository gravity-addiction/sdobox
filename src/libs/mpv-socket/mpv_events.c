// System Headers
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <wiringPi.h> // Gordons Wiring Pi
#include <pthread.h>
#include <errno.h>

#include "libs/shared.h"
#include "libs/dbg/dbg.h"

#include "libs/queue/queue.h"
#include "libs/json/json.h"
#include "libs/mpv-wrapper/mpv-cache.h"

#include "mpv_events.h"
#include "mpv-socket.h"




struct libMpvEventsCbData * LIBMPV_EVENTS_INIT_DATA()
{
  struct libMpvEventsCbData *cbList = (struct libMpvEventsCbData*)malloc(sizeof(struct libMpvEventsCbData));
  cbList->max = 16;
  cbList->len = 0;
  cbList->idC = 0;
  cbList->cbs = (struct libMpvEventsCb**)malloc(cbList->max * sizeof(struct libMpvEventsCb));
  for (int c = 0; c < cbList->max; ++c) {
    cbList->cbs[c] = NULL;
  }
  return cbList;
}

void LIBMPV_EVENTS_DESTROY_DATA(struct libMpvEventsCbData *cbList) {// free cb
  libMpvCallbackClean(cbList);
  free(cbList->cbs);
}


void libMpvCallbackClean(struct libMpvEventsCbData *cbList) {
  for (size_t i = 0; i < cbList->len; ++i) {
    if (cbList->cbs[i] != NULL) {
      free(cbList->cbs[i]);
    }
  }
  cbList->len = 0;
}

unsigned long int libMpvCallbackAppend(void (*function)(char*)) {
  if (libMpvEventsCbList != NULL &&
      libMpvEventsCbList->len < libMpvEventsCbList->max
  ) {
    
    struct libMpvEventsCb *eventCb = (struct libMpvEventsCb*)malloc(sizeof(struct libMpvEventsCb));
    eventCb->id = libMpvEventsCbList->idC;
    eventCb->cb = function;
    libMpvEventsCbList->cbs[libMpvEventsCbList->len] = eventCb;

    libMpvEventsCbList->len++;
    return libMpvEventsCbList->idC++;
  }
  return -1;
}

void libMpvCallbackRemove(unsigned long int cbListId) {
  for (size_t i = 0; i < libMpvEventsCbList->len; ++i) {
    if (libMpvEventsCbList->cbs[i] != NULL && libMpvEventsCbList->cbs[i]->id == cbListId) {
      for (size_t ix = i + 1; i < (libMpvEventsCbList->len - 1); ++i) {
        *libMpvEventsCbList->cbs[ix - 1] = *libMpvEventsCbList->cbs[ix];
      }

      free(libMpvEventsCbList->cbs[libMpvEventsCbList->len - 1]);
      libMpvEventsCbList->cbs[libMpvEventsCbList->len - 1] = NULL;

      libMpvEventsCbList->len--;
      return;
    }
  }
}

/*
void* libMpvCallbackFunc(void* targ) {
  // struct libMpvEventThreadData *data;
  // data = (struct libMpvEventThreadData *) targ;
  printf("Here! %s\n", (char*)targ);
  // data->cb(data->event);
  // free(data);
  // pthread_exit(NULL);
}
*/




void libMpvProcessEvent(char *event) {

  // Updates an allocated space in memory with a counter
  // off-threads can rely on the libmpvCache->player->cnt as a counter
  // would like to figure out a way to deal with going over 4,294,967,265 and crashing.

  // Is Playing
  if (strcmp(event, "start-file") == 0 ||
      strcmp(event, "play") == 0
  ) {
    libmpvCache->player->is_playing = 1;

  // No Longer Playing
  } else if (
    strcmp(event, "end-file") == 0 || 
    strcmp(event, "pause") == 0
  ) {
    libmpvCache->player->is_playing = 0;

  // Is Loaded
  } else if (strcmp(event, "file-loaded") == 0) {
    libmpvCache->player->is_loaded = 1;
    libmpvCache->player->has_seeked = 1;

  // Nothing Loaded
  } else if (strcmp(event, "idle") == 0) {
    libmpvCache->player->is_loaded = 0;
    libmpvCache->player->has_seeked = 1;
    
  // Seeking Thru File, Not Playing, and not Paused
  } else if (strcmp(event, "seek") == 0) {
    libmpvCache->player->is_seeking = 1;
    libmpvCache->player->has_seeked = 1;

  // No longer Seeking, Playable
  } else if (strcmp(event, "playback-restart") == 0) {
    libmpvCache->player->is_seeking = 0;

  } else if (strcmp(event, "metadata-update") == 0) {
    libmpv_setduration();
    libmpvCache->player->has_seeked = 1;
  } else {
    printf("Event: %s\n", event);
    return;
  }
  libmpvCache->player->cnt++;



  /*
    // Apply for callback of event, sends event as char* argument
    // Setup callback function
    void mpvEventCb(char* event) {
      printf("Yay Event: %s\n", event);
    }
    // Append callback to mpv_events
    int mpvEventCbId = libMpvCallbackAppend(&mpvEventCb);
    // Stop event cbs and clean mpv_events of your cb
    if (mpvEventCbId > -1) {
      libMpvCallbackRemove(mpvEventCbId);
    }
  */
  // Process Subscribed Callbacks
  for (size_t i = 0; i < libMpvEventsCbList->len; ++i) {
    if (libMpvEventsCbList->cbs[i] != NULL && libMpvEventsCbList->cbs[i]->cb != NULL) {
      libMpvEventsCbList->cbs[i]->cb(event);
    }
  }
}



// Set sdob_player->duration
void libmpv_setduration() {
  // Update Duration
  mpv_any_u * retDur;
  if ((mpvSocketSinglet("duration", &retDur)) != -1) {
    if (retDur->hasPtr == 1) { libmpvCache->player->duration = atof(retDur->ptr);
    } else { libmpvCache->player->duration = retDur->floating;
    }
    MPV_ANY_U_FREE(retDur);
    // printf("Video Duration Dbl: %f\n", libmpvCache->player->duration);
  } else {
    // printf("%s\n", "No Video Duration!!");
    libmpvCache->player->duration = 0;
  }
}

// ------------------------
// MPV Event Queue Thread
// ------------------------
PI_THREAD (libMpvQueueThread)
{
  if (libMpvQueueThreadRunning) {
    // debug_print("%s\n", "Not Starting MPV Event Thread, Already Started");
    return NULL;
  }
  libMpvQueueThreadRunning = 1;

  if (libMpvQueueThreadKill) {
    // debug_print("%s\n", "Not Starting MPV Event Thread, Stop Flag Set");
    libMpvQueueThreadRunning = 0;
    return NULL;
  }

  char* json_event_raw;
  int rcE;
  // Grab MPV Events, sent in JSON format
  while(!libMpvQueueThreadKill) {

    if (libMpvEvents_QueueLen > 0) {
      // Fetch Items to deal with in queue
      struct queue_head *item = queue_get(libMpvEvents_Queue, &libMpvEvents_QueueLen);
      if (item) {
        switch (item->action) {
          case 0: // RAW
            rcE = ta_json_parse((char *)item->data, "event", &json_event_raw);
            if (rcE > 0) {
               dbgprintf(DBG_MPV_EVENT, "Got Raw %s\n", json_event_raw);
               libMpvProcessEvent(json_event_raw);
            }
            free(json_event_raw);
          break;
          case 1: // Event Parsed
            dbgprintf(DBG_MPV_EVENT, "Event %s\n", (char *)item->data);
            libMpvProcessEvent(item->data);
          break;
        }
      }
      if (item->data != NULL) { free(item->data); }
      free(item);

      usleep(100);
    } else {
      usleep(200000);
    }
    
  }
  // close
  // debug_print("%s\n", "Closing MPV RPC");
  libMpvQueueThreadRunning = 0;
  return NULL;
}


int libMpvQueueThreadStart() {
  // debug_print("%s\n", "libMpvQueueThreadStart()");
  if (libMpvQueueThreadRunning) { return 0; }

  //////////////////////////////
  // Queue Initializer
  libMpvEvents_Queue = ALLOC_QUEUE_ROOT();
  libMpvEvents_QueueLen = 0;

  // debug_print("SkydiveOrBust MPV Socket Thread Spinup: %d\n", libMpvQueueThreadRunning);
  libMpvQueueThreadKill = 0;
  return piThreadCreate(libMpvQueueThread);
}

void libMpvQueueThreadStop() {
  // debug_print("%s\n", "libMpvQueueThreadStop()");
  // Shutdown MPV Socket Thread
  if (libMpvQueueThreadRunning) {
    libMpvQueueThreadKill = 1;
    int shutdown_cnt = 0;
    while (libMpvQueueThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    
    free(libMpvEvents_Queue);
    libMpvEvents_QueueLen = 0;
  }
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
      dbgprintf(DBG_ERROR, "Mpv Events Sockets Re-Connect\n");
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

      struct queue_head *item = new_qhead();
      item->data = strdup(json_event);
      item->action = 1;
      queue_put(item, libMpvEvents_Queue, &libMpvEvents_QueueLen);

      dbgprintf(DBG_MPV_EVENT, "MPV Event: -%s-, Parsed: %s Len: %d\n", mpv_event_ret, json_event, rcE);
      free(mpv_event_ret);
      if (rcE < 0) { continue; }
      // for (int tI = 0; tI < libMpvEventThreads->len; ++tI) {
        // pthread_t threads;
        // struct libMpvEventThreadData threadsData; // = (struct libMpvEventThreadData*)malloc(sizeof(struct libMpvEventThreadData));
        // threadsData.event = json_event;
        // threadsData.cb = libMpvEventThreads->cbs[tI]->cb;
        // pthread_create(&threads, NULL, libMpvCallbackFunc, (void *)&json_event);
      // }
      free(json_event);
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
  libMpvEventsCbList = LIBMPV_EVENTS_INIT_DATA();

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

    LIBMPV_EVENTS_DESTROY_DATA(libMpvEventsCbList);
    free(libMpvEventsCbList);
    // debug_print("SkydiveOrBust MPV Socket Thread Shutdown %d\n", shutdown_cnt);
  }
}


