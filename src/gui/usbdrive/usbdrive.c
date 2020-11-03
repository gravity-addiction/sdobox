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

#include "usbdrive.h"

#include "libs/shared.h"
#include "libs/queue/queue.h"
#include "libs/buttons/buttons.h"
#include "libs/sdob-socket/sdob-socket.h"
#include "libs/vlisting/vlisting.h"
#include "libs/usb-drives/usb-drives.h"

int pg_usbdrive_thread_usbCnt = -1;

void pg_usbdrive_loadDrive(int driveI) {
  if (driveI > libUsbDrivesCount->max) { libUsbDrivesI = -1; return; }
  libUsbDrivesI = driveI;
  pg_usbDrive_listConfig->len = libUsbDrivesList[libUsbDrivesI]->partitionMax;
  pg_usbDrive_listConfig->cur = -1;
  gslc_ElemSetRedraw(&m_gui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);
}




//////////////////
// Box Drawing
bool pg_usbDrive_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);

  vlist_sliderFill(pGui, pg_usbDrive_listConfig);
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}


// A
bool pg_usbDrive_cbBtn_elA(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_usbDrive_listConfig, 0)) { return true; }
  int bIndex = vlist_getBtnIndex(pg_usbDrive_listConfig, 0);
  if (bIndex > libUsbDrivesList[libUsbDrivesI]->partitionMax) { return true; }

  libUsbDrivesList[libUsbDrivesI]->partitionCur = bIndex;
  pg_usbdrive_thread_usbCnt--;

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

// B
bool pg_usbDrive_cbBtn_elB(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_usbDrive_listConfig, 1)) { return true; }
  int bIndex = vlist_getBtnIndex(pg_usbDrive_listConfig, 1);
  if (bIndex > libUsbDrivesList[libUsbDrivesI]->partitionMax) { return true; }

  libUsbDrivesList[libUsbDrivesI]->partitionCur = bIndex;
  pg_usbdrive_thread_usbCnt--;

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

// C
bool pg_usbDrive_cbBtn_elC(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_usbDrive_listConfig, 2)) { return true; }
  int bIndex = vlist_getBtnIndex(pg_usbDrive_listConfig, 2);
  if (bIndex > libUsbDrivesList[libUsbDrivesI]->partitionMax) { return true; }

  libUsbDrivesList[libUsbDrivesI]->partitionCur = bIndex;
  pg_usbdrive_thread_usbCnt--;

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

// D
bool pg_usbDrive_cbBtn_elD(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_usbDrive_listConfig, 3)) { return true; }
  int bIndex = vlist_getBtnIndex(pg_usbDrive_listConfig, 3);
  if (bIndex > libUsbDrivesList[libUsbDrivesI]->partitionMax) { return true; }

  libUsbDrivesList[libUsbDrivesI]->partitionCur = bIndex;
  pg_usbdrive_thread_usbCnt--;

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}



bool pg_usbDrive_cbBtn_sliderPos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsXSlider* pSlider   = (gslc_tsXSlider*)(pElem->pXData);

  // Fetch the new RGB component from the slider
  if (pSlider->eTouch == GSLC_TOUCH_DOWN_IN ||
      pSlider->eTouch == GSLC_TOUCH_MOVE_IN ||
      pSlider->eTouch == GSLC_TOUCH_MOVE_OUT
  ) {
    // Set slider config
    vlist_sliderSetPos(pGui, pg_usbDrive_listConfig, gslc_ElemXSliderGetPos(pGui, pElemRef));
    // Update Visual List
    gslc_ElemSetRedraw(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);
  }
  return true;
}

bool pg_usbDrive_cbBtn_sliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_usbDrive_listConfig, -1);
  gslc_ElemSetRedraw(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

bool pg_usbDrive_cbBtn_sliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_usbDrive_listConfig, 1);
  gslc_ElemSetRedraw(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}





bool pg_usbdrive_cbBtn_close(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageGoBack(pGui);
  return true;
}

