#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "main.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "libs/audio/audio.h"
#include "libs/buttons/buttons.h"
#include "libs/mpv/mpv.h"
#include "libs/mpv/mpv_events.h"

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

  system("sudo -H -u pi bash -c \"DISPLAY=:0.0 /usr/bin/wmctrl -a '- Chromium' || DISPLAY=:0.0 /usr/bin/chromium-browser &\"");
  return true;
}

bool pg_main_cbBtn_galculator(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  system("sudo -H -u pi bash -c \"DISPLAY=:0.0 /usr/bin/wmctrl -a 'galculator' || DISPLAY=:0.0 /usr/bin/galculator &\"");
  return true;
}

bool pg_main_cbBtn_file_manager(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  system("sudo -H -u pi bash -c \"DISPLAY=:0.0 /usr/bin/wmctrl -a 'xdg-open' || DISPLAY=:0.0 /usr/bin/xdg-open /home/pi/shared &\"");
  return true;
}

bool pg_main_cbBtn_vscode(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  system("sudo -H -u pi bash -c \"DISPLAY=:0.0 /usr/bin/wmctrl -a '- Code - OSS (headmelted)' || DISPLAY=:0.0 /usr/bin/code-oss &\"");
  return true;
}

bool pg_main_cbBtn_slideshow(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SLIDESHOW);
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
void setVolumeDisplay(gslc_tsGui *pGui) {
  if (volume_cur <= -10200) {
    snprintf(m_cPosVolume, 32, "Volume: Mute");
  } else {
    snprintf(m_cPosVolume, 32, "Volume: %0.fdB", (volume_cur * .01));
  }
  gslc_ElemSetTxtStr(pGui, pg_mainEl[E_MAIN_EL_VOLUME_DISPLAY], m_cPosVolume);
}

bool CbSlidePosVolume(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  // gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  //gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);
  m_nPosVolume = gslc_ElemXSliderGetPos(pGui, pElemRef);
  long iVolume = m_nPosVolume - 10239;
  volume_setVolume(iVolume);
  setVolumeDisplay(pGui);
  

/*
  // Fetch the new RGB component from the slider
  switch (pElem->nId) {
    case E_SLIDER_R:
      m_nPosR = gslc_ElemXSliderGetPos(pGui,pElemRef);
      break;
    case E_SLIDER_G:
      m_nPosG = gslc_ElemXSliderGetPos(pGui,pElemRef);
      break;
    case E_SLIDER_B:
      m_nPosB = gslc_ElemXSliderGetPos(pGui,pElemRef);
      break;
    default:
      break;
  }

  // Calculate the new RGB value
  gslc_tsColor colRGB = (gslc_tsColor){m_nPosR,m_nPosG,m_nPosB};

  // Update the color box
  gslc_tsElemRef* pElemColor = gslc_PageFindElemById(pGui,E_PG_MAIN,E_ELEM_COLOR);
  gslc_ElemSetCol(pGui, pElemColor, GSLC_COL_WHITE, colRGB, GSLC_COL_WHITE);
*/
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
          0, 10639, m_nPosVolume, 5, false)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_VOLUME], GSLC_COL_RED, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemXSliderSetStyle(pGui, pg_mainEl[E_MAIN_EL_VOLUME], true, GSLC_COL_RED_DK4, 10, 5, GSLC_COL_GRAY_DK2);
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


void pg_mainButtonRotaryCW() {

}
void pg_mainButtonRotaryCCW() {

}
void pg_mainButtonLeftPressed() {

}
void pg_mainButtonRightPressed() {

}
void pg_mainButtonRotaryPressed() {

}
void pg_mainButtonLeftHeld() {

}
void pg_mainButtonRightHeld() {

}
void pg_mainButtonRotaryHeld() {
  guislice_wrapper_mirror_toggle(&m_gui);
}
void pg_mainButtonDoubleHeld() {
  touchscreenPageGoBack(&m_gui);
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
  m_cPosVolume = (char*)calloc(32, sizeof(char));


  // Cleanup so Init is only ran once
  cbInit[E_PG_MAIN] = NULL;
}


// GUI Open
void pg_main_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_mainButtonSetFuncs();
  volume_getVolume();
  setVolumeDisplay(pGui);
  int iVol = volume_cur + 10239;
  gslc_ElemXSliderSetPos(pGui, pg_mainEl[E_MAIN_EL_VOLUME], iVol);

  guislice_wrapper_setClock(pGui, pg_mainEl[E_MAIN_EL_CLOCK], 1);
}

// GUI Thread
uint32_t pg_main_clockUpdate = 0;
int pg_main_thread(gslc_tsGui *pGui) {
  guislice_wrapper_setClock(pGui, pg_mainEl[E_MAIN_EL_CLOCK], 0);
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
