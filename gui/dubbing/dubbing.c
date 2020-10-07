// System Headers
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "dubbing.h"

#include "libs/shared.h"
#include "libs/queue/queue.h"
#include "libs/buttons/buttons.h"
#include "libs/mpv/mpv.h"
#include "libs/mpv/mpv_events.h"
#include "gui/pages.h"
#include "libs/fbcp/fbcp.h"


void pg_dubbing_setSlateTime() {
  char* retTimePos;

  if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
    pg_dubbing_videoSlate = atof(retTimePos);
    dbgprintf(DBG_DEBUG, "Slate: %f\n", pg_dubbing_videoSlate);
    free(retTimePos);
  }
}

void pg_dubbing_setExitTime() {
  char* retTimePos;

  if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
    pg_dubbing_videoExit = atof(retTimePos);
    dbgprintf(DBG_DEBUG, "Exit: %f\n", pg_dubbing_videoExit);
    free(retTimePos);

  }
}

bool pg_dubbing_cbBtn_close(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageGoBack(pGui);
  return true;
}

bool pg_dubbing_cbBtn_stopVideo(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
  mpv_stop();
  return true;
}

bool pg_dubbing_cbBtn_playPauseVideo(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
  mpv_playpause_toggle();
  return true;
}

bool pg_dubbing_cbBtn_setExit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  return true;
}

bool pg_dubbing_cbBtn_setSlate(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  return true;
}

bool pg_dubbing_cbBtn_fbcp(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  guislice_wrapper_mirror_toggle(pGui);
  return true;
}

void pg_dubbingGuiInit(gslc_tsGui *pGui) {
  int ePage = E_PG_DUBBING;

  gslc_PageAdd(pGui, E_PG_DUBBING, pg_dubbingElem, MAX_ELEM_PG_DUBBING_RAM,
               pg_dubbingElemRef, MAX_ELEM_PG_DUBBING);


  // Set Slate
  pg_dubbingEl[E_DUBBING_EL_SETSLATE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {0,0,110,110},
          "Set Slate", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_setSlate);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], true);

  // Set Exit
  pg_dubbingEl[E_DUBBING_EL_SETEXIT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {370,0,110,110},
          "Set Exit", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_setExit);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], true);


  // Stop Video
  pg_dubbingEl[E_DUBBING_EL_STOPVIDEO] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {0,210,110,110},
          "Stop Video", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_stopVideo);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], true);


  // Play / Pause Video
  pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {150,210,110,110},
          "Play / Pause", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_playPauseVideo);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], true);



  // Close Key
  pg_dubbingEl[E_DUBBING_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {420,270,60,50},
          "Close", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_CLOSE], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_CLOSE], true);



}

void pg_dubbingButtonRotaryCW() {
  mpv_seek(2);
}

void pg_dubbingButtonRotaryCCW() {
  mpv_seek(-2);
}


void pg_dubbingButtonLeftPressed() {
  pg_dubbing_setSlateTime();
}

void pg_dubbingButtonRightPressed() {
  pg_dubbing_setExitTime();
}

void pg_dubbingButtonRotaryPressed() {
  mpv_playpause_toggle();
}

void pg_dubbingButtonLeftHeld() {

}

void pg_dubbingButtonRightHeld() {

}


void pg_dubbingButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_dubbingButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_dubbingButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_dubbingButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_dubbingButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_dubbingButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_dubbingButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_dubbingButtonRightHeld);
}

void pg_dubbingButtonUnsetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, NULL);
}





