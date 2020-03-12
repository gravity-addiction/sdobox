#include "msgbox.h"

#include "gui/pages.h"

void pg_msgbox_setTitle(gslc_tsGui *pGui, const char* title) {
  gslc_ElemSetTxtStr(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_HEADER], title);
}
void pg_msgbox_setMsg(gslc_tsGui *pGui, char* msg) {
  gslc_ElemXTextboxAdd(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_MSG], msg);
}

bool pg_msgbox_cbBtn_Ok(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPopupMsgBoxClose(pGui);

  return true;
}


/////////////////////
// Init Gui Elements
void pg_msgboxGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_MSGBOX;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgMsgboxElem, MAX_ELEM_PG_MSGBOX, m_asPgMsgboxElemRef, MAX_ELEM_PG_MSGBOX);

  // Create textbox
  pg_msgboxEl[E_MSGBOX_EL_BOX_MSG] = gslc_ElemXTextboxCreate(pGui, GSLC_ID_AUTO, ePage,
          &pg_msgboxTextbox,
          (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + 60), rFullscreen.w, (rFullscreen.h - 60)},
          E_FONT_MONO18, pg_msgboxBuf, pg_msgboxRows, pg_msgboxCols);
  gslc_ElemXTextboxWrapSet(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_MSG], true);
  gslc_ElemSetCol(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_MSG], GSLC_COL_BLUE_LT2, GSLC_COL_BLACK, GSLC_COL_GRAY_DK3);

  gslc_DrawFillRect(pGui, (gslc_tsRect) {rFullscreen.x, rFullscreen.y, rFullscreen.w, 60}, GSLC_COL_BLACK);

  pg_msgboxEl[E_MSGBOX_EL_BOX_HEADER] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x, rFullscreen.y, rFullscreen.w, 60},
          (char*)"SYSTEM MESSAGE", 0, E_FONT_MONO18);
  gslc_ElemSetCol(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_HEADER], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtCol(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_HEADER], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_HEADER], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_HEADER], true);
  gslc_ElemSetFrameEn(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_HEADER], false);

  pg_msgboxEl[E_MSGBOX_EL_BOX_OK] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x + rFullscreen.w - 80, rFullscreen.y, 80, 60},
          (char*)"OK", 0, E_FONT_MONO18, &pg_msgbox_cbBtn_Ok);
  gslc_ElemSetTxtCol(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_OK], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_OK], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_OK], false);
  gslc_ElemSetFrameEn(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_OK], true);

}


// GUI Init
void pg_msgbox_init(gslc_tsGui *pGui) {
  pg_msgboxRows = 10;
  pg_msgboxCols = 43;
  pg_msgboxBuf = calloc((pg_msgboxRows * pg_msgboxCols) + 1, sizeof(char));

  pg_msgboxGuiInit(pGui);
  // gslc_ElemXTextboxReset(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_MSG]);

  // Cleanup so Init is only ran once
  cbInit[E_PG_MSGBOX] = NULL;
}


// GUI Open
void pg_msgbox_open(gslc_tsGui *pGui) {
  gslc_ElemSetRedraw(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX], GSLC_REDRAW_FULL);
}

// GUI Close
void pg_msgbox_close(gslc_tsGui *pGui) {
  gslc_ElemXTextboxReset(pGui, pg_msgboxEl[E_MSGBOX_EL_BOX_MSG]);
}

// GUI Destroy
void pg_msgbox_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_msgbox(void) {
  cbInit[E_PG_MSGBOX] = &pg_msgbox_init;
  cbOpen[E_PG_MSGBOX] = &pg_msgbox_open;
  cbClose[E_PG_MSGBOX] = &pg_msgbox_close;
  cbDestroy[E_PG_MSGBOX] = &pg_msgbox_destroy;
}
