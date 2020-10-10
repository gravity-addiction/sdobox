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
#include "libs/usb-drives/usb-drives-thread.h"

#include "sdob-socket.h"

// ------------------------
// SDOBOX Socket Thread
// ------------------------
// static pthread_mutex_t libSdobSocketWriteLock = PTHREAD_MUTEX_INITIALIZER;

PI_THREAD (libSdobSocketThread)
{
  // Check if Socket Running
  if (libSdobSocketThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting SkydiveOrBust Socket Thread, Already Started");
    return NULL;
  }
  libSdobSocketThreadRunning = 1;

  // Check if Socket is Killed
  if (libSdobSocketThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting SkydiveOrBust Socket Thread, Stop Flag Set");
    libSdobSocketThreadRunning = 0;
    return NULL;
  }

  // Checks good, lets start
  dbgprintf(DBG_DEBUG, "%s\n", "Starting SkydiveOrBust Socket Thread");

  // Setup Vars
  struct sockaddr_un svaddr, claddr;
  ssize_t numBytes;
  socklen_t len;
  char libSdobSocket_buf[libSdobSocket_buf_size];

  // Open DGram Socket
  libSdobSocket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (libSdobSocket_fd == -1) {
    dbgprintf(DBG_DEBUG, "%s\n", "SkydiveOrBust Socket Error");
    libSdobSocketThreadKill = 1;
  }

   // Socket Path Length
  if (strlen(libSdobSocket_socket_path) > sizeof(svaddr.sun_path)-1) {
    dbgprintf(DBG_DEBUG, "SkydiveOrBust Socket path to long must be %d chars\n", sizeof(svaddr.sun_path)-1);
    libSdobSocketThreadKill = 1;
  }

  // Remove Old Socket
  if(remove(libSdobSocket_socket_path) == -1 && errno != ENOENT) {
    dbgprintf(DBG_DEBUG, "Error removing SkydiveOrBust Socket: %d\n", errno);
    libSdobSocketThreadKill = 1;
  }
  
  // Non-blocking socket
  setnonblock(libSdobSocket_fd);

  // Create Server Socket
  memset(&svaddr, 0, sizeof(struct sockaddr_un));
  svaddr.sun_family = AF_UNIX;
  if (*libSdobSocket_socket_path == '\0') {
    *svaddr.sun_path = '\0';
    strncpy(svaddr.sun_path+1, libSdobSocket_socket_path+1, sizeof(svaddr.sun_path)-2);
  } else {
    strncpy(svaddr.sun_path, libSdobSocket_socket_path, sizeof(svaddr.sun_path)-1);
  }

  if (bind(libSdobSocket_fd, (struct sockaddr*) &svaddr, sizeof(struct sockaddr_un)) == -1) {
    // MPV Connect Error
    dbgprintf(DBG_DEBUG, "%s\n", "SkydiveOrBust Socket Connect Error");
    libSdobSocketThreadKill = 1;
  }

  
  // Grab MPV Events, sent in JSON format
  while(!libSdobSocketThreadKill) {
    len = sizeof(struct sockaddr_un);

//     if (!fd_is_valid(libSdobSocket_fd)) {
//      // try closing fd
//      if (libSdobSocket_fd) { close(libSdobSocket_fd); }
//      // reconnect fd
//      printf("reconnect sdob socket fd\n");
//      if (bind(libSdobSocket_fd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1) {
//        // MPV Connect Error
//        dbgprintf(DBG_DEBUG, "%s\n", "SkydiveOrBust Socket ReConnect Error");
//        libSdobSocketThreadKill = 1;
//      }
//    }
    
    // Grab Next Socket Line
    numBytes = recvfrom(libSdobSocket_fd, libSdobSocket_buf, libSdobSocket_buf_size, 0, (struct sockaddr*) &claddr, &len);
    if (numBytes > 0) {
      dbgprintf(DBG_DEBUG, "Received %ld bytes from %s\n", (long) numBytes, claddr.sun_path);
      // dbgprintf(DBG_DEBUG, "%s\n", libSdobSocket_buf);

      char* sdob_event;
      int sdob_event_len = ta_json_parse(libSdobSocket_buf, "event", &sdob_event);

      char* sdob_syslog;
      int sdob_syslog_len = ta_json_parse(libSdobSocket_buf, "syslog", &sdob_syslog);

      if (sdob_event_len > 0) { dbgprintf(DBG_DEBUG, "SDOB Event %s\n", sdob_event); }
      if (sdob_syslog_len > 0) {
        dbgprintf(DBG_DEBUG, "SDOB SysLog %s\n", sdob_syslog);
        free(sdob_syslog);
      }

      if (sdob_event_len > 0 && strcmp(sdob_event, "usb-drive") == 0 && libUsbDrivesThreadRunning) {
        char* tmp;
        int sdob_data_len = ta_json_parse(libSdobSocket_buf, "data", &tmp);

        if (sdob_data_len > 0) {
          char *sdob_data = malloc(sdob_data_len + 1);
          strncpy(sdob_data, tmp, sdob_data_len + 1);
          free(tmp);

          struct queue_head *itemUsbDrives = new_qhead();
          itemUsbDrives->action = 0;
          itemUsbDrives->data = sdob_data;
          queue_put(itemUsbDrives, libUsbDrives_Queue, &libUsbDrives_QueueLen);
          
        }
      }
      if (sdob_event_len > 0) { free(sdob_event); }

      usleep(100);
      CLEAR(libSdobSocket_buf, libSdobSocket_buf_size);
    } else if (libSdobSocket_WriteQueueLen > 0) {

      // Try to Write Something
      struct queue_head *item = queue_get(libSdobSocket_WriteQueue, &libSdobSocket_WriteQueueLen);
      if (item) {
        if (item->data != NULL) {
          dbgprintf(DBG_DEBUG, "Sending data len: %d - %s\n", strlen((char*) item->data), item->data);
          int jfd = sendto(libSdobSocket_fd, item->data, strlen((char*) item->data), 0, (struct sockaddr*) &claddr, len);
          if (jfd != strlen((char*) item->data)) {
            dbgprintf(DBG_DEBUG, "Error sendto sdobox.socket %s\n", strerror(errno));
          }
          free(item->data);
        }
        free(item);
      }

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