// ------------------------
// MPV Socket Thread
// ------------------------
PI_THREAD (pg_dubbingSocketThread)
{
  if (pg_dubbingSocketThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting Dubbing Event Thread, Already Started");
    return NULL;
  }
  pg_dubbingSocketThreadRunning = 1;

  if (pg_dubbingSocketThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting Dubbing Event Thread, Stop Flag Set");
    pg_dubbingSocketThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_DEBUG, "%s\n", "Starting Dubbing Event Thread");

  struct sockaddr_un svaddr, claddr;
  ssize_t numBytes;
  socklen_t len;
  char pg_dubbing_buf[pg_dubbing_buf_size];

  pg_dubbing_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (pg_dubbing_fd == -1) {
    dbgprintf(DBG_DEBUG, "%s\n", "Dubbing Socket Error");
    pg_dubbingSocketThreadKill = 1;
  }

  if (strlen(pg_dubbing_socket_path) > sizeof(svaddr.sun_path)-1) {
    dbgprintf(DBG_DEBUG, "Dubbing Socket path to long must be %d chars\n", sizeof(svaddr.sun_path-1));
    pg_dubbingSocketThreadKill = 1;
  }

  if(remove(pg_dubbing_socket_path) == -1 && errno != ENOENT) {
    dbgprintf(DBG_DEBUG, "Error removing socket: %d\n", errno);
    pg_dubbingSocketThreadKill = 1;
  }
  
  setnonblock(pg_dubbing_fd);

  memset(&svaddr, 0, sizeof(struct sockaddr_un));
  svaddr.sun_family = AF_UNIX;
  if (*pg_dubbing_socket_path == '\0') {
    *svaddr.sun_path = '\0';
    strncpy(svaddr.sun_path+1, pg_dubbing_socket_path+1, sizeof(svaddr.sun_path)-2);
  } else {
    strncpy(svaddr.sun_path, pg_dubbing_socket_path, sizeof(svaddr.sun_path)-1);
  }

  if (bind(pg_dubbing_fd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1) {
    // MPV Connect Error
    dbgprintf(DBG_DEBUG, "%s\n", "Dubbing Socket Connect Error");
    pg_dubbingSocketThreadKill = 1;
  }

  // Grab MPV Events, sent in JSON format
  while(!pg_dubbingSocketThreadKill) {
    if (!fd_is_valid(pg_dubbing_fd)) {
      // printf("FD Re-Connect: %d, %d\n", pg_dubbing_fd_timer, millis());
      // try closing fd
      if (pg_dubbing_fd) { close(pg_dubbing_fd); }
      // reconnect fd
      if (bind(pg_dubbing_fd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1) {
        // MPV Connect Error
        dbgprintf(DBG_DEBUG, "%s\n", "Dubbing Socket ReConnect Error");
        pg_dubbingSocketThreadKill = 1;
      }
    }


    // Grab Next Socket Line
    len = sizeof(struct sockaddr_un);
    numBytes = recvfrom(pg_dubbing_fd, pg_dubbing_buf, pg_dubbing_buf_size, 0, (struct sockaddr*) &claddr, &len);
    if (numBytes > 0) {
      dbgprintf(DBG_DEBUG, "Received %ld bytes from %s\n", (long) numBytes, claddr.sun_path);
      dbgprintf(DBG_DEBUG, "%s\n", pg_dubbing_buf);

      char* dubbing_event;
      char* dubbing_filename;
      int dubbing_event_len = ta_json_parse(pg_dubbing_buf, "event", &dubbing_event);
      int dubbing_filename_len = ta_json_parse(pg_dubbing_buf, "filename", &dubbing_filename);
      CLEAR(pg_dubbing_buf, pg_dubbing_buf_size);

      dbgprintf(DBG_DEBUG, "Event: %s\n", dubbing_event);
      dbgprintf(DBG_DEBUG, "Filename: %s\n", dubbing_filename);

//      // Do stuff with datagram
//      char delim[] = " ";
//      char *cAction = strtok(pg_dubbing_buf, delim);
//      char *cData = strtok(NULL, delim);

//      dbgprintf(DBG_DEBUG, "Action: %s\n", cAction);
//      dbgprintf(DBG_DEBUG, "Data: %s\n", cData);
      // cleanup
      if (dubbing_event_len) { CLEAR(dubbing_event, dubbing_event_len); }
      if (dubbing_filename_len) { CLEAR(dubbing_filename, dubbing_filename_len); }
      
      usleep(100);
    } else {
      usleep(200000);
    }

  }
  // close

  dbgprintf(DBG_DEBUG, "%s\n", "Closing Dubbing RPC");
  pg_dubbingSocketThreadRunning = 0;
  return NULL;
}


int pg_dubbingSocketThreadStart() {
  dbgprintf(DBG_DEBUG, "%s\n", "pg_dubbingSocketThreadStart()");
  if (pg_dubbingSocketThreadRunning) { return 0; }

  dbgprintf(DBG_DEBUG, "SkydiveOrBust Dubbing Socket Thread Spinup: %d\n", pg_dubbingSocketThreadRunning);
  pg_dubbingSocketThreadKill = 0;
  return piThreadCreate(pg_dubbingSocketThread);
}

void pg_dubbingSocketThreadStop() {
  dbgprintf(DBG_DEBUG, "%s\n", "pg_dubbingSocketThreadStop()");
  // Shutdown MPV Socket Thread
  if (pg_dubbingSocketThreadRunning) {
    pg_dubbingSocketThreadKill = 1;
    int shutdown_cnt = 0;
    while (pg_dubbingSocketThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    dbgprintf(DBG_DEBUG, "SkydiveOrBust Dubbing Socket Thread Shutdown %d\n", shutdown_cnt);
  }
}



// GUI Init
void pg_dubbing_init(gslc_tsGui *pGui) {
  pg_dubbing_socket_path = "/tmp/dubbing.socket";
  pg_dubbing_buf_size = 256;
  pg_dubbingGuiInit(pGui);

  pg_dubbingSocketThreadKill = 0; // Stopping SDOB Thread
  pg_dubbingSocketThreadRunning = 0; // Running flag for SDOB Thread

  cbInit[E_PG_DUBBING] = NULL;
}


// GUI Open
void pg_dubbing_open(gslc_tsGui *pGui) {

  pg_dubbingButtonSetFuncs();

  dbgprintf(DBG_DEBUG, "%s\n", "Page Dubbing Starting Socket Thread");
  pg_dubbingSocketThreadStart();
}


void pg_dubbing_close(gslc_tsGui *pGui) {

  dbgprintf(DBG_DEBUG, "%s\n", "Page Dubbing Stopping Socket Thread");
  pg_dubbingSocketThreadStop();

}

int pg_dubbing_thread(gslc_tsGui *pGui) {
  return 0;
}

// GUI Destroy
void pg_dubbing_destroy(gslc_tsGui *pGui) {

}

void __attribute__ ((constructor)) pg_dubbing_constructor(void) {
  cbInit[E_PG_DUBBING] = &pg_dubbing_init;
  cbOpen[E_PG_DUBBING] = &pg_dubbing_open;
  cbClose[E_PG_DUBBING] = &pg_dubbing_close;
  // cbThread[E_PG_DUBBING] = &pg_dubbing_thread;
  cbDestroy[E_PG_DUBBING] = &pg_dubbing_destroy;
  // pg_dubbingQueue = ALLOC_QUEUE_ROOT();
}

void __attribute__ ((destructor)) pg_dubbing_destructor(void) {

}

