#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "libs/shared.h"
#include "system.h"
#include "libs/buttons/buttons.h"
#include "gui/pages.h"
#include "gui/keyboard/keyboard.h"


//////////////////////////////////
//
// GUI CALLBACK BUTTONS
//
bool pg_system_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageGoBack(pGui);
  return true;
}

bool pg_system_cbBtn_wifi(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // touchscreenPageOpen(pGui, E_PG_WIFI);
  return true;
}

bool pg_system_cbBtn_powercycleHdmi(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], true);
  /***gslc_Update(pGui);*/
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

bool pg_system_cbBtn_exit(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  m_bQuit = 1;

  return true;
}

bool pg_system_cbBtn_upgrade(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_system_apt_upgrade == 1) { return true; }
  pg_system_apt_upgrade = 1;
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  system("/opt/sdobox/scripts/update-sdob.sh &");
  gslc_ElemSetTxtStr(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], "Upgrading ...");
/*
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  FILE *fd;
  fd = popen("sudo apt -qy update && sudo apt -qy -o \"Dpkg::Options::=--force-confdef\" -o \"Dpkg::Options::=--force-confold\" upgrade", "r");
  if (!fd) return 1;

  char buffer[64];
  size_t chread;
  int dots = 0;

  gslc_ElemSetTxtStr(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], "Upgrading");
  gslc_Update(pGui);
  // Use fread so binary data is dealt with correctly
  while ((chread = fread(buffer, 1, sizeof(buffer), fd)) != 0) {
    if (dots == 0) {
      gslc_ElemSetTxtStr(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], "Upgrading .");
    } else if (dots == 1) {
      gslc_ElemSetTxtStr(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], "Upgrading ..");
    } else if (dots == 2) {
      gslc_ElemSetTxtStr(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], "Upgrading ...");
      dots = -1;
    }
    dots++;
    gslc_Update(pGui);
  }

  pclose(fd);
  pg_system_apt_upgrade = 0;
  gslc_ElemSetTxtStr(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], "Finished");
  gslc_Update(pGui);
  */
  return true;
}

bool pg_system_cbBtn_startup(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_STARTUP);
  return true;
}

bool pg_system_cbBtn_reconfigButtons(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_CONFIGURE);
  touchscreenPageGoBack(pGui);
  return true;
}


bool pg_system_cbBtn_registerDevice(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  system("/opt/sdobox/scripts/register_device.sh &");
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

  pg_systemEl[E_SYSTEM_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen);
  gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_BOX], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);

  gslc_ElemCreateImg(pGui, GSLC_ID_AUTO, ePage, rFullscreen, gslc_GetImageFromFile(IMG_SCREEN_SYSTEM, GSLC_IMGREF_FMT_BMP16));

  // Close Key
  pg_systemEl[E_SYSTEM_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){430, 0, 50, 50},
          "", 0, E_FONT_MONO14, &pg_system_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_CLOSE], false);

  // Reboot device
  if ((
    pg_systemEl[E_SYSTEM_EL_REBOOT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {404, 245, 75, 75},
            (char*)"", 0, E_FONT_MONO14, &pg_system_cbBtn_reboot)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], GSLC_COL_WHITE, GSLC_COL_RED, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_REBOOT], false);
  }
/*
  // Close App
  if ((
    pg_systemEl[E_SYSTEM_EL_EXIT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 210), ((rFullscreen.y + rFullscreen.h) - 50), 100, 50},
            "Exit", 0, E_FONT_MONO14, &pg_system_cbBtn_exit)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_EXIT], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_EXIT], GSLC_COL_WHITE, GSLC_COL_RED, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_EXIT], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_EXIT], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_EXIT], true);
  }
*/
  // Startup
  if ((
    pg_systemEl[E_SYSTEM_EL_STARTUP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 0, 74, 74},
            (char*)"", 0, E_FONT_MONO14, &pg_system_cbBtn_startup)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_STARTUP], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_STARTUP], GSLC_COL_WHITE, GSLC_COL_RED, GSLC_COL_YELLOW);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_STARTUP], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_STARTUP], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_STARTUP], false);
  }

  // Wifi Settings
  if ((
    pg_systemEl[E_SYSTEM_EL_WIFI] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {85, 128, 64, 64},
            (char*)"", 0, E_FONT_MONO14, &pg_system_cbBtn_wifi)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_WIFI], false);
  }
/*
  // Powercycle HDMI Port
  if ((
    pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 130, 150, 50},
            "Powercycle HDMI", 0, E_FONT_MONO14, &pg_system_cbBtn_powercycleHdmi)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], GSLC_COL_WHITE, GSLC_COL_RED, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_POWERCYCLEHDMI], true);
  }
*/


  // Update
  if ((
    pg_systemEl[E_SYSTEM_EL_UPGRADE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {180, 150, 104, 64},
            (char*)"", 0, E_FONT_MONO14, &pg_system_cbBtn_upgrade)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], GSLC_COL_WHITE, GSLC_COL_RED, GSLC_COL_YELLOW);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], GSLC_ALIGN_BOT_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_UPGRADE], false);
  }


  // Reconfigure Buttons
  if ((
    pg_systemEl[E_SYSTEM_EL_RECONFIGBUTTONS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 270, 150, 50},
            "Configure Buttons", 0, E_FONT_MONO14, &pg_system_cbBtn_reconfigButtons)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_RECONFIGBUTTONS], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_RECONFIGBUTTONS], GSLC_COL_WHITE, GSLC_COL_RED, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_RECONFIGBUTTONS], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_RECONFIGBUTTONS], false);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_RECONFIGBUTTONS], true);
  }

  // Reconfigure Buttons
  if ((
    pg_systemEl[E_SYSTEM_EL_REGISTERDEVICE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {180, 270, 150, 50},
            "Register Device", 0, E_FONT_MONO14, &pg_system_cbBtn_registerDevice)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_systemEl[E_SYSTEM_EL_REGISTERDEVICE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_systemEl[E_SYSTEM_EL_REGISTERDEVICE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_RED);
    gslc_ElemSetTxtAlign(pGui, pg_systemEl[E_SYSTEM_EL_REGISTERDEVICE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_systemEl[E_SYSTEM_EL_REGISTERDEVICE], true);
    gslc_ElemSetFrameEn(pGui, pg_systemEl[E_SYSTEM_EL_REGISTERDEVICE], true);
  }

  return 1;
}

void pg_system_btnSetFuncs() {

}


// GUI Init
void pg_system_init(gslc_tsGui *pGui) {
  pg_system_apt_upgrade = 0;

  // Create Interface
  pg_system_guiInit(pGui);

  cbInit[E_PG_SYSTEM] = NULL;
}


// GUI Open
void pg_system_open(gslc_tsGui *pGui) {
  pg_system_btnSetFuncs();
}

void pg_system_close(gslc_tsGui *pGui) {

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
