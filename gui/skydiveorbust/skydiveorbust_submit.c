#include "skydiveorbust_submit.h"

#include "libs/buttons/buttons.h"
#include "libs/queue/queue.h"
#include "gui/pages.h"

#include "gui/skydiveorbust/skydiveorbust.h"
#include "gui/keyboard/keyboard.h"

void pg_sdobSubmitClose(gslc_tsGui *pGui) {
  touchscreenPageGoBack(pGui);
}

////////////////
// Button Callback
bool pg_sdobSubmitCbBtnCancel(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  pg_sdobSubmitClose(pGui);
  return true;
}

bool pg_sdobSubmitCbBtnClear(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (pg_sdob_submit_clearCheck == 1) {
    // Clear Scorecard
    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_CLEAR;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    // Close Submit Menu
    pg_sdobSubmitClose(pGui);
  } else {
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR], true);
    pg_sdob_submit_clearCheck = 1;
  }
  return true;
}

bool pg_sdobSubmitCbBtnSubmitScore(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  // Submit Scorecard
  struct queue_head *item = new_qhead();
  item->action = E_Q_SCORECARD_SUBMIT_SCORECARD;
  queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

  // Close Submit Menu
  pg_sdobSubmitClose(pGui);
  return true;
}




//////////////////
// Box Drawing
bool pg_sdobSubmitCbDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);
  gslc_DrawLine(pGui, pRect.x, pRect.y + 60, pRect.x + pRect.w, pRect.y + 60, GSLC_COL_GRAY);

//  gslc_ElemSetTxtStr(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TMP], sdob_judgement->video_file);
  gslc_ElemSetTxtStr(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_VIDEOFILE], sdob_judgement->video_file);

  size_t chapterSz = snprintf(NULL, 0, "Chapter: %d", (sdob_chapters->cur + 1)) + 1;
  if (chapterSz > 0 && chapterSz <= 32) {
    snprintf(sdob_submit_info.chapterStr, chapterSz, "Chapter: %d", (sdob_chapters->cur + 1));
    gslc_ElemSetTxtStr(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_CHAPTER], sdob_submit_info.chapterStr);
  }

  size_t teamSz = snprintf(NULL, 0, "Team: %s", sdob_judgement->team) + 1;
  if (teamSz > 0 && teamSz <= 32) {
    snprintf(sdob_submit_info.teamStr, teamSz, "Team: %s", sdob_judgement->team);
    gslc_ElemSetTxtStr(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TEAMNUM], sdob_submit_info.teamStr);
  }

  size_t roundSz = snprintf(NULL, 0, "Round: %s", sdob_judgement->round) + 1;
  if (roundSz > 0 && roundSz <= 32) {
    snprintf(sdob_submit_info.roundStr, roundSz, "Round: %s", sdob_judgement->round);
    gslc_ElemSetTxtStr(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_ROUNDNUM], sdob_submit_info.roundStr);
  }

  // Set Score Total
  pg_sdobUpdateCount(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE]);

  // Capture set score and add the Score: Prefix
  char* scoreTotal = gslc_ElemGetTxtStr(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE]);
  if (scoreTotal != NULL) {
    size_t scoreSz = snprintf(NULL, 0, "Score: %s", scoreTotal) + 1;
    if (scoreSz > 0 && scoreSz <= 32) {
      snprintf(sdob_submit_info.scoreStr, scoreSz, "Score: %s", scoreTotal);
      gslc_ElemSetTxtStr(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE], sdob_submit_info.scoreStr);
    }
  }

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}




