#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "shared.h"
#include "system.h"
#include "buttons/buttons.h"
#include "gui/pages.h"
#include "gui/keyboard/keyboard.h"


//////////////////////////////////
//
// GUI CALLBACK BUTTONS
//
bool pg_system_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
  touchscreenPageClose(pGui, E_PG_SYSTEM);
  touchscreenPageOpen(pGui, E_PG_MAIN);
  return true;
}

bool pg_system_cbBtn_wifi(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
  touchscreenPageOpen(pGui, E_PG_WIFI);
  return true;
}

bool pg_system_cbBtn_powercycleHdmi(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
  gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], true);
  gslc_Update(pGui);
  system("/usr/bin/vcgencmd display_power 0");
  usleep(2000000);
  system("/usr/bin/vcgencmd display_power 1");
  gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], false);
  
  return true;
}

bool pg_system_cbBtn_reboot(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  
  m_bQuit = 1;
  system("sudo /sbin/shutdown -r now &");

  return true;
}

bool pg_system_cbBtn_update(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  

  return true;
}

/////////////////////////////
//
// GUI DISPLAY FUNCTIONS

int pg_system_guiInit(gslc_tsGui *pGui)
{
  // debug_print("%s\n", "Wifi GUI Init");
  int ePage = E_PG_SYSTEM;
  gslc_PageAdd(pGui, ePage, pg_systemElem, MAX_ELEM_PG_SYSTEM_RAM, pg_systemElemRef, MAX_ELEM_PG_SYSTEM);

  // Close Key
  pg_systemEl[E_SYSTEM_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 100),(rFullscreen.y + 5),100,50},
          "Close", 0, E_FONT_MONO14, &pg_system_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], true);

  // Reboot device
  if ((
    pg_systemEl[E_SYSTEM_EL_REBOOT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 100), ((rFullscreen.y + rFullscreen.h) - 50), 100, 50},
            "Reboot", 0, E_FONT_MONO14, &pg_system_cbBtn_reboot)
  ) != NULL) {            
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], GSLC_COL_WHITE, GSLC_COL_RED, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], true); 
  }
  
  // Wifi Settings
  if ((
    pg_systemEl[E_SYSTEM_EL_WIFI] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 60, 100, 50},
            "Wifi", 0, E_FONT_MONO14, &pg_system_cbBtn_wifi)
  ) != NULL) {            
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], true); 
  }
  
  // Powercycle HDMI Port
  if ((
    pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 160, 150, 50},
            "Powercycle HDMI", 0, E_FONT_MONO14, &pg_system_cbBtn_powercycleHdmi)
  ) != NULL) {            
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], GSLC_COL_WHITE, GSLC_COL_RED, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], true); 
  }
  
  return 1;
}

void pg_system_btnDoubleHeld() {
  touchscreenPageClose(&m_gui, E_PG_SYSTEM);
  touchscreenPageOpen(&m_gui, E_PG_MAIN);
}

void pg_system_btnSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, &pg_system_btnDoubleHeld);
}

void pg_system_btnUnsetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, NULL);
}

// GUI Init
void pg_system_init(gslc_tsGui *pGui) {
  // Create Interface
  pg_system_guiInit(pGui);

  cbInit[E_PG_SYSTEM] = NULL;
}


// GUI Open
void pg_system_open(gslc_tsGui *pGui) {
  pg_system_btnSetFuncs();
}

void pg_system_close(gslc_tsGui *pGui) {
  pg_system_btnUnsetFuncs();
}

// GUI Destroy
void pg_system_destroy() {

}

void __attribute__ ((constructor)) pg_system_setup(void) {
  cbInit[E_PG_SYSTEM] = &pg_system_init;
  cbOpen[E_PG_SYSTEM] = &pg_system_open;
  cbClose[E_PG_SYSTEM] = &pg_system_close;
  cbDestroy[E_PG_SYSTEM] = &pg_system_destroy;
}
