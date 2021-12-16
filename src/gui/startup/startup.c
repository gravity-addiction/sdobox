#include <limits.h>
#include <fcntl.h>

#include "libs/buttons/buttons.h"

#include "gui/pages.h"
#include "gui/keyboard/keyboard.h"
#include "startup.h"

#include "gui/wifi/lib_wifi_wpa.h"

char pg_startupIpboxBuf[PG_STARTUP_IPBOX_ROWS * PG_STARTUP_IPBOX_COLS];

/////////////////////
// Function calls
void updateIpAddress(gslc_tsGui *pGui) {
  FILE* input = popen("ip address list | grep inet | grep -v 127.0.0 | cut -d \" \" -f 6 | cut -d \"/\" -f 1", "r");
  char result[PG_STARTUP_IPBOX_ROWS * PG_STARTUP_IPBOX_COLS];
  char* got;
  gslc_ElemXTextboxReset(pGui, pg_startupEl[E_STARTUP_EL_IPBOX]);
  pg_startupIpBoxLines = 0;
  while((got = fgets(result, sizeof(result) - 1, input)) != NULL) {
    if (got && strlen(got) > 0) {
      pg_startupIpBoxLines++;
      gslc_ElemXTextboxAdd(pGui, pg_startupEl[E_STARTUP_EL_IPBOX], result);
    }
  }
  fclose(input);
}

void updateHostname(gslc_tsGui *pGui) {
  FILE* input = popen("hostname --fqd", "r");
  char result[HOST_NAME_MAX + 1];
  char* got;

  while((got = fgets(result, sizeof(result) - 1, input)) != NULL) {
    if (got && strlen(got) > (HOST_NAME_MAX + 1)) {
      strlcpy(pg_startupHostname, result, HOST_NAME_MAX + 1);
    } else if (got && strlen(got) > 0) {
      strlcpy(pg_startupHostname, result, strlen(got));
    } else {
      CLEAR(pg_startupHostname, HOST_NAME_MAX + 1);
    }
  }
  fclose(input);
  gslc_ElemSetTxtStr(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT], pg_startupHostname);
}

void calibrateTouchscreen() {
  system("/opt/sdobox/scripts/calibrate");
}

void pg_startup_updateIpBoxScroll(gslc_tsGui *pGui) {
  int winRows = 4;
  int winMax = (PG_STARTUP_IPBOX_ROWS - winRows);

  if (pg_startupIpBoxScroll < 0) {
    pg_startupIpBoxScroll = pg_startupIpBoxLines - 1;
  }
  if (pg_startupIpBoxScroll > winMax || pg_startupIpBoxScroll > pg_startupIpBoxLines) {
    pg_startupIpBoxScroll = 0;
  }

  if (pg_startupIpBoxScroll == 0) {
    gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
    gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLUE);
  } else if (pg_startupIpBoxScroll == winMax) {
    gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLUE);
    gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  } else {
    gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLUE);
    gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLUE);
  }
  gslc_ElemXTextboxScrollSet(pGui, pg_startupEl[E_STARTUP_EL_IPBOX], pg_startupIpBoxScroll, winMax);

}

///////////////////////
// Keyboard Callback
void pg_startupCbBtnKeyboard_Callback(gslc_tsGui *pGui, char* str) {

}

void pg_startupCbBtnHostname_Callback(gslc_tsGui *pGui, char* str) {
  if (str && strlen(str) && strcmp(pg_startupHostname, str) != 0) {
    size_t hostCmdSz = snprintf(NULL, 0, "/opt/sdobox/scripts/hostname \"%s\" \"%s\"", str, pg_startupHostname) + 1;
    char *hostCmd = (char*)malloc(hostCmdSz * sizeof(char));
    snprintf(hostCmd, hostCmdSz, "/opt/sdobox/scripts/hostname \"%s\" \"%s\"", str, pg_startupHostname);
    // printf("Cmd: %s\n", hostCmd);
    system(hostCmd);
    free(hostCmd);
    updateHostname(pGui);
  }
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

bool pg_startup_cbBtn_menu(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_MAIN);
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
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_startupIpBoxScroll--;
  pg_startup_updateIpBoxScroll(pGui);

  return true;
}

bool pg_startup_cbBtn_ipBoxDown(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_startupIpBoxScroll++;
  pg_startup_updateIpBoxScroll(pGui);

  return true;
}

bool pg_startupCbBtnKeyboard(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Show Keyboard
  pg_keyboard_show(pGui, 16, "", &pg_startupCbBtnKeyboard_Callback);
  return true;
}

bool pg_startup_cbBtn_ipRefresh(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  updateIpAddress(pGui);
  updateHostname(pGui);

  pg_startupIpBoxScroll = 0;
  pg_startup_updateIpBoxScroll(pGui);
  return true;
}


bool pg_startup_cbBtn_hostname(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
   gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_keyboard_show(pGui, HOST_NAME_MAX, pg_startupHostname, &pg_startupCbBtnHostname_Callback);
  return true;
}

