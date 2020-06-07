#include <time.h>
#include <unistd.h>

#include "GUIslice/src/GUIslice.h"
#include "GUIslice-wrapper.h"
#include "gui/keyboard/keyboard.h"
#include "libs/dbg/dbg.h"
#include "libs/fbcp/fbcp.h"
#include "libs/clock/clock.h"

uint32_t guislice_wrapper_clockUpdate;
char *timeStr;

// Configure environment variables suitable for display
void UserInitEnv()
{
  // setenv((char*)"FRAMEBUFFER",GSLC_DEV_FB,1);

  // Honor whatever the user may have set in the environment already
  if (!getenv("SDL_FBDEV")) {
    // Auto-detect the right fbdev
    FILE* input = popen("/bin/sh -c \"grep fb_ili9486 /proc/fb | head -1 | awk '{printf \\\"/dev/fb%d\\\",\\$1}'\"", "r");
    char result[16];		/* more than enough for "/dev/fb#" */
    char* got = fgets(result, sizeof(result), input);
    if (got && strlen(got) > 0) {
      dbgprintf(DBG_INFO, "GUISlice-wrapper: autodetected fbdev at %s\n", got);
      setenv("SDL_FBDEV",got,0);
    }
    else {
      // Nothing auto-detected, use GSLC_DEV_FB
      dbgprintf(DBG_DEBUG, "GUISlice-wrapper: failed to locate touchscreen, defaulting to %s\n", GSLC_DEV_FB);
      setenv("SDL_FBDEV",GSLC_DEV_FB,1);
    }
    fclose(input);
  }
  setenv((char*)"SDL_VIDEODRIVER",GSLC_DEV_VID_DRV,1);
  setenv((char*)"TSLIB_FBDEVICE",GSLC_DEV_FB,1);
  setenv((char*)"TSLIB_TSDEVICE",GSLC_DEV_TOUCH,1);
  setenv((char*)"TSLIB_CALIBFILE",(char*)"/usr/local/etc/pointercal",1);
  setenv((char*)"TSLIB_CONFFILE",(char*)"/usr/local/etc/ts.conf",1);
  setenv((char*)"TSLIB_PLUGINDIR",(char*)"/usr/local/lib/ts",1);
}

int guislice_wrapper_init(gslc_tsGui *pGui) {
  if (gslc_wrapper_initalized) { return 1; }

  gslc_tsRect rF = {0, 0, 480, 320};
  rFullscreen = rF;
  // GUISlice Environment
  UserInitEnv();

  guislice_wrapper_clockUpdate = 0;
  timeStr = (char *)malloc(32 * sizeof(char));
  if (!gslc_Init(pGui, &m_drv, m_asPage, MAX_PAGES, m_asFont, MAX_FONT)) { return 0; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
  gslc_FontAdd(pGui,E_FONT_MONO14,GSLC_FONTREF_FNAME,FONT_NATO_MONO1,14);
  gslc_FontAdd(pGui,E_FONT_MONO18,GSLC_FONTREF_FNAME,FONT_NATO_MONO1,18);
  gslc_FontAdd(pGui,E_FONT_MONO24,GSLC_FONTREF_FNAME,FONT_NATO_MONO1,24);
  gslc_FontAdd(pGui,E_FONT_MONO28,GSLC_FONTREF_FNAME,FONT_NATO_MONO1,28);
  gslc_FontAdd(pGui,E_FONT_MONO2,GSLC_FONTREF_FNAME,FONT_NATO_MONO1,30);

  // gslcWrapperThreadStart();


  gslc_wrapper_initalized = 1;
  return 1;
}

int guislice_wrapper_quit(gslc_tsGui *pGui) {
  gslc_wrapper_initalized = 0;
  free(timeStr);
  
  gslc_SetPageCur(pGui, GSLC_PAGE_NONE);
  // gslcWrapperThreadStop();
  gslc_Quit(pGui);

  return 1;
}

// Framebuffer mirroring, global
void guislice_wrapper_mirror_toggle(gslc_tsGui *pGui) {
  if (!fbcp_toggle()) {
    usleep(50000);
    gslc_SetTouchDisabled(pGui, false);
    gslc_SetScreenDisabled(pGui, false);
    gslc_PageRedrawSet(pGui, true);
    system("DISPLAY=:0.0 xinput set-prop 'ADS7846 Touchscreen' 'Device Enabled' 0 &");
  } else {
    gslc_SetTouchDisabled(pGui, true);
    gslc_SetScreenDisabled(pGui, true);
    gslc_PageRedrawSet(pGui, false);
    system("DISPLAY=:0.0 xinput set-prop 'ADS7846 Touchscreen' 'Device Enabled' 1 &");
  }
}

int guislice_wrapper_mirroring() {
  return fbcpThreadRunning;
}


void guislice_wrapper_setClock(gslc_tsGui *pGui, gslc_tsElemRef *pElemRef, int forceUpdate) {
  if (guislice_wrapper_mirroring() == 0 && 
    (forceUpdate == 1 || millis() - guislice_wrapper_clockUpdate > 1000)
  ) {
    // Reset milli tracking var
    if (forceUpdate == 0 || guislice_wrapper_clockUpdate == 0) {
      guislice_wrapper_clockUpdate = millis();
    }
    clock_getTime(&timeStr);
    gslc_ElemSetTxtStr(pGui, pElemRef, timeStr);
  }
}
