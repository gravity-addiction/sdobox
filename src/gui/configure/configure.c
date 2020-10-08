#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "configure.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "libs/audio/audio.h"
#include "libs/buttons/buttons.h"

#include "gui/pages.h"




////////////////
// Button Callback
bool pg_configure_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageGoBack(pGui);
  return true;
}



//////////////////
// Box Drawing
bool pg_configure_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}




/////////////////////
// Init Gui Elements
void pg_configureGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_CONFIGURE;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgConfigureElem, MAX_ELEM_PG_CONFIGURE, m_asPgConfigureElemRef, MAX_ELEM_PG_CONFIGURE);

  // Configure View Box
  pg_configureEl[E_CONFIGURE_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, (gslc_tsRect){0, 0, 480, 320});
  gslc_ElemSetCol(pGui, pg_configureEl[E_CONFIGURE_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_configureEl[E_CONFIGURE_EL_BOX], &pg_configure_cbDrawBox);

  /////////////////////
  // Page Defined Elements

  // Message Lines
  if ((
    pg_configureEl[E_CONFIGURE_EL_MSG] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){0, ((rFullscreen.h / 2) - 50), rFullscreen.w, 100},
          (char*)" ", 0, E_FONT_MONO28)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], false);
    gslc_ElemSetFrameEn(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], false);
  }

  if ((
    pg_configureEl[E_CONFIGURE_EL_MSGA] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){0, ((rFullscreen.h / 2) - 110), rFullscreen.w, 100},
          (char*)" ", 0, E_FONT_MONO28)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], false);
    gslc_ElemSetFrameEn(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], false);
  }

  if ((
    pg_configureEl[E_CONFIGURE_EL_MSGB] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){0, ((rFullscreen.h / 2) + 10), rFullscreen.w, 100},
          (char*)" ", 0, E_FONT_MONO28)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], false);
    gslc_ElemSetFrameEn(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], false);
  }
}


// GUI Init
void pg_configure_init(gslc_tsGui *pGui) {
  pg_configureGuiInit(pGui);


  // Cleanup so Init is only ran once
  cbInit[E_PG_CONFIGURE] = NULL;
}



// GUI Open
void pg_configure_open(gslc_tsGui *pGui) {
  system("/opt/sdobox/scripts/calibrate");
  
  if (wiringPiSetup () == -1) {
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], "");
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], "Cannot Initialize Wiring Pi");
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], "");
    dbgprintf(DBG_DEBUG, "Failed Initializing Pi Wiring\n");
  }

  int leftBtn, rightBtn, rotaryBtn, rotaryA, rotaryB = -1;

  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], "");
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], "Press Left Button");
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], "");
  gslc_Update(pGui);

  dbgprintf(DBG_DEBUG, "Press Left Button\n");
  leftBtn = lib_buttons_findGPIO(10000, 100);
  dbgprintf(DBG_DEBUG, "Left Button Found! %d\n", leftBtn);
  
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], "");
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], "Press Right Button");
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], "");
  gslc_Update(pGui);
  dbgprintf(DBG_DEBUG, "Press Right Button\n");
  lib_buttons_waitRelease(leftBtn);
  rightBtn = lib_buttons_findGPIO(10000, 100);
  dbgprintf(DBG_DEBUG, "Right Button Found! %d\n", rightBtn);

  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], "Press Directly Down");
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], "On Rotary Button");
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], " ");
  gslc_Update(pGui);
  dbgprintf(DBG_DEBUG, "Press Directly Down On Rotary Button\n");
  lib_buttons_waitRelease(rightBtn);
  rotaryBtn = lib_buttons_findGPIO(10000, 100);
  dbgprintf(DBG_DEBUG, "Rotary Button Found! %d\n", rotaryBtn);

  
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], "Spin Rotary Knob");
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], "Clockwise Until Detected");
  gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], "");
  gslc_Update(pGui);
  dbgprintf(DBG_DEBUG, "Spin Rotary Knob Clockwise Until Detected\n");
  lib_buttons_waitRelease(rotaryBtn);
  rotaryA = lib_buttons_findGPIO(10000, 50);
  rotaryB = lib_buttons_findGPIO(10000, 50);

  int i_now = millis();
  int ii_now = millis();
  int i_diff = 0;
  while (rotaryA == rotaryB) {
    rotaryB = lib_buttons_findGPIO(10000, 50);
    // Check for Timeout
    ii_now = millis();
    i_diff = ii_now - i_now;
    if ( i_diff > 20000) { 
      rotaryB = -1;
    }
  }
  if (rotaryB == -1) {
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], "");
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], "Rotary Knob NOT Found");
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], "20second Timeout!");
    gslc_Update(pGui);
    dbgprintf(DBG_DEBUG, "Rotary Knob NOT Found, 20second Timeout!\n");
  } else {
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGA], "");
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSG], "All Buttons Found!");
    gslc_ElemSetTxtStr(pGui, pg_configureEl[E_CONFIGURE_EL_MSGB], "");
    gslc_Update(pGui);
    dbgprintf(DBG_DEBUG, "Rotary Knob Found! %d %d\n", rotaryA, rotaryB);
  }

  mkdir("/home/pi/.config/sdobox", 0775);
  lib_buttons_saveConfig("/home/pi/.config/sdobox/sdobox.conf", leftBtn, rightBtn, rotaryBtn, rotaryA, rotaryB);

}


// GUI Destroy
void pg_configure_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_configure_setup(void) {
  cbInit[E_PG_CONFIGURE] = &pg_configure_init;
  cbOpen[E_PG_CONFIGURE] = &pg_configure_open;
  cbDestroy[E_PG_CONFIGURE] = &pg_configure_destroy;
}