bool pg_startup_cbBtn_system(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_SYSTEM);
  return true;
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

  pg_startupEl[E_STARTUP_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_BOX], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);

  // IP Address Txt
  pg_startupEl[E_STARTUP_EL_IP_ADDRESS] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x), (rFullscreen.y + 5), 190, (15 + yPadding)},
          (char*)"IP Addresses: ", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], GSLC_COL_GRAY_LT2);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], xPadding, yPadding);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], false);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], false);
  // gslc_ElemSetVisible(pGui, pg_startupEl[E_STARTUP_EL_IP_ADDRESS], false);

  // IP Address Refresh Btn
  pg_startupEl[E_STARTUP_EL_IP_REFRESH] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x + 200), (rFullscreen.y + 5), 160, (15 + yPadding)},
          (char*)"Refresh", 0, E_FONT_MONO18, &pg_startup_cbBtn_ipRefresh);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_IP_REFRESH], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IP_REFRESH], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_IP_REFRESH], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_IP_REFRESH], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_IP_REFRESH], true);


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
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_UP], true);

  pg_startupEl[E_STARTUP_EL_IPBOX_DOWN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.w - 45), (rFullscreen.y + yPadding + 80), 40, 40},
          (char*)"v", 0, E_FONT_MONO24, &pg_startup_cbBtn_ipBoxDown);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_IPBOX_DOWN], true);




  // Hostname Btn
  pg_startupEl[E_STARTUP_EL_HOSTNAME] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x), (rFullscreen.y + 140), 100, (15 + yPadding)},
          (char*)"Hostname: ", 0, E_FONT_MONO18, &pg_startup_cbBtn_hostname);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME], xPadding, yPadding);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME], false);
  // gslc_ElemSetVisible(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME], false);

    // Hostname Txt
  pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x + 110), (rFullscreen.y + 140), (rFullscreen.w - (rFullscreen.x + 110)), (15 + yPadding)},
          (char*)" ", 0, E_FONT_MONO18, &pg_startup_cbBtn_hostname);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT], xPadding, yPadding);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT], false);
  // gslc_ElemSetVisible(pGui, pg_startupEl[E_STARTUP_EL_HOSTNAME_TXT], false);




/*
  pg_startupEl[E_STARTUP_EL_BTN_CONFIG] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {10, 200, 180, 60},
          (char*)"Button Config", 0, E_FONT_MONO18, &pg_startup_cbBtn_btnConfig);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_CONFIG], true);
*/

  pg_startupEl[E_STARTUP_EL_BTN_WIFI] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {120, 200, 100, 60},
          (char*)"Wifi", 0, E_FONT_MONO18, &pg_startup_cbBtn_wifi);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_WIFI], true);

  pg_startupEl[E_STARTUP_EL_BTN_MENU] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {10, 200, 100, 60},
          (char*)"Menu", 0, E_FONT_MONO18, &pg_startup_cbBtn_menu);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_MENU], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_MENU], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_BTN_MENU], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_MENU], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_MENU], true);

  pg_startupEl[E_STARTUP_EL_BTN_SYSTEM] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {230, 200, 100, 60},
          (char*)"System", 0, E_FONT_MONO18, &pg_startup_cbBtn_system);
  gslc_ElemSetTxtCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_SYSTEM], GSLC_COL_GRAY_LT2);
  gslc_ElemSetCol(pGui, pg_startupEl[E_STARTUP_EL_BTN_SYSTEM], GSLC_COL_GRAY_LT2, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, pg_startupEl[E_STARTUP_EL_BTN_SYSTEM], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_SYSTEM], true);
  gslc_ElemSetFrameEn(pGui, pg_startupEl[E_STARTUP_EL_BTN_SYSTEM], true);

}


int pg_startupButtonRotaryCW() {
  return 0;
}
int pg_startupButtonRotaryCCW() {
  return 0;
}
int pg_startupButtonLeftPressed() {
  return 0;
}
int pg_startupButtonRightPressed() {
  return 0;
}
int pg_startupButtonRotaryPressed() {
  return 0;
}
int pg_startupButtonLeftHeld() {
  return 0;
}
int pg_startupButtonRightHeld() {
  return 0;
}
int pg_startupButtonRotaryHeld() {
  guislice_wrapper_mirror_toggle(&m_gui);
  return 1;
}
int pg_startupButtonDoubleHeld() {
  touchscreenPageGoBack(&m_gui);
  return 1;
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

  pg_startupIpBoxLines = 0;
  pg_startupIpBoxScroll = 0;
  pg_startupHostname = (char*)malloc(HOST_NAME_MAX * sizeof(char));


  pg_startup_updateIpBoxScroll(pGui);
  updateIpAddress(pGui);
  updateHostname(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_STARTUP] = NULL;
}


// GUI Open
void pg_startup_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_startupButtonSetFuncs();

}


// GUI Close
void pg_startup_close(gslc_tsGui *pGui) {

}


// GUI Destroy
void pg_startup_destroy(gslc_tsGui *pGui) {
  free(pg_startupHostname);
}

// Setup Constructor
void __attribute__ ((constructor)) pg_startup_setup(void) {
  cbInit[E_PG_STARTUP] = &pg_startup_init;
  cbOpen[E_PG_STARTUP] = &pg_startup_open;
  cbClose[E_PG_STARTUP] = &pg_startup_close;
  cbDestroy[E_PG_STARTUP] = &pg_startup_destroy;
}
