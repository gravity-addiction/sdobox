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

void pg_usbdrive_loadDrive(struct usbDriveData *driveData) {
  printf("Drive Block: %d %s\n", driveData->partitionI, driveData->drives->partitions[driveData->partitionI]->name);
  usbDriveDataset = driveData;
}

bool pg_usbdrive_cbBtn_close(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageGoBack(pGui);
  return true;
}

bool pg_usbdrive_cbBtn_eject(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  printf("Eject %s\n", usbDriveDataset->drives->partitions[usbDriveDataset->partitionI]->name);
  return true;
}

bool pg_usbdrive_cbBtn_poweroff(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  printf("Poweroff %s\n", usbDriveDataset->drives->partitions[usbDriveDataset->partitionI]->name);
  return true;
}



void pg_usbdriveGuiInit(gslc_tsGui *pGui) {
  int ePage = E_PG_USBDRIVE;

  gslc_PageAdd(pGui, E_PG_USBDRIVE, pg_usbdriveElem, MAX_ELEM_PG_USBDRIVE_RAM,
               pg_usbdriveElemRef, MAX_ELEM_PG_USBDRIVE);



  // Eject Key
  pg_usbdriveEl[E_USBDRIVE_EL_EJECT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {0,210,110,110},
          "Eject", 0, E_FONT_MONO14, &pg_usbdrive_cbBtn_eject);
  gslc_ElemSetTxtCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], false);
  gslc_ElemSetGlowEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], false);
  gslc_ElemSetFrameEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_EJECT], true);

  // Power Off Key
  pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {130,210,110,110},
          "Power Off", 0, E_FONT_MONO14, &pg_usbdrive_cbBtn_poweroff);
  gslc_ElemSetTxtCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], false);
  gslc_ElemSetGlowEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], false);
  gslc_ElemSetFrameEn(pGui, pg_usbdriveEl[E_USBDRIVE_EL_POWEROFF], true);



  // Close Key
  pg_usbdriveEl[E_USBDRIVE_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {370,210,110,110},
          "Close", 0, E_FONT_MONO14, &pg_usbdrive_cbBtn_close);
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
  
  return 0;
}

// GUI Destroy
void pg_usbdrive_destroy(gslc_tsGui *pGui) {
  if (usbDriveDataset != NULL) { free(usbDriveDataset); }
}

void __attribute__ ((constructor)) pg_usbdrive_constructor(void) {
  cbInit[E_PG_USBDRIVE] = &pg_usbdrive_init;
  cbOpen[E_PG_USBDRIVE] = &pg_usbdrive_open;
  cbClose[E_PG_USBDRIVE] = &pg_usbdrive_close;
  // cbThread[E_PG_USBDRIVE] = &pg_usbdrive_thread;
  cbDestroy[E_PG_USBDRIVE] = &pg_usbdrive_destroy;
  // pg_usbdriveQueue = ALLOC_QUEUE_ROOT();
}

void __attribute__ ((destructor)) pg_usbdrive_destructor(void) {

}

