#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <libgen.h>       // For path parsing
#include "main.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "libs/audio/audio.h"
#include "libs/backlight/backlight.h"
#include "libs/buttons/buttons.h"
// #include "libs/mpv-zmq/mpv-zmq.h"

#include "gui/pages.h"
#include "gui/keyboard/keyboard.h"
#include "gui/skydiveorbust/skydiveorbust.h"


////////////////
// Button Callback
bool pg_main_cbBtn_settings(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  // XDOTOOL Open Window, Maximize
  // awk '$4 == galculator {print $1}' <(wmctrl -lp) | xargs -i% sh -c "xwit -id % -pop -raise; wmctrl -i -r % -b add,maximized_vert,maximized_horz"
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SYSTEM);
  return true;
}

bool pg_main_cbBtn_chromium_browser(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  system("DISPLAY=:0.0 /usr/bin/wmctrl -a '- Chromium' || DISPLAY=:0.0 /usr/bin/chromium-browser &");
  return true;
}

bool pg_main_cbBtn_galculator(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  system("DISPLAY=:0.0 /usr/bin/wmctrl -a 'galculator' || DISPLAY=:0.0 /usr/bin/galculator &");
  return true;
}

bool pg_main_cbBtn_file_manager(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  system("DISPLAY=:0.0 /usr/bin/wmctrl -a 'xdg-open' || DISPLAY=:0.0 /usr/bin/xdg-open /home/pi &");
  return true;
}

bool pg_main_cbBtn_vscode(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  system("DISPLAY=:0.0 /usr/bin/wmctrl -a '- Code - OSS (headmelted)' || DISPLAY=:0.0 /usr/bin/code-oss &");
  return true;
}

bool pg_main_cbBtn_spotify(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SPOTIFY);
  return true;
}

bool pg_main_cbBtn_slideshow(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_DUBBING);
  return true;
}

bool pg_main_cbBtn_cookbook(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_COOKBOOK);
  return true;
}

bool pg_main_cbBtn_parachute(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SKYDIVEORBUST);
  return true;
}



//////////////////
// Volume Slider
bool CbSlidePosVolume(void* pvGui, void* pvElemRef, int16_t nPos)
{
  m_nPosVolume = nPos;
  volume_new = nPos;
  return true;
}


//////////////////
// Box Drawing
bool pg_main_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
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
void pg_mainGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_MAIN;
  
  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgMainElem, MAX_ELEM_PG_MAIN, m_asPgMainElemRef, MAX_ELEM_PG_MAIN);

  // Main View Box
  pg_mainEl[E_MAIN_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, (gslc_tsRect){0, 0, 480, 320});
  gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_mainEl[E_MAIN_EL_BOX], &pg_main_cbDrawBox);

  gslc_ElemCreateImg(pGui, GSLC_ID_AUTO, ePage, rFullscreen, gslc_GetImageFromFile(IMG_SCREEN_MAIN_1, GSLC_IMGREF_FMT_BMP16));

  /////////////////////
  // Page Defined Elements

  gslc_DrawLine(pGui, 5, 5, 100, 100, GSLC_COL_WHITE);
  // Chromium Browser Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_CHROMIUM] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){25, 75, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_chromium_browser)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_CHROMIUM], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_CHROMIUM], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_CHROMIUM], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_CHROMIUM], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_CHROMIUM], false);
  }

  // Galculator Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_GALCULATOR] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){110, 75, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_galculator)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_GALCULATOR], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_GALCULATOR], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_GALCULATOR], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_GALCULATOR], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_GALCULATOR], false);
  }

  // File Manager Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_FILE_MANAGER] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){220, 75, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_file_manager)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_FILE_MANAGER], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_FILE_MANAGER], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_FILE_MANAGER], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_FILE_MANAGER], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_FILE_MANAGER], false);
  }



  // VSCode Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_VSCODE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){290, 75, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_vscode)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_VSCODE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_VSCODE], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_VSCODE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_VSCODE], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_VSCODE], false);
  }


  // Spotify Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_SPOTIFY] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){380, 75, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_spotify)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_SPOTIFY], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_SPOTIFY], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_SPOTIFY], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_SPOTIFY], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_SPOTIFY], false);
  }

  // Slideshow Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_SLIDESHOW] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){25, 160, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_slideshow)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_SLIDESHOW], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_SLIDESHOW], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_SLIDESHOW], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_SLIDESHOW], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_SLIDESHOW], false);
  }

  // Cookbook Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_COOKBOOK] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){110, 160, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_cookbook)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_COOKBOOK], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_COOKBOOK], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_COOKBOOK], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_COOKBOOK], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_COOKBOOK], false);
  }

  // Parachute Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_PARACHUTE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){200, 160, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_parachute)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_PARACHUTE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_PARACHUTE], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_PARACHUTE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_PARACHUTE], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_PARACHUTE], false);
  }

  // Settings Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_SETTINGS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){395, 225, 64, 64},
            "", 0, E_FONT_MONO14, &pg_main_cbBtn_settings)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_SETTINGS], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_SETTINGS], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_SETTINGS], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_SETTINGS], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_SETTINGS], false);
  }

  // Volume
  if ((
    pg_mainEl[E_MAIN_EL_VOLUME] = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO, ePage, &m_sXSlider_Volume,
          (gslc_tsRect){(rFullscreen.x + 25), (rFullscreen.h - 60), 210, 40},
          0, 104, m_nPosVolume, 5, false)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_VOLUME], GSLC_COL_YELLOW, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemXSliderSetStyle(pGui, pg_mainEl[E_MAIN_EL_VOLUME], true, GSLC_COL_RED_DK4, 10, 10, GSLC_COL_GRAY_DK2);
    // gslc_ElemXSliderSetStyleCustom(pGui, pg_mainEl[E_MAIN_EL_VOLUME], pg_mainPosVolumeTicks, true, true); //true, GSLC_COL_RED_DK4, 10, 5, GSLC_COL_GRAY_DK2);
    gslc_ElemXSliderSetPosFunc(pGui, pg_mainEl[E_MAIN_EL_VOLUME], &CbSlidePosVolume);
  }

  // Volume Display
  if ((
    pg_mainEl[E_MAIN_EL_VOLUME_DISPLAY] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 25), (rFullscreen.h - 25), 210, 25},
          (char*)"Volume:", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_VOLUME_DISPLAY], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_VOLUME_DISPLAY], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_VOLUME_DISPLAY], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_VOLUME_DISPLAY], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_VOLUME_DISPLAY], false);
  }

  // Clock
  if ((
    pg_mainEl[E_MAIN_EL_CLOCK] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){240, (rFullscreen.h - 25), 236, 25},
          (char*)"", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_CLOCK], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_CLOCK], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_CLOCK], GSLC_ALIGN_MID_RIGHT);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_CLOCK], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_CLOCK], false);
  }


}