/////////////////////
// Init Gui Elements
void pg_sdobSubmitGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_SDOB_SUBMIT;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgSdobSubmitElem, E_SDOB_SUBMIT_EL_MAX, m_asPgSdobSubmitElemRef, E_SDOB_SUBMIT_EL_MAX);

  // Create Fullscreen Draw Box
  // Must use a box so redrawing between pages functions correctly
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BOX], GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BOX], &pg_sdobSubmitCbDraw);
  }



  /////////////////////
  // Page Defined Elements

  // Video File Text
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_VIDEOFILE] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y, (rFullscreen.w - 148), 60},
          (char*)" ", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_VIDEOFILE], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_VIDEOFILE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_VIDEOFILE], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_VIDEOFILE], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_VIDEOFILE], false);
  }


  // Chapter
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_CHAPTER] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + rFullscreen.w) - 144, rFullscreen.y, 140, 60},
          (char*)" ", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_CHAPTER], GSLC_COL_ORANGE);
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_CHAPTER], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_CHAPTER], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_CHAPTER], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_CHAPTER], false);
  }

  // Score Totals Text
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + (rFullscreen.w / 3)), rFullscreen.y + 65, (rFullscreen.w / 3), 60},
          (char*)" ", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE], false);
  }

  // Team Number
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TEAMNUM] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y + 65, (rFullscreen.w / 3) - 8, 30},
          (char*)" ", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TEAMNUM], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TEAMNUM], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TEAMNUM], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TEAMNUM], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TEAMNUM], false);
  }

    // Round Number
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_ROUNDNUM] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y + 95, (rFullscreen.w / 3) - 8, 30},
          (char*)" ", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_ROUNDNUM], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_ROUNDNUM], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_ROUNDNUM], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_ROUNDNUM], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_ROUNDNUM], false);
  }


  // Cancel Button
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CANCEL] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
            "Cancel", 0, E_FONT_MONO14, &pg_sdobSubmitCbBtnCancel)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CANCEL], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CANCEL], GSLC_COL_RED, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CANCEL], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CANCEL], false);
    gslc_ElemSetFrameEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CANCEL], true);
  }

  // Clear Scorecard Button
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {(rFullscreen.x + rFullscreen.w) - 100, (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
            "Clear", 0, E_FONT_MONO14, &pg_sdobSubmitCbBtnClear)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR], GSLC_COL_PURPLE, GSLC_COL_PURPLE, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR], false);
    gslc_ElemSetFrameEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR], true);
  }

  // Submit Button
  if ((
    pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_SUBMIT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x + ((rFullscreen.w - 100) / 2), (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
            "Submit Score", 0, E_FONT_MONO14, &pg_sdobSubmitCbBtnSubmitScore)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_SUBMIT], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_SUBMIT], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_SUBMIT], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_SUBMIT], false);
    gslc_ElemSetFrameEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_SUBMIT], true);
  }

}


void pg_sdobSubmitButtonRotaryCW() {
  // debug_print("%s\n", "SDOB Submit CW");
}
void pg_sdobSubmitButtonRotaryCCW() {
  // debug_print("%s\n", "SDOB Submit CCW");
}
void pg_sdobSubmitButtonLeftPressed() {
  // debug_print("%s\n", "SDOB Submit Left Pressed");
}
void pg_sdobSubmitButtonRightPressed() {
  // debug_print("%s\n", "SDOB Submit Right Pressed");
}
void pg_sdobSubmitButtonRotaryPressed() {
  // debug_print("%s\n", "SDOB Submit Rotary Pressed");
}
void pg_sdobSubmitButtonLeftHeld() {
  // debug_print("%s\n", "SDOB Submit Left Held");
}
void pg_sdobSubmitButtonRightHeld() {
  // debug_print("%s\n", "SDOB Submit Right Held");
}
void pg_sdobSubmitButtonRotaryHeld() {
  // debug_print("%s\n", "SDOB Submit Rotary Held");
}
void pg_sdobSubmitButtonDoubleHeld() {
  // debug_print("%s\n", "SDOB Submit Double Held");
  // systemMenuOpen();
}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_sdobSubmitButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_sdobSubmitButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_sdobSubmitButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_sdobSubmitButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_sdobSubmitButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_sdobSubmitButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_sdobSubmitButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_sdobSubmitButtonRightHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, &pg_sdobSubmitButtonRotaryHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, &pg_sdobSubmitButtonDoubleHeld);
}


// GUI Init
void pg_sdobSubmit_init(gslc_tsGui *pGui) {
  pg_sdobSubmitGuiInit(pGui);

  CLEAR(&sdob_submit_info,sizeof(sdob_submit_info));

  // Cleanup so Init is only ran once
  cbInit[E_PG_SDOB_SUBMIT] = NULL;
}


// GUI Open
void pg_sdobSubmit_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_sdobSubmitButtonSetFuncs();

  gslc_ElemSetFillEn(pGui, pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR], false);
  pg_sdob_submit_clearCheck = 0;
}


// GUI Destroy
void pg_sdobSubmit_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_sdobSubmit_setup(void) {
  cbInit[E_PG_SDOB_SUBMIT] = &pg_sdobSubmit_init;
  cbOpen[E_PG_SDOB_SUBMIT] = &pg_sdobSubmit_open;
  cbDestroy[E_PG_SDOB_SUBMIT] = &pg_sdobSubmit_destroy;
}
