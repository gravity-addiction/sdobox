#include <stdio.h>
#include "cookbook.h"

#include "libs/buttons/buttons.h"
#include "gui/pages.h"


////////////////
// Button Callback

bool pg_cookbook_cbBtn_x(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  // XDOTOOL Open Window, Maximize
  // awk '$4 == galculator {print $1}' <(wmctrl -lp) | xargs -i% sh -c "xwit -id % -pop -raise; wmctrl -i -r % -b add,maximized_vert,maximized_horz"
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageGoBack(pGui);
  return true;
}

bool pg_cookbook_cbBtn_hello_world(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_COOKBOOK_HELLO_WORLD);
  return true;
}


//////////////////
// Box Drawing
bool pg_cookbook_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
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
void pg_cookbookGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_COOKBOOK;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgCookbookElem, MAX_ELEM_PG_COOKBOOK, m_asPgCookbookElemRef, MAX_ELEM_PG_COOKBOOK);

  // Cookbook View Box
  pg_cookbookEl[E_COOKBOOK_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, (gslc_tsRect){0, 0, 480, 320});
  gslc_ElemSetCol(pGui, pg_cookbookEl[E_COOKBOOK_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_cookbookEl[E_COOKBOOK_EL_BOX], &pg_cookbook_cbDrawBox);

  gslc_ElemCreateImg(pGui, GSLC_ID_AUTO, ePage, rFullscreen, gslc_GetImageFromFile(IMG_SCREEN_COOKBOOK, GSLC_IMGREF_FMT_BMP16));
  
  /////////////////////
  // Page Defined Elements


  // Hello World Button
  if ((
    pg_cookbookEl[E_COOKBOOK_EL_BTN_HELLO_WORLD] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){25, 75, 64, 64},
            "", 0, E_FONT_MONO14, &pg_cookbook_cbBtn_hello_world)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_HELLO_WORLD], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_HELLO_WORLD], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_HELLO_WORLD], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_HELLO_WORLD], false);
    gslc_ElemSetFrameEn(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_HELLO_WORLD], true);
  }

  // X
  if ((
    pg_cookbookEl[E_COOKBOOK_EL_BTN_X] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){437, 0, 42, 42},
            "", 0, E_FONT_MONO14, &pg_cookbook_cbBtn_x)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_X], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_X], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_X], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_X], false);
    gslc_ElemSetFrameEn(pGui, pg_cookbookEl[E_COOKBOOK_EL_BTN_X], false);
  }

  // Clock
  if ((
    pg_cookbookEl[E_COOKBOOK_EL_CLOCK] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){240, (rFullscreen.h - 25), 236, 25},
          (char*)"", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_cookbookEl[E_COOKBOOK_EL_CLOCK], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_cookbookEl[E_COOKBOOK_EL_CLOCK], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_cookbookEl[E_COOKBOOK_EL_CLOCK], GSLC_ALIGN_MID_RIGHT);
    gslc_ElemSetFillEn(pGui, pg_cookbookEl[E_COOKBOOK_EL_CLOCK], false);
    gslc_ElemSetFrameEn(pGui, pg_cookbookEl[E_COOKBOOK_EL_CLOCK], false);
  }





}


void pg_cookbookButtonRotaryCW() {

}
void pg_cookbookButtonRotaryCCW() {

}
void pg_cookbookButtonLeftPressed() {

}
void pg_cookbookButtonRightPressed() {

}
void pg_cookbookButtonRotaryPressed() {

}
void pg_cookbookButtonLeftHeld() {

}
void pg_cookbookButtonRightHeld() {

}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_cookbookButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_cookbookButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_cookbookButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_cookbookButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_cookbookButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_cookbookButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_cookbookButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_cookbookButtonRightHeld);
}


// GUI Init
void pg_cookbook_init(gslc_tsGui *pGui) {
  pg_cookbookGuiInit(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_COOKBOOK] = NULL;
}


// GUI Open
void pg_cookbook_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_cookbookButtonSetFuncs();

  guislice_wrapper_setClock(pGui, pg_cookbookEl[E_COOKBOOK_EL_CLOCK], 1);
}

// GUI Thread
uint32_t pg_cookbook_clockUpdate = 0;
int pg_cookbook_thread(gslc_tsGui *pGui) {
  guislice_wrapper_setClock(pGui, pg_cookbookEl[E_COOKBOOK_EL_CLOCK], 0);
  return 0;
}
// GUI Destroy
void pg_cookbook_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_cookbook_setup(void) {
  cbInit[E_PG_COOKBOOK] = &pg_cookbook_init;
  cbOpen[E_PG_COOKBOOK] = &pg_cookbook_open;
  cbThread[E_PG_COOKBOOK] = &pg_cookbook_thread;
  cbDestroy[E_PG_COOKBOOK] = &pg_cookbook_destroy;
}
