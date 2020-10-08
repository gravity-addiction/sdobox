#include <stdio.h>
#include "spotify.h"

#include "libs/buttons/buttons.h"
#include "gui/pages.h"


////////////////
// Button Callback

bool pg_spotify_cbBtn_x(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  // XDOTOOL Open Window, Maximize
  // awk '$4 == galculator {print $1}' <(wmctrl -lp) | xargs -i% sh -c "xwit -id % -pop -raise; wmctrl -i -r % -b add,maximized_vert,maximized_horz"
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageGoBack(pGui);
  return true;
}



//////////////////
// Box Drawing
bool pg_spotify_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
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
void pg_spotifyGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_SPOTIFY;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgSpotifyElem, MAX_ELEM_PG_SPOTIFY, m_asPgSpotifyElemRef, MAX_ELEM_PG_SPOTIFY);

  // Spotify View Box
  pg_spotifyEl[E_SPOTIFY_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, (gslc_tsRect){0, 0, 480, 320});
  gslc_ElemSetCol(pGui, pg_spotifyEl[E_SPOTIFY_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_spotifyEl[E_SPOTIFY_EL_BOX], &pg_spotify_cbDrawBox);

  gslc_ElemCreateImg(pGui, GSLC_ID_AUTO, ePage, rFullscreen, gslc_GetImageFromFile(IMG_SCREEN_SPOTIFY, GSLC_IMGREF_FMT_BMP16));
  
  /////////////////////
  // Page Defined Elements

  // X
  if ((
    pg_spotifyEl[E_SPOTIFY_EL_BTN_X] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){437, 0, 42, 42},
            "", 0, E_FONT_MONO14, &pg_spotify_cbBtn_x)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_spotifyEl[E_SPOTIFY_EL_BTN_X], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_spotifyEl[E_SPOTIFY_EL_BTN_X], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(pGui, pg_spotifyEl[E_SPOTIFY_EL_BTN_X], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_spotifyEl[E_SPOTIFY_EL_BTN_X], false);
    gslc_ElemSetFrameEn(pGui, pg_spotifyEl[E_SPOTIFY_EL_BTN_X], false);
  }

  // Clock
  if ((
    pg_spotifyEl[E_SPOTIFY_EL_CLOCK] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){240, (rFullscreen.h - 25), 236, 25},
          (char*)"", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_spotifyEl[E_SPOTIFY_EL_CLOCK], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_spotifyEl[E_SPOTIFY_EL_CLOCK], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_spotifyEl[E_SPOTIFY_EL_CLOCK], GSLC_ALIGN_MID_RIGHT);
    gslc_ElemSetFillEn(pGui, pg_spotifyEl[E_SPOTIFY_EL_CLOCK], false);
    gslc_ElemSetFrameEn(pGui, pg_spotifyEl[E_SPOTIFY_EL_CLOCK], false);
  }





}


void pg_spotifyButtonRotaryCW() {

}
void pg_spotifyButtonRotaryCCW() {

}
void pg_spotifyButtonLeftPressed() {

}
void pg_spotifyButtonRightPressed() {

}
void pg_spotifyButtonRotaryPressed() {

}
void pg_spotifyButtonLeftHeld() {

}
void pg_spotifyButtonRightHeld() {

}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_spotifyButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_spotifyButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_spotifyButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_spotifyButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_spotifyButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_spotifyButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_spotifyButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_spotifyButtonRightHeld);
}


// GUI Init
void pg_spotify_init(gslc_tsGui *pGui) {
  pg_spotifyGuiInit(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_SPOTIFY] = NULL;
}


// GUI Open
void pg_spotify_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_spotifyButtonSetFuncs();

  guislice_wrapper_setClock(pGui, pg_spotifyEl[E_SPOTIFY_EL_CLOCK], 1);
}

// GUI Thread
uint32_t pg_spotify_clockUpdate = 0;
int pg_spotify_thread(gslc_tsGui *pGui) {
  guislice_wrapper_setClock(pGui, pg_spotifyEl[E_SPOTIFY_EL_CLOCK], 0);
  return 0;
}
// GUI Destroy
void pg_spotify_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_spotify_setup(void) {
  cbInit[E_PG_SPOTIFY] = &pg_spotify_init;
  cbOpen[E_PG_SPOTIFY] = &pg_spotify_open;
  cbThread[E_PG_SPOTIFY] = &pg_spotify_thread;
  cbDestroy[E_PG_SPOTIFY] = &pg_spotify_destroy;
}