bool pg_usbdrive_cbBtn_eject(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (libUsbDrivesI > -1 && libUsbDrivesList[libUsbDrivesI]->lock == 0) {
    // Determine Drive State
    if (libUsbDrivesList[libUsbDrivesI]->partitionCur > -1 &&
        strcmp(libUsbDrivesList[libUsbDrivesI]->partitions[libUsbDrivesList[libUsbDrivesI]->partitionCur]->mountpoint, "null") == 0
    ) { // No Mount Point
      size_t cmdSz = snprintf(NULL, 0, "sudo -u pi /usr/bin/udisksctl mount --block-device /dev/%s --no-user-interaction", libUsbDrivesList[libUsbDrivesI]->partitions[libUsbDrivesList[libUsbDrivesI]->partitionCur]->name) + 1;
      char *cmd = (char *)malloc(cmdSz * sizeof(char));
      snprintf(cmd, cmdSz, "sudo -u pi /usr/bin/udisksctl mount --block-device /dev/%s --no-user-interaction", libUsbDrivesList[libUsbDrivesI]->partitions[libUsbDrivesList[libUsbDrivesI]->partitionCur]->name);
      system(cmd);
      free(cmd);

      // printf("MP %s\n", libUsbDrivesList[libUsbDrivesI]->partitions[libUsbDrivesList[libUsbDrivesI]->partitionCur]->mountpoint);
     
    } else if (libUsbDrivesList[libUsbDrivesI]->partitionCur > -1 && libUsbDrivesList[libUsbDrivesI]->lock == 0) {
      size_t cmdSz = snprintf(NULL, 0, "sudo -u pi /usr/bin/udisksctl unmount --block-device /dev/%s --no-user-interaction &", libUsbDrivesList[libUsbDrivesI]->partitions[libUsbDrivesList[libUsbDrivesI]->partitionCur]->name) + 1;
      char *cmd = (char *)malloc(cmdSz * sizeof(char));
      snprintf(cmd, cmdSz, "sudo -u pi /usr/bin/udisksctl unmount --block-device /dev/%s --no-user-interaction &", libUsbDrivesList[libUsbDrivesI]->partitions[libUsbDrivesList[libUsbDrivesI]->partitionCur]->name);
      system(cmd);
      free(cmd);

    } else {
      gslc_ElemSetTxtStr(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], (char*)" ");
    }

    pg_usbdrive_loadDrive(libUsbDrivesI);
  }

  return true;
}

bool pg_usbdrive_cbBtn_poweroff(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
  if (libUsbDrivesI > -1 && libUsbDrivesList[libUsbDrivesI] && libUsbDrivesList[libUsbDrivesI]->lock == 0) {
    dbgprintf(DBG_DEBUG, "Poweroff /dev/%s\n", libUsbDrivesList[libUsbDrivesI]->name);
    libUsbDrivesList[libUsbDrivesI]->lock = 1;
    
    // Find anything mounted
    for (int luI = 0; luI < libUsbDrivesList[libUsbDrivesI]->partitionMax; ++luI) {
      if (libUsbDrivesList[libUsbDrivesI]->partitions[luI] && libUsbDrivesList[libUsbDrivesI]->partitions[luI]->mountpoint &&
          strcmp(libUsbDrivesList[libUsbDrivesI]->partitions[luI]->mountpoint, "null") != 0
      ) {
        size_t cmdSz = snprintf(NULL, 0, "/usr/bin/udisksctl unmount --block-device /dev/%s --no-user-interaction", libUsbDrivesList[libUsbDrivesI]->partitions[luI]->name) + 1;
        char *cmd = (char *)malloc(cmdSz * sizeof(char));
        snprintf(cmd, cmdSz, "/usr/bin/udisksctl unmount --block-device /dev/%s --no-user-interaction", libUsbDrivesList[libUsbDrivesI]->partitions[luI]->name);
        system(cmd);
        free(cmd);
      }
    }

    // Poweroff Drive
    size_t cmdSz = snprintf(NULL, 0, "/usr/bin/udisksctl power-off --block-device /dev/%s --no-user-interaction &", libUsbDrivesList[libUsbDrivesI]->name) + 1;
    char *cmd = (char *)malloc(cmdSz * sizeof(char));
    snprintf(cmd, cmdSz, "/usr/bin/udisksctl power-off --block-device /dev/%s --no-user-interaction &", libUsbDrivesList[libUsbDrivesI]->name);
    system(cmd);
    free(cmd);

    touchscreenPageGoBack(pGui);
  }

  return true;
}



