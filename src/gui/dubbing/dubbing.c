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
#include "libs/mpv/mpv_info.h"
#include "gui/pages.h"
#include "libs/fbcp/fbcp.h"
#include "libs/sdob-socket/sdob-socket.h"
#include "libs/usb-drives/usb-drives.h"

#include "gui/usbdrive/usbdrive.h"


void pg_dubbing_setSlateTime() {
  mpv_any_u* retTimePos;

  if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
    if (retTimePos->hasPtr == 1) { pg_dubbing_videoSlate = atof(retTimePos->ptr);
    } else { pg_dubbing_videoSlate = retTimePos->floating;
    }
    dbgprintf(DBG_DEBUG, "Slate: %f\n", pg_dubbing_videoSlate);

    int sLen = snprintf(NULL, 0, "{\"slate\":\"%f\"}", retTimePos->floating) + 1;
    char *sdata = malloc(sLen);
    snprintf(sdata, sLen, "{\"slate\":\"%f\"}", retTimePos->floating);
    MPV_ANY_U_FREE(retTimePos);

    struct queue_head *item = new_qhead();
    item->data = sdata;
    queue_put(item, libSdobSocket_WriteQueue, &libSdobSocket_WriteQueueLen);
  }
}

void pg_dubbing_setExitTime() {
  mpv_any_u* retTimePos;

  if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
    if (retTimePos->hasPtr == 1) { pg_dubbing_videoExit = atof(retTimePos->ptr);
    } else { pg_dubbing_videoExit = retTimePos->floating;
    }
    dbgprintf(DBG_DEBUG, "Exit: %f\n", pg_dubbing_videoExit);

    int sLen = snprintf(NULL, 0, "{\"exit\":\"%f\"}", retTimePos->floating) + 1;
    char *sdata = malloc(sLen);
    snprintf(sdata, sLen, "{\"exit\":\"%f\"}", retTimePos->floating);
    MPV_ANY_U_FREE(retTimePos);

    struct queue_head *item = new_qhead();
    item->data = sdata;
    queue_put(item, libSdobSocket_WriteQueue, &libSdobSocket_WriteQueueLen);
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

bool pg_dubbing_cbBtn_usbDrive_A(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_dubbing_driveI_A == -1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_USBDRIVE);
  pg_usbdrive_loadDrive(pg_dubbing_driveI_A);

  return true;
}

bool pg_dubbing_cbBtn_usbDrive_B(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_dubbing_driveI_B == -1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_USBDRIVE);
  pg_usbdrive_loadDrive(pg_dubbing_driveI_B);
  return true;
}

bool pg_dubbing_cbBtn_usbDrive_C(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_dubbing_driveI_C == -1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_USBDRIVE);
  pg_usbdrive_loadDrive(pg_dubbing_driveI_C);
  return true;
}

bool pg_dubbing_cbBtn_usbDrive_D(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_dubbing_driveI_D == -1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_USBDRIVE);
  pg_usbdrive_loadDrive(pg_dubbing_driveI_D);
  return true;
}


void pg_dubbing_setupButtons(gslc_tsGui *pGui) {
  if (libMpvVideoInfo->is_loaded) {
    gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], true);
    gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], true);
    gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], true);
    gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], true);
  } else {
    gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], false);
    gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], false);
    gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], false);
    gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], false);
  }
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
  // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_SETSLATE], false);

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
  // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_SETEXIT], false);


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
  // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_STOPVIDEO], false);


  // Play / Pause Video
  pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {123,210,110,110},
          "Play / Pause", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_playPauseVideo);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], true);
  // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_PLAYPAUSEVIDEO], false);





  // USB Drive A
  pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {0,125,110,70},
          (char*)" ", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_usbDrive_A);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], GSLC_COL_WHITE, GSLC_COL_GREEN_DK4, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], true);
  // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], false);

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){0, 125, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA], GSLC_ALIGN_BOT_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA], false);
  }

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){0, 150, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB], false);
  }

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){0, 175, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC], GSLC_ALIGN_TOP_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC], false);
  }



  // USB Drive B
  pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {123,125,110,70},
          "", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_usbDrive_B);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], GSLC_COL_WHITE, GSLC_COL_GREEN_DK4, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], true);
  // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], false);

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){123, 125, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA], GSLC_ALIGN_BOT_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA], false);
  }

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){123, 150, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB], false);
  }

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){123, 175, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC], GSLC_ALIGN_TOP_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC], false);
  }



  // USB Drive C
  pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {247,125,110,70},
          "", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_usbDrive_C);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], GSLC_COL_WHITE, GSLC_COL_GREEN_DK4, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], true);
  // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], false);

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){247, 125, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA], GSLC_ALIGN_BOT_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA], false);
  }

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){247, 150, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB], false);
  }

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){247, 175, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC], GSLC_ALIGN_TOP_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC], false);
  }



    // USB Drive D
  pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {370,125,110,70},
          "", 0, E_FONT_MONO14, &pg_dubbing_cbBtn_usbDrive_D);
  gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], GSLC_COL_WHITE, GSLC_COL_GREEN_DK4, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], false);
  gslc_ElemSetGlowEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], false);
  gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], true);
  // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], false);


  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){370, 125, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA], GSLC_ALIGN_BOT_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA], false);
  }

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){370, 150, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB], false);
  }

  if ((
    pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){370, 175, 110, 25},
          (char*)" ", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC], GSLC_ALIGN_TOP_MID);
    gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC], false);
    gslc_ElemSetFrameEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC], false);
    // gslc_ElemSetVisible(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC], false);
  }





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

