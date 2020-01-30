#include "main.h"

#include "buttons/buttons.h"
#include "gui/pages.h"

#include "gui/keyboard/keyboard.h"

////////////////
// Button Callback
bool pg_mainCbBtn(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_WIFI);
  gslc_ElemSetTxtStr(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], "Pressed!");

  return true;
}


bool pg_mainCbBtnSlideshow(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SLIDESHOW);
  return true;
}

bool pg_mainCbBtnSDOB(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SKYDIVEORBUST);

  return true;
}

///////////////////////
// Keyboard Button
void pg_mainCbBtnKeyboard_Callback(gslc_tsGui *pGui, char* str) {
  gslc_ElemSetTxtStr(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], str);
}

bool pg_mainCbBtnKeyboard(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Show Keyboard
  pg_keyboard_show(pGui, 16, gslc_ElemGetTxtStr(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB]), &pg_mainCbBtnKeyboard_Callback);
  return true;
}



//////////////////
// Box Drawing
bool pg_mainCbDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);
  gslc_DrawLine(pGui, pRect.x, pRect.y + 60, pRect.x + pRect.w, pRect.y + 60, GSLC_COL_GRAY);

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}




/////////////////////
// Init Gui Elements
void pg_mainGuiInit(gslc_tsGui *pGui) {
  
  // Define page enum (gui/pages.h)
  int ePage = E_PG_MAIN;
  gslc_tsRect rFullscreen = {0,0,480,320};

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgMainElem, MAX_ELEM_PG_DEFAULT, m_asPgMainElemRef, MAX_ELEM_PG_DEFAULT);
  
  // Create Fullscreen Draw Box
  // Must use a box so redrawing between pages functions correctly
  if ((
    pg_mainEl[E_MAIN_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, pg_mainEl[E_MAIN_EL_BOX], &pg_mainCbDraw);
  }



  /////////////////////
  // Page Defined Elements

  // Add Text Element
  if ((
    pg_mainEl[E_MAIN_EL_TXT_TMP] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y, (rFullscreen.w - 108), 60},
          (char*)"Some Text", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], true);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], false);
  }


  // Add Text Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_TMP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.w / 3, rFullscreen.h / 3, rFullscreen.w / 3, rFullscreen.h / 3},
            "Wifi", 0, E_FONT_MONO14, &pg_mainCbBtn)
  ) != NULL) {            
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], true); 
  }


  // Add Text Keyboard Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_TMPKB] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){(rFullscreen.w - 100), rFullscreen.y, 100, 60},
            "Change Text", 0, E_FONT_MONO14, &pg_mainCbBtnKeyboard)
  ) != NULL) {            
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], true); 
  }


  // Open Slideshow
  if ((
    pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {30, 200, 100, 60},
            "Slideshow", 0, E_FONT_MONO14, &pg_mainCbBtnSlideshow)
  ) != NULL) {            
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], true); 
  }

  // Open Skydive or Bust
  if ((
    pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {350, 200, 100, 60},
            "Skydive Or Bust", 0, E_FONT_MONO14, &pg_mainCbBtnSDOB)
  ) != NULL) {            
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], true); 
  }
}


void pg_mainButtonRotaryCW() {
  // debug_print("%s\n", "Main CW");
}
void pg_mainButtonRotaryCCW() {
  // debug_print("%s\n", "Main CCW");
}
void pg_mainButtonLeftPressed() {
  // debug_print("%s\n", "Main Left Pressed");
}
void pg_mainButtonRightPressed() {
  // debug_print("%s\n", "Main Right Pressed");
}
void pg_mainButtonRotaryPressed() {
  // debug_print("%s\n", "Main Rotary Pressed");
}
void pg_mainButtonLeftHeld() {
  // debug_print("%s\n", "Main Left Held");
}
void pg_mainButtonRightHeld() {
  // debug_print("%s\n", "Main Right Held");
}
void pg_mainButtonRotaryHeld() {
  // debug_print("%s\n", "Main Rotary Held");
}
void pg_mainButtonDoubleHeld() {
  // debug_print("%s\n", "Main Double Held");
  // systemMenuOpen();
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
  // Setup button function callbacks every time page is opened / reopened
  pg_mainButtonSetFuncs();

}


// GUI Destroy
void pg_main_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_main_setup(void) {
  cbInit[E_PG_MAIN] = &pg_main_init;
  cbOpen[E_PG_MAIN] = &pg_main_open;
  cbDestroy[E_PG_MAIN] = &pg_main_destroy;
}
