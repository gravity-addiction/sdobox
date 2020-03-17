#include "buttons/buttons.h"

#include "gui/pages.h"
#include "gui/keyboard/keyboard.h"
#include "startup.h"

#include "gui/wifi/lib_wifi_wpa.h"

char pg_startupIpboxBuf[PG_STARTUP_IPBOX_ROWS * PG_STARTUP_IPBOX_COLS];

///////////////////////
// Keyboard Callback
void pg_startupCbBtnKeyboard_Callback(gslc_tsGui *pGui, char* str) {

}


////////////////
// Button Callback


bool pg_startup_cbBtn(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  return true;
}

bool pg_startup_cbBtn_wifi(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_WIFI);
  return true;
}

bool pg_startup_cbBtn_btnConfig(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  gslc_ElemSetTxtStr(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], "Left Button");
  return true;
}

bool pg_startup_cbBtn_ipBoxUp(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  return true;
}

bool pg_startup_cbBtn_ipBoxDown(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  return true;
}

bool pg_startupCbBtnKeyboard(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Show Keyboard
  pg_keyboard_show(pGui, 16, "", &pg_startupCbBtnKeyboard_Callback);
  return true;
}





/////////////////////
// Function calls
void updateIpAddress(gslc_tsGui *pGui) {
  FILE* input = popen("ip address list | grep inet | grep -v 127.0.0 | cut -d \" \" -f 6 | cut -d \"/\" -f 1", "r");
  char result[PG_STARTUP_IPBOX_ROWS * PG_STARTUP_IPBOX_COLS];
  char* got;
  gslc_ElemXTextboxReset(pGui, pg_startupEl[E_STARTUP_EL_IPBOX]);
  while((got = fgets(result, sizeof(result), input)) != NULL) {
    if (got && strlen(got) > 0) {
      gslc_ElemXTextboxAdd(pGui, pg_startupEl[E_STARTUP_EL_IPBOX], result);
    }
  }
  fclose(input);
}



/////////////////////
// Init Gui Elements
void pg_startupGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_STARTUP;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgStartupElem, MAX_ELEM_PG_STARTUP, m_asPgStartupElemRef, MAX_ELEM_PG_STARTUP);

  int yPadding = 10;
  int xPadding = 10;

  // IP Address Txt
  pg_startupEl[E_STARTUP_EL_IP_ADDRESS] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x), (rFullscreen.y + 5), rFullscreen.w, (15 + yPadding)},
          (char*)"IP Addresses: ", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], GSLC_COL_GRAY_LT2);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], xPadding, yPadding);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], false);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], false);
  // gslc_ElemSetVisible(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], false);

  // Create textbox
  pg_startupEl[E_STARTUP_EL_IPBOX] = gslc_ElemXTextboxCreate(pGui, GSLC_ID_AUTO, ePage,
          &pg_startupIpboxTextbox,
          (gslc_tsRect) {(rFullscreen.x + xPadding), (rFullscreen.y + yPadding + 20), (rFullscreen.w - 60), 100},
          E_FONT_MONO18, (char*)&pg_startupIpboxBuf, PG_STARTUP_IPBOX_ROWS, PG_STARTUP_IPBOX_COLS);
  gslc_ElemXTextboxWrapSet(pGui, pg_startupEl[E_STARTUP_EL_IPBOX], true);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX], GSLC_COL_GRAY_LT2, GSLC_COL_GRAY_DK3, GSLC_COL_GRAY_LT2);


  pg_startupEl[E_STARTUP_EL_IPBOX_UP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.w - 45), (rFullscreen.y + yPadding + 20), 40, 40},
          (char*)"^", 0, E_FONT_MONO24, &pg_startup_cbBtn_ipBoxUp);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], true);

  pg_startupEl[E_STARTUP_EL_IPBOX_DOWN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.w - 45), (rFullscreen.y + yPadding + 80), 40, 40},
          (char*)"v", 0, E_FONT_MONO24, &pg_startup_cbBtn_ipBoxDown);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], true);

  pg_startupEl[E_STARTUP_EL_BTN_CONFIG] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {10, 200, 180, 60},
          (char*)"Button Config", 0, E_FONT_MONO18, &pg_startup_cbBtn_btnConfig);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], true);

  pg_startupEl[E_STARTUP_EL_BTN_WIFI] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {200, 200, 100, 60},
          (char*)"Wifi", 0, E_FONT_MONO18, &pg_startup_cbBtn_wifi);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], true);

}


void pg_startupButtonRotaryCW() {

}
void pg_startupButtonRotaryCCW() {

}
void pg_startupButtonLeftPressed() {

}
void pg_startupButtonRightPressed() {

}
void pg_startupButtonRotaryPressed() {

}
void pg_startupButtonLeftHeld() {

}
void pg_startupButtonRightHeld() {

}
void pg_startupButtonRotaryHeld() {

}
void pg_startupButtonDoubleHeld() {

}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_startupButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_startupButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_startupButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_startupButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_startupButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_startupButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_startupButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_startupButtonRightHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, &pg_startupButtonRotaryHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, &pg_startupButtonDoubleHeld);
}


// GUI Init
void pg_startup_init(gslc_tsGui *pGui) {
  pg_startupGuiInit(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_STARTUP] = NULL;
}


// GUI Open
void pg_startup_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_startupButtonSetFuncs();

  updateIpAddress(pGui);
}


// GUI Close
void pg_startup_close(gslc_tsGui *pGui) {

}


// GUI Destroy
void pg_startup_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_startup_setup(void) {
  cbInit[E_PG_STARTUP] = &pg_startup_init;
  cbOpen[E_PG_STARTUP] = &pg_startup_open;
  cbClose[E_PG_STARTUP] = &pg_startup_close;
  cbDestroy[E_PG_STARTUP] = &pg_startup_destroy;
}