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

#include "usb-drives.h"
#include "usb-drives-thread.h"

// ------------------------
// USB Drives Socket Thread
// ------------------------

PI_THREAD (libUsbDrivesThread)
{
  // Check if Socket Running
  if (libUsbDrivesThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting USB Drives Thread, Already Started");
    return NULL;
  }
  libUsbDrivesThreadRunning = 1;

  // Check if Socket is Killed
  if (libUsbDrivesThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting USB Drives Thread, Stop Flag Set");
    libUsbDrivesThreadRunning = 0;
    return NULL;
  }

  // Checks good, lets start
  dbgprintf(DBG_DEBUG, "%s\n", "Starting USB Drives Thread");

  libUsbDrives_Queue = ALLOC_QUEUE_ROOT();
  libUsbDrives_QueueLen = 0;

  // Grab MPV Events, sent in JSON format
  while(!libUsbDrivesThreadKill) {
    if (libUsbDrives_QueueLen > 0) {
      // Dump Most Duplicates
      // while(libUsbDrives_QueueLen > 0) {
      //   struct queue_head *item = queue_get(libUsbDrives_Queue, &libUsbDrives_QueueLen);
      //   if (item->data != NULL) { free(item->data); }
      //   free(item);
      //   usleep(100);
      // }
      struct queue_head *item = queue_get(libUsbDrives_Queue, &libUsbDrives_QueueLen);
      if (item != NULL) {
        if (item->data != NULL) {
          // libUsbDrives_cleanDriveList(libUsbDrivesList, libUsbDrivesCount);
          // libUsbDrivesCount->cur = 0;
          libUsbDrives_parse_lsblk(item->data, libUsbDrivesList, libUsbDrivesCount);
          free(item->data);
        }
        free(item);
      }
    }
    usleep(200000);
  }

  dbgprintf(DBG_DEBUG, "%s\n", "Closing USB Drives");
  libUsbDrivesThreadRunning = 0;
  return NULL;
}


int libUsbDrivesThreadStart() {
  dbgprintf(DBG_DEBUG, "%s\n", "libUsbDrivesThreadStart()");
  if (libUsbDrivesThreadRunning) { return 0; }

  libUsbDrivesCount = (struct libUsbDrivesCounter*)malloc(sizeof(struct libUsbDrivesCounter) * 5);
  libUsbDrivesCount->max = 5;
  libUsbDrivesCount->cur = 0;
  libUsbDrivesCount->cnt = 0;

  libUsbDrivesList = (struct libUsbDrivesHardware**)malloc(sizeof(struct libUsbDrivesHardware*) * libUsbDrivesCount->max);
  for (int dl = 0; dl < libUsbDrivesCount->max; dl++) {
    libUsbDrivesList[dl] = (struct libUsbDrivesHardware*)malloc(sizeof(struct libUsbDrivesHardware));
    libUsbDrivesList[dl]->partitionCur = 0;
    libUsbDrivesList[dl]->partitionMax = 0;
  }
  dbgprintf(DBG_DEBUG, "USB Drives Thread Spinup: %d\n", libUsbDrivesThreadRunning);
  libUsbDrivesThreadKill = 0;
  return piThreadCreate(libUsbDrivesThread);
}

void libUsbDrivesThreadStop() {
  dbgprintf(DBG_DEBUG, "%s\n", "libUsbDrivesThreadStop()");
  // Shutdown USB Drives Thread
  if (libUsbDrivesThreadRunning) {
    libUsbDrivesThreadKill = 1;
    int shutdown_cnt = 0;
    while (libUsbDrivesThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    dbgprintf(DBG_DEBUG, "USB Drives Thread Shutdown %d\n", shutdown_cnt);
  }

  libUsbDrives_cleanDriveList(libUsbDrivesList, libUsbDrivesCount);
  free(libUsbDrivesList);
  free(libUsbDrivesCount);

  free(libUsbDrives_Queue);
  libUsbDrives_QueueLen = 0;
}