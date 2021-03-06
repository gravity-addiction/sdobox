#include <stdio.h>

#include "libs/buttons/buttons.h"

#include "gui/pages.h"

enum {
  E_COOKBOOK_HELLO_WORLD_EL_BOX,
  E_COOKBOOK_HELLO_WORLD_EL_CLOCK,
  E_COOKBOOK_HELLO_WORLD_EL_BTN_X,
  
  E_COOKBOOK_HELLO_WORLD_EL_MAX
};

#define MAX_ELEM_PG_COOKBOOK_HELLO_WORLD      E_COOKBOOK_HELLO_WORLD_EL_MAX
#define MAX_ELEM_PG_COOKBOOK_HELLO_WORLD_RAM  MAX_ELEM_PG_COOKBOOK_HELLO_WORLD

gslc_tsElem m_asPgCookbookHelloWorldElem[MAX_ELEM_PG_COOKBOOK_HELLO_WORLD_RAM];
gslc_tsElemRef m_asPgCookbookHelloWorldElemRef[MAX_ELEM_PG_COOKBOOK_HELLO_WORLD];

gslc_tsElemRef* pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_MAX];


////////////////
// Button Callback

bool pg_cookbookHelloWorld_cbBtn_x(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  // XDOTOOL Open Window, Maximize
  // awk '$4 == galculator {print $1}' <(wmctrl -lp) | xargs -i% sh -c "xwit -id % -pop -raise; wmctrl -i -r % -b add,maximized_vert,maximized_horz"
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageGoBack(pGui);
  return true;
}


//////////////////
// Box Drawing
bool pg_cookbookHelloWorld_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
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
void pg_cookbookHelloWorldGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_COOKBOOK_HELLO_WORLD;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgCookbookHelloWorldElem, MAX_ELEM_PG_COOKBOOK_HELLO_WORLD, m_asPgCookbookHelloWorldElemRef, MAX_ELEM_PG_COOKBOOK_HELLO_WORLD);

  // Cookbook View Box
  pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, (gslc_tsRect){0, 0, 480, 320});
  gslc_ElemSetCol(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BOX], &pg_cookbookHelloWorld_cbDrawBox);

  gslc_ElemCreateImg(pGui, GSLC_ID_AUTO, ePage, rFullscreen, gslc_GetImageFromFile(IMG_SCREEN_BLANK, GSLC_IMGREF_FMT_BMP16));
  
  /////////////////////
  // Page Defined Elements


  // X
  if ((
    pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BTN_X] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){437, 0, 42, 42},
            "", 0, E_FONT_MONO14, &pg_cookbookHelloWorld_cbBtn_x)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BTN_X], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BTN_X], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BTN_X], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BTN_X], false);
    gslc_ElemSetFrameEn(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_BTN_X], false);
  }
  
  // Clock
  if ((
    pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_CLOCK] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){240, (rFullscreen.h - 25), 236, 25},
          (char*)"", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_CLOCK], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_CLOCK], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_CLOCK], GSLC_ALIGN_MID_RIGHT);
    gslc_ElemSetFillEn(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_CLOCK], false);
    gslc_ElemSetFrameEn(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_CLOCK], false);
  }


}


void pg_cookbookHelloWorldButtonRotaryCW() {

}
void pg_cookbookHelloWorldButtonRotaryCCW() {

}
void pg_cookbookHelloWorldButtonLeftPressed() {

}
void pg_cookbookHelloWorldButtonRightPressed() {

}
void pg_cookbookHelloWorldButtonRotaryPressed() {

}
void pg_cookbookHelloWorldButtonLeftHeld() {

}
void pg_cookbookHelloWorldButtonRightHeld() {

}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_cookbookHelloWorldButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_cookbookHelloWorldButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_cookbookHelloWorldButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_cookbookHelloWorldButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_cookbookHelloWorldButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_cookbookHelloWorldButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_cookbookHelloWorldButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_cookbookHelloWorldButtonRightHeld);
}


// GUI Init
void pg_cookbookHelloWorld_init(gslc_tsGui *pGui) {
  pg_cookbookHelloWorldGuiInit(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_COOKBOOK_HELLO_WORLD] = NULL;
}


// GUI Open
void pg_cookbookHelloWorld_open(gslc_tsGui *pGui) {
  pg_cookbookHelloWorldButtonSetFuncs();
  guislice_wrapper_setClock(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_CLOCK], 1);
}

// GUI Thread
uint32_t pg_cookbookHelloWorld_clockUpdate = 0;
int pg_cookbookHelloWorld_thread(gslc_tsGui *pGui) {
  guislice_wrapper_setClock(pGui, pg_cookbookHelloWorldEl[E_COOKBOOK_HELLO_WORLD_EL_CLOCK], 0);
  return 0;
}
// GUI Destroy
void pg_cookbookHelloWorld_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_cookbookHelloWorld_setup(void) {
  cbInit[E_PG_COOKBOOK_HELLO_WORLD] = &pg_cookbookHelloWorld_init;
  cbOpen[E_PG_COOKBOOK_HELLO_WORLD] = &pg_cookbookHelloWorld_open;
  cbThread[E_PG_COOKBOOK_HELLO_WORLD] = &pg_cookbookHelloWorld_thread;
  cbDestroy[E_PG_COOKBOOK_HELLO_WORLD] = &pg_cookbookHelloWorld_destroy;
}