void pg_mainUpdateVolume() {
  if (volume_no_device == 0) {
    return;
  }
  long volLvl, volMin, volMax;
  volume_getVolumeRange(&volMin, &volMax);
  if (volume_getVolume(&volLvl)) {
    if (volMin < 0) {
      volume_dbToPercent(volLvl, volMin, volMax, &volume_cur);
    } else {
      volume_cur = (volLvl * 100) / (volMax - volMin);
    }
  }
}

int pg_mainButtonRotaryCW() {
  volume_new = m_nPosVolume + 8;
  if (volume_new > 104) { volume_new = 104; }
  m_nPosVolume = volume_new;
  return 1;
}
int pg_mainButtonRotaryCCW() {
  volume_new = m_nPosVolume - 8;
  if (volume_new < 0) { volume_new = 0; }
  m_nPosVolume = volume_new;
  return 1;
}
int pg_mainButtonLeftPressed() {
  system("/opt/sdobox/scripts/spotify/spotify_cmd.sh previous");
  return 1;
}
int pg_mainButtonRightPressed() {
  system("/opt/sdobox/scripts/spotify/spotify_cmd.sh next");
  return 1;
}
int pg_mainButtonRotaryPressed() {
  system("/opt/sdobox/scripts/spotify/spotify_cmd.sh pause");
  return 1;
}
int pg_mainButtonLeftHeld() {
  backlight_off();
  return 1;
}
int pg_mainButtonRightHeld() {
  backlight_on();
  return 1;
}
int pg_mainButtonRotaryHeld() {
  guislice_wrapper_mirror_toggle(&m_gui);
  return 1;
}
int pg_mainButtonDoubleHeld() {
  touchscreenPageGoBack(&m_gui);
  return 1;
}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_mainButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_mainButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_mainButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_mainButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_mainButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_mainButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_mainButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_mainButtonRightHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, &pg_mainButtonRotaryHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, &pg_mainButtonDoubleHeld);
}


// GUI Init
void pg_main_init(gslc_tsGui *pGui) {
  pg_mainGuiInit(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_MAIN] = NULL;
}



// GUI Open
void pg_main_open(gslc_tsGui *pGui) {
  /* Volume Update
  long pg_main_volpercent = 0;
  volume_dbToPercent(volume_cur, volume_min, volume_max, &pg_main_volpercent);
  m_nPosVolume = pg_main_volpercent;

  pg_mainUpdateVolume();
  */
  // Setup button function callbacks every time page is opened / reopened
  pg_mainButtonSetFuncs();
}

// GUI Thread
int pg_main_thread(gslc_tsGui *pGui) {
  // printf("Main Thread\n");
  guislice_wrapper_setClock(pGui, pg_mainEl[E_MAIN_EL_CLOCK], 0);
  // guislice_wrapper_setVolumeAndDisplay(pGui, pg_mainEl[E_MAIN_EL_VOLUME], 0, pg_mainEl[E_MAIN_EL_VOLUME_DISPLAY]);
  // volume_debounceCheck();
  return 0;
}

// GUI Destroy
void pg_main_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_main_setup(void) {
  cbInit[E_PG_MAIN] = &pg_main_init;
  cbOpen[E_PG_MAIN] = &pg_main_open;
  cbThread[E_PG_MAIN] = &pg_main_thread;
  cbDestroy[E_PG_MAIN] = &pg_main_destroy;
}