void pg_usbdriveGuiInit(gslc_tsGui *pGui) {
  int ePage = E_PG_USBDRIVE;

  gslc_PageAdd(pGui, E_PG_USBDRIVE, pg_usbdriveElem, MAX_ELEM_PG_USBDRIVE_RAM,
               pg_usbdriveElemRef, MAX_ELEM_PG_USBDRIVE);




  int xHei = 40;
  gslc_tsRect rListBox = {0,20,423,160};
  // Main View Box
  pg_usbdriveEl[E_USBDRIVE_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rListBox);
  gslc_ElemSetCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], &pg_usbDrive_cbDrawBox);

  // Button A
  pg_usbDrive_listConfig->refs[0] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (0 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_usbDrive_cbBtn_elA);
  gslc_ElemSetTxtCol(pGui, pg_usbDrive_listConfig->refs[0], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_usbDrive_listConfig->refs[0], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_usbDrive_listConfig->refs[0], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_usbDrive_listConfig->refs[0], false);
  gslc_ElemSetFrameEn(pGui, pg_usbDrive_listConfig->refs[0], true);

  // Button B
  pg_usbDrive_listConfig->refs[1] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (1 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_usbDrive_cbBtn_elB);
  gslc_ElemSetTxtCol(pGui, pg_usbDrive_listConfig->refs[1], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_usbDrive_listConfig->refs[1], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_usbDrive_listConfig->refs[1], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_usbDrive_listConfig->refs[1], false);
  gslc_ElemSetFrameEn(pGui, pg_usbDrive_listConfig->refs[1], true);

  // Button C
  pg_usbDrive_listConfig->refs[2] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (2 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_usbDrive_cbBtn_elC);
  gslc_ElemSetTxtCol(pGui, pg_usbDrive_listConfig->refs[2], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_usbDrive_listConfig->refs[2], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_usbDrive_listConfig->refs[2], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_usbDrive_listConfig->refs[2], false);
  gslc_ElemSetFrameEn(pGui, pg_usbDrive_listConfig->refs[2], true);

  // Button D
  pg_usbDrive_listConfig->refs[3] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (3 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_usbDrive_cbBtn_elD);
  gslc_ElemSetTxtCol(pGui, pg_usbDrive_listConfig->refs[3], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_usbDrive_listConfig->refs[3], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_usbDrive_listConfig->refs[3], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_usbDrive_listConfig->refs[3], false);
  gslc_ElemSetFrameEn(pGui, pg_usbDrive_listConfig->refs[3], true);

  //// Button E
  // pg_usbDrive_listConfig->refs[4] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
  //       (gslc_tsRect) {rListBox.x, (rListBox.y + (4 * xHei)), rListBox.w, xHei},
  //       (char*)" ", 0, E_FONT_MONO18, &pg_usbDrive_cbBtn_elE);
  // gslc_ElemSetTxtCol(pGui, pg_usbDrive_listConfig->refs[4], GSLC_COL_WHITE);
  // gslc_ElemSetTxtAlign(pGui, pg_usbDrive_listConfig->refs[4], GSLC_ALIGN_MID_LEFT);
  // gslc_ElemSetTxtMarginXY(pGui, pg_usbDrive_listConfig->refs[4], 10, 0);
  // gslc_ElemSetFillEn(pGui, pg_usbDrive_listConfig->refs[4], false);
  // gslc_ElemSetFrameEn(pGui, pg_usbDrive_listConfig->refs[4], true);


  //////////////////////////////////////////
  // Create vertical scrollbar
  pg_usbDrive_listConfig->sliderEl = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO,
      ePage, &pg_usbDrive_listSlider, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, rListBox.y + 55, rFullscreen.w - (rListBox.x + rListBox.w) - 2, rListBox.h - 110},
      0, pg_usbDrive_listConfig->scrollMax, 0, 10, true);
  pg_usbDrive_listConfig->slider = &pg_usbDrive_listSlider; // Assign to listConfig for later access

  gslc_ElemSetCol(pGui, pg_usbDrive_listConfig->sliderEl, GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pGui, pg_usbDrive_listConfig->sliderEl, true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(pGui, pg_usbDrive_listConfig->sliderEl, &pg_usbDrive_cbBtn_sliderPos);



  //////////////////////////////////////////
  // Create vertical scrollbar Up Arrow
  pg_usbDrive_listConfig->sliderUpEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, rListBox.y, rFullscreen.w - (rListBox.x + rListBox.w) - 2, 50}, (char*)"^",
      0, E_FONT_MONO18, &pg_usbDrive_cbBtn_sliderUp);
  gslc_ElemSetTxtCol(pGui, pg_usbDrive_listConfig->sliderUpEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_usbDrive_listConfig->sliderUpEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_usbDrive_listConfig->sliderUpEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_usbDrive_listConfig->sliderUpEl, GSLC_ALIGN_MID_MID);


  //////////////////////////////////////////
  // Create vertical scrollbar Down Arrow
  pg_usbDrive_listConfig->sliderDownEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, (rListBox.y + rListBox.h) - 50, rFullscreen.w - (rListBox.x + rListBox.w) - 2, 50}, (char*)"v",
      0, E_FONT_MONO18, &pg_usbDrive_cbBtn_sliderDown);
  gslc_ElemSetTxtCol(pGui, pg_usbDrive_listConfig->sliderDownEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_usbDrive_listConfig->sliderDownEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_usbDrive_listConfig->sliderDownEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_usbDrive_listConfig->sliderDownEl, GSLC_ALIGN_MID_MID);
  pg_usbDrive_listConfig->sliderDownEl = pg_usbDrive_listConfig->sliderDownEl;





  // Eject Key
  pg_usbdriveEl[E_USBDRIVE_EL_EJECT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {0,210,110,110},
          (char*)" ", 0, E_FONT_MONO14, &pg_usbdrive_cbBtn_eject);
  gslc_ElemSetTxtCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], false);
  gslc_ElemSetGlowEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], false);
  gslc_ElemSetFrameEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], true);