void pg_dubbing_clean_usbButtons(gslc_tsGui *pGui) {
  char* blank = " ";
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], false);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], false);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], false);
  gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], false);

  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB], blank);
  gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC], blank);

  pg_dubbing_driveI_A = 0;
  pg_dubbing_driveI_B = 0;
  pg_dubbing_driveI_C = 0;
  pg_dubbing_driveI_D = 0;
}


int pg_dubbing_thread(gslc_tsGui *pGui) {
  // MPV Event
  if (libMpvVideoInfo->cnt > 0 && pg_dubbing_libMpvCnt != libMpvVideoInfo->cnt) {
    libMpvVideoInfo->cnt = pg_dubbing_libMpvCnt;
    pg_dubbing_setupButtons(pGui);
  }

  // USB Event
  if (libUsbDrivesCount->cnt > 0 && pg_dubbing_libUsbCnt != libUsbDrivesCount->cnt) {
    dbgprintf(DBG_DEBUG, "Update USB Drives! Drives Count: %d\n", libUsbDrivesCount->cur);
    pg_dubbing_clean_usbButtons(pGui);
    int dLen = libUsbDrivesCount->cur;
    // Find Local Drive
    int rD = 0;
    for (int d = 0; d < dLen; d++) {
      // List Partitions
      int tPartCntLen = snprintf(NULL, 0, "%d Partitions", libUsbDrivesList[d]->partitionMax) + 1;
      char *tPartCnt = (char*)malloc(tPartCntLen * sizeof(char));
      snprintf(tPartCnt, tPartCntLen, "%d Partitions", libUsbDrivesList[d]->partitionMax);

      int hasMount = 0;
      double largestMount = 0.00;
      // Check Mounted Partitions
      for (int cP = 0; cP < libUsbDrivesList[d]->partitionMax; cP++) {
        if (libUsbDrivesList[d]->partitions[cP] == NULL) { continue; }
        // printf( "Drive: %s - Mounted: %s\n", libUsbDrivesList[d]->name, libUsbDrivesList[d]->partitions[cP]->mountpoint);
        if (
          libUsbDrivesList[d]->partitions[cP]->mountpoint != NULL &&
          strcmp(libUsbDrivesList[d]->partitions[cP]->mountpoint, "null") &&
          strlen(libUsbDrivesList[d]->partitions[cP]->mountpoint) > 0
        ) {
          hasMount = 1;
          double tMountSize = atof(libUsbDrivesList[d]->partitions[cP]->drivesize);
          if (tMountSize > largestMount) {
            libUsbDrivesList[d]->partitionLargest = cP;
            largestMount = tMountSize;
          }
        }
      }

      if (strcmp(libUsbDrivesList[d]->name, "mmcblk0") == 0) {
        /* pg_dubbing_driveI_A = d;
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AA], (char*)"LOCAL DRIVE");
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AB], libUsbDrivesList[d]->drivesize);
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_AC], tPartCnt);
        gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_A], (hasMount)?true:false);
        */
      } else if (rD == 0) {
        pg_dubbing_driveI_B = d;
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BA], libUsbDrivesList[d]->name);
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BB], libUsbDrivesList[d]->drivesize);
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_BC], tPartCnt);
        gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_B], (hasMount)?true:false);
        rD++;
      } else if (rD == 1) {
        pg_dubbing_driveI_C = d;
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CA], libUsbDrivesList[d]->name);
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CB], libUsbDrivesList[d]->drivesize);
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_CC], tPartCnt);
        gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_C], (hasMount)?true:false);
        rD++;
      } else if (rD == 2) {
        pg_dubbing_driveI_D = d;
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DA], libUsbDrivesList[d]->name);
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DB], libUsbDrivesList[d]->drivesize);
        gslc_ElemSetTxtStr(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_DC], tPartCnt);
        gslc_ElemSetFillEn(pGui, pg_dubbingEl[E_DUBBING_EL_USBDRIVE_D], (hasMount)?true:false);
        rD++;
      }

      free(tPartCnt);
      // printf("Drive: %s (%s)\n", libUsbDrivesList[d]->name, libUsbDrivesList[d]->drivesize);
    }
    pg_dubbing_libUsbCnt = libUsbDrivesCount->cnt;
  }
  return 0;
}

// GUI Destroy
void pg_dubbing_destroy(gslc_tsGui *pGui) {

}

void __attribute__ ((constructor)) pg_dubbing_constructor(void) {
  cbInit[E_PG_DUBBING] = &pg_dubbing_init;
  cbOpen[E_PG_DUBBING] = &pg_dubbing_open;
  cbClose[E_PG_DUBBING] = &pg_dubbing_close;
  cbThread[E_PG_DUBBING] = &pg_dubbing_thread;
  cbDestroy[E_PG_DUBBING] = &pg_dubbing_destroy;
  // pg_dubbingQueue = ALLOC_QUEUE_ROOT();
}

void __attribute__ ((destructor)) pg_dubbing_destructor(void) {

}

