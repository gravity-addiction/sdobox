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
#include "libs/sdob-socket/sdob-socket.h"


void pg_dubbing_setSlateTime() {
  char* retTimePos;

  if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
    pg_dubbing_videoSlate = atof(retTimePos);
    dbgprintf(DBG_DEBUG, "Slate: %f\n", pg_dubbing_videoSlate);
    struct queue_head *item = new_qhead();
    item->data = strdup(retTimePos);
    queue_put(item, libSdobSocket_WriteQueue, &libSdobSocket_WriteQueueLen);

    free(retTimePos);
  }
}

void pg_dubbing_setExitTime() {
  char* retTimePos;

  if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
    pg_dubbing_videoExit = atof(retTimePos);
    dbgprintf(DBG_DEBUG, "Exit: %f\n", pg_dubbing_videoExit);
    struct queue_head *item = new_qhead();
    item->data = strdup(retTimePos);
    queue_put(item, libSdobSocket_WriteQueue, &libSdobSocket_WriteQueueLen);
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


// GUI Init
void pg_dubbing_init(gslc_tsGui *pGui) {
  pg_dubbingGuiInit(pGui);

  cbInit[E_PG_DUBBING] = NULL;
}


// GUI Open
void pg_dubbing_open(gslc_tsGui *pGui) {

  pg_dubbingButtonSetFuncs();
}


void pg_dubbing_close(gslc_tsGui *pGui) {

  dbgprintf(DBG_DEBUG, "%s\n", "Page Dubbing Stopping Socket Thread");
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

