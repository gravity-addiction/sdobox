// System Headers
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "libs/shared.h"
#include "libs/dbg/dbg.h"
#include "libs/queue/queue.h"
#include "libs/json/json.h"

#include "sdob-socket.h"

// ------------------------
// SDOBOX Socket Thread
// ------------------------
static pthread_mutex_t libSdobSocketWriteLock = PTHREAD_MUTEX_INITIALIZER;

PI_THREAD (libSdobSocketThread)
{
  if (libSdobSocketThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting SkydiveOrBust Socket Thread, Already Started");
    return NULL;
  }
  libSdobSocketThreadRunning = 1;

  if (libSdobSocketThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting SkydiveOrBust Socket Thread, Stop Flag Set");
    libSdobSocketThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_DEBUG, "%s\n", "Starting SkydiveOrBust Socket Thread");

  struct sockaddr_un svaddr, claddr;
  ssize_t numBytes;
  socklen_t len;
  char libSdobSocket_buf[libSdobSocket_buf_size];

  libSdobSocket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (libSdobSocket_fd == -1) {
    dbgprintf(DBG_DEBUG, "%s\n", "SkydiveOrBust Socket Error");
    libSdobSocketThreadKill = 1;
  }

  if (strlen(libSdobSocket_socket_path) > sizeof(svaddr.sun_path)-1) {
    dbgprintf(DBG_DEBUG, "SkydiveOrBust Socket path to long must be %d chars\n", sizeof(svaddr.sun_path)-1);
    libSdobSocketThreadKill = 1;
  }

  if(remove(libSdobSocket_socket_path) == -1 && errno != ENOENT) {
    dbgprintf(DBG_DEBUG, "Error removing SkydiveOrBust Socket: %d\n", errno);
    libSdobSocketThreadKill = 1;
  }
  
  setnonblock(libSdobSocket_fd);

  memset(&svaddr, 0, sizeof(struct sockaddr_un));
  svaddr.sun_family = AF_UNIX;
  if (*libSdobSocket_socket_path == '\0') {
    *svaddr.sun_path = '\0';
    strncpy(svaddr.sun_path+1, libSdobSocket_socket_path+1, sizeof(svaddr.sun_path)-2);
  } else {
    strncpy(svaddr.sun_path, libSdobSocket_socket_path, sizeof(svaddr.sun_path)-1);
  }

  if (bind(libSdobSocket_fd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1) {
    // MPV Connect Error
    dbgprintf(DBG_DEBUG, "%s\n", "SkydiveOrBust Socket Connect Error");
    libSdobSocketThreadKill = 1;
  }

  // Grab MPV Events, sent in JSON format
  while(!libSdobSocketThreadKill) {
    if (!fd_is_valid(libSdobSocket_fd)) {
      // try closing fd
      if (libSdobSocket_fd) { close(libSdobSocket_fd); }
      // reconnect fd
      if (bind(libSdobSocket_fd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1) {
        // MPV Connect Error
        dbgprintf(DBG_DEBUG, "%s\n", "SkydiveOrBust Socket ReConnect Error");
        libSdobSocketThreadKill = 1;
      }
    }

    
    // Grab Next Socket Line
    len = sizeof(struct sockaddr_un);
    numBytes = recvfrom(libSdobSocket_fd, libSdobSocket_buf, libSdobSocket_buf_size, 0, (struct sockaddr*) &claddr, &len);
    if (numBytes > 0) {
      dbgprintf(DBG_DEBUG, "Received %ld bytes from %s\n", (long) numBytes, claddr.sun_path);
      // dbgprintf(DBG_DEBUG, "%s\n", libSdobSocket_buf);

      char* dubbing_event;
      char* dubbing_data;
      // int sdobox_event_len = 
      ta_json_parse(libSdobSocket_buf, "event", &dubbing_event);
      int sdobox_data_len = ta_json_parse(libSdobSocket_buf, "data", &dubbing_data);
      
      if (sdobox_data_len > 0) {
        printf("Got Data! %s\n", dubbing_data);
      }
      free(dubbing_event);
      free(dubbing_data);
/*
      char* dubbing_event;
      char* dubbing_filename;
      int dubbing_event_len = ta_json_parse(libSdobSocket_buf, "event", &dubbing_event);
      int dubbing_filename_len = ta_json_parse(libSdobSocket_buf, "filename", &dubbing_filename);
      CLEAR(libSdobSocket_buf, libSdobSocket_buf_size);

      dbgprintf(DBG_DEBUG, "Event: %s\n", dubbing_event);
      dbgprintf(DBG_DEBUG, "Filename: %s\n", dubbing_filename);

//      // Do stuff with datagram
//      char delim[] = " ";
//      char *cAction = strtok(libSdobSocket_buf, delim);
//      char *cData = strtok(NULL, delim);

//      dbgprintf(DBG_DEBUG, "Action: %s\n", cAction);
//      dbgprintf(DBG_DEBUG, "Data: %s\n", cData);
      // cleanup
      if (dubbing_event_len) { CLEAR(dubbing_event, dubbing_event_len); }
      if (dubbing_filename_len) { CLEAR(dubbing_filename, dubbing_filename_len); }
*/
      
      usleep(100);
      CLEAR(libSdobSocket_buf, libSdobSocket_buf_size);
    } else if (libSdobSocket_WriteQueueLen > 0) {
      pthread_mutex_lock(&libSdobSocketWriteLock);
      // Try to Write Something
      struct queue_head *item = queue_get(libSdobSocket_WriteQueue, &libSdobSocket_WriteQueueLen);
      if (item) {
        if (item->data != NULL) {
          int jfd = sendto(libSdobSocket_fd, (char*) item->data, strlen((char*) item->data), 0, (struct sockaddr*) &claddr, len);
          if (jfd != strlen((char*) item->data)) {
            dbgprintf(DBG_DEBUG, "Error sendto sdobox.socket %s\n", strerror(errno));
          }
          dbgprintf(DBG_DEBUG, "%s\n", "FREE THE DATA");
          free(item->data);
        }
        free(item);
      }
      pthread_mutex_unlock(&libSdobSocketWriteLock);
      usleep(100);
    } else {
      usleep(200000);
    }

  }
  // close

  dbgprintf(DBG_DEBUG, "%s\n", "Closing SkydiveOrBust Socket");
  libSdobSocketThreadRunning = 0;
  return NULL;
}


int libSdobSocketThreadStart() {
  dbgprintf(DBG_DEBUG, "%s\n", "libSdobSocketThreadStart()");
  if (libSdobSocketThreadRunning) { return 0; }

  libSdobSocket_socket_path = "/tmp/sdobox.socket";
  libSdobSocket_buf_size = 8192;

  libSdobSocket_WriteQueue = ALLOC_QUEUE_ROOT();
  libSdobSocket_WriteQueueLen = 0;

  dbgprintf(DBG_DEBUG, "SkydiveOrBust Socket Thread Spinup: %d\n", libSdobSocketThreadRunning);
  libSdobSocketThreadKill = 0;
  return piThreadCreate(libSdobSocketThread);
}

void libSdobSocketThreadStop() {
  dbgprintf(DBG_DEBUG, "%s\n", "libSdobSocketThreadStop()");
  // Shutdown MPV Socket Thread
  if (libSdobSocketThreadRunning) {
    libSdobSocketThreadKill = 1;
    int shutdown_cnt = 0;
    while (libSdobSocketThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    dbgprintf(DBG_DEBUG, "SkydiveOrBust Socket Thread Shutdown %d\n", shutdown_cnt);
  }

  free(libSdobSocket_WriteQueue);
  libSdobSocket_WriteQueueLen = 0;
}