/*
  // Power Off Key
  pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {123,210,110,110},
          (char*)"Power Off", 0, E_FONT_MONO14, &pg_usbdrive_cbBtn_poweroff);
  gslc_ElemSetTxtCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], false);
  gslc_ElemSetGlowEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], false);
  gslc_ElemSetFrameEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], true);
*/

  // Open Key
  pg_usbdriveEl[E_USBDRIVE_EL_ACTION] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {247,210,110,110},
          (char*)" ", 0, E_FONT_MONO14, &pg_usbdrive_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_ACTION], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_ACTION], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_usbdriveEl[E_USBDRIVE_EL_ACTION], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_ACTION], false);
  gslc_ElemSetGlowEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_ACTION], false);
  gslc_ElemSetFrameEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_ACTION], true);
  //glsc_ElemSetVisible(pGui, pg_usbdriveEl[E_USBDRIVE_EL_ACTION], false);


  // Close Key
  pg_usbdriveEl[E_USBDRIVE_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {370,210,110,110},
          (char*)"Close", 0, E_FONT_MONO14, &pg_usbdrive_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_usbdriveEl[E_USBDRIVE_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_CLOSE], false);
  gslc_ElemSetGlowEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_CLOSE], true);

}

void pg_usbdriveButtonRotaryCW() {

}

void pg_usbdriveButtonRotaryCCW() {

}


void pg_usbdriveButtonLeftPressed() {

}

void pg_usbdriveButtonRightPressed() {

}

void pg_usbdriveButtonRotaryPressed() {

}

void pg_usbdriveButtonLeftHeld() {

}

void pg_usbdriveButtonRightHeld() {

}


void pg_usbdriveButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_usbdriveButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_usbdriveButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_usbdriveButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_usbdriveButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_usbdriveButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_usbdriveButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_usbdriveButtonRightHeld);
}

void pg_usbdriveButtonUnsetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, NULL);
}


// GUI Init
void pg_usbdrive_init(gslc_tsGui *pGui) {
  pg_usbDrive_listConfig = VLIST_INIT_CONFIG(4, 32);
  pg_usbdriveGuiInit(pGui);

  cbInit[E_PG_USBDRIVE] = NULL;
}


// GUI Open
void pg_usbdrive_open(gslc_tsGui *pGui) {

  pg_usbdriveButtonSetFuncs();
}


void pg_usbdrive_close(gslc_tsGui *pGui) {

}



int pg_usbdrive_thread(gslc_tsGui *pGui) {
  if (libUsbDrivesI > -1 && pg_usbdrive_thread_usbCnt != libUsbDrivesCount->cnt) {
    if (libUsbDrivesList[libUsbDrivesI]->lock == 1) { return 0; }

    // Determine Drive State
    if (libUsbDrivesList[libUsbDrivesI]->partitionCur > -1 &&
        strcmp(libUsbDrivesList[libUsbDrivesI]->partitions[libUsbDrivesList[libUsbDrivesI]->partitionCur]->mountpoint, "null") == 0
    ) { // No Mount Point
      gslc_ElemSetTxtStr(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], (char*)"Mount");
    } else if (libUsbDrivesList[libUsbDrivesI]->partitionCur > -1) {
      gslc_ElemSetTxtStr(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], (char*)"Un Mount");
    } else {
      gslc_ElemSetTxtStr(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], (char*)" ");
    }


    // Generate list of items based on default list info
    char **list = (char**)malloc(pg_usbDrive_listConfig->len * sizeof(char*));
    int listI = 0;
    for (int l = 0; l < pg_usbDrive_listConfig->len; ++l) {
      if (l > libUsbDrivesList[libUsbDrivesI]->partitionMax) { break; }
      size_t nameSz = snprintf(NULL, 0, "(%s) %s [%s]", libUsbDrivesList[libUsbDrivesI]->partitions[l]->drivesize, libUsbDrivesList[libUsbDrivesI]->partitions[l]->label, libUsbDrivesList[libUsbDrivesI]->partitions[l]->mountpoint) + 1;
      list[l] = (char *)malloc(nameSz * sizeof(char));
      snprintf(list[l], nameSz, "(%s) %s [%s]", libUsbDrivesList[libUsbDrivesI]->partitions[l]->drivesize, libUsbDrivesList[libUsbDrivesI]->partitions[l]->label, libUsbDrivesList[libUsbDrivesI]->partitions[l]->mountpoint);
      listI++;
    }

    // Use new List
    vlist_sliderDraw(pGui, pg_usbDrive_listConfig, list, 29);

    // Clean list
    for (int l = 0; l < listI; ++l) {
      free(list[l]);
    }
    free(list);

    pg_usbdrive_thread_usbCnt = libUsbDrivesCount->cnt;
    gslc_ElemSetRedraw(pGui, pg_usbdriveEl[E_USBDRIVE_EL_BOX], GSLC_REDRAW_FULL);
  }
  return 0;
}

// GUI Destroy
void pg_usbdrive_destroy(gslc_tsGui *pGui) {

}

void __attribute__ ((constructor)) pg_usbdrive_constructor(void) {
  cbInit[E_PG_USBDRIVE] = &pg_usbdrive_init;
  cbOpen[E_PG_USBDRIVE] = &pg_usbdrive_open;
  cbClose[E_PG_USBDRIVE] = &pg_usbdrive_close;
  cbThread[E_PG_USBDRIVE] = &pg_usbdrive_thread;
  cbDestroy[E_PG_USBDRIVE] = &pg_usbdrive_destroy;
  // pg_usbdriveQueue = ALLOC_QUEUE_ROOT();
}

void __attribute__ ((destructor)) pg_usbdrive_destructor(void) {

}

