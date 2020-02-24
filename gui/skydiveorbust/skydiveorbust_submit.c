#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>             /* CHAR_BIT */

#include "skydiveorbust_submit.h"

#include "buttons/buttons.h"
#include "queue/queue.h"
#include "gui/pages.h"

#include "gui/skydiveorbust/skydiveorbust.h"
#include "gui/keyboard/keyboard.h"

////////////////////////////////////////////////////////////////
// Local data:
#define JUDGELIST_PATH "/opt/sdobox/share/judges.txt"
static char** sdob_submit_judges_list = NULL;
static int sdob_submit_num_judges = 0;
static unsigned sdob_submit_selected_judges_mask = 0;
static char* sdob_submit_selected_judges_str = NULL;

static void sdob_submit_update_judges_str() {
  free(sdob_submit_selected_judges_str);
  sdob_submit_selected_judges_str = NULL;

  int len=0;
  for(int i=0;i<E_SDOB_SUBMIT_MAX_JUDGES;++i) {
    if (sdob_submit_selected_judges_mask & (1U<<i)) {
      if (len > 0)
        ++len;                  /* @ */
      len += strlen(sdob_submit_judges_list[i]);
    }
  }

  dbgprintf(DBG_INFO, "len = %d\n", len);

  sdob_submit_selected_judges_str = calloc(len+1,1);
  char* dst = sdob_submit_selected_judges_str;

  for(int i=0;i<E_SDOB_SUBMIT_MAX_JUDGES; ++i) {
    if (sdob_submit_selected_judges_mask & (1U<<i)) {
      if (dst != sdob_submit_selected_judges_str)
        *dst++ = '@';
      strcpy(dst, sdob_submit_judges_list[i]);
      dst += strlen(sdob_submit_judges_list[i]);
    }
  }

  dbgprintf(DBG_INFO, "sdob_submit_selected_judges_str now is '%s'\n",
            sdob_submit_selected_judges_str);

  assert(strlen(sdob_submit_selected_judges_str) == len);

  dbgprintf(DBG_INFO, "sdob_submit_selected_judges_str now is '%s'\n",
            sdob_submit_selected_judges_str);

  strlcpy(sdob_judgement->judge,
          sdob_submit_selected_judges_str,
          64);
}

static void pg_sdob_submit_destroy_judges_list() {
  for(int i=0;i<sdob_submit_num_judges;++i)
    free(sdob_submit_judges_list[i]);
  free(sdob_submit_judges_list);
  sdob_submit_judges_list = NULL;
  sdob_submit_num_judges = 0;

  free(sdob_submit_selected_judges_str);
  sdob_submit_selected_judges_str = NULL;
}

static void pg_sdob_submit_init_judges_list() {
  // Just in case this is a refresh?
  pg_sdob_submit_destroy_judges_list();
  FILE* f = fopen(JUDGELIST_PATH, "r");
  if (f) {
    char* line = NULL;
    size_t len = 0;
    ssize_t nread;
    while((nread = getline(&line, &len, f)) >= 0)
      ++sdob_submit_num_judges;

    sdob_submit_judges_list = malloc(sdob_submit_num_judges * sizeof(char*));
    rewind(f);
    for(int i=0;i<sdob_submit_num_judges;++i) {
      nread = getline(&line,&len,f);
      assert(nread >= 0);
      assert(line[strlen(line)-1] == '\n');
      line[strlen(line)-1] = '\0';
      sdob_submit_judges_list[i] = strdup(line);

      dbgprintf(DBG_INFO, "judge[%d] = %s\n", i, sdob_submit_judges_list[i]);
    }
    fclose(f);
  }
  else {
    dbgprintf(DBG_ERROR, "failed to open %s, %s\n", JUDGELIST_PATH, strerror(errno));
  }
}
////////////////////////////////////////////////////////////////

void pg_sdobSubmitClose(gslc_tsGui *pGui) {
  touchscreenPageClose(pGui, E_PG_SDOB_SUBMIT);
  touchscreenPageOpen(pGui, m_page_previous);
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

  // If there are available judges to pick from, you must pick at least one
  if (sdob_submit_num_judges > 0 && sdob_submit_selected_judges_mask == 0) {
    dbgprintf(DBG_ERROR, "SUBMIT: attempted to submit without selecting any judges\n");
    return true;
  }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  // Submit Scorecard
  struct queue_head *item = new_qhead();
  item->action = E_Q_SCORECARD_SUBMIT_SCORECARD;
  queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

  // Close Submit Menu
  pg_sdobSubmitClose(pGui);
  return true;
}

bool pg_sdobSubmitCbBtnSelectJudge(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  unsigned mask;

  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // which index is this?
  int jindex = 0;
  for(; jindex < sdob_submit_num_judges; ++jindex) {
    if (pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_JUDGE0 + jindex] == pvElemRef)
      goto found_it;
  }
  dbgprintf(DBG_ERROR, "pg_sdobSubmitCbBtnSelectJudge: element does not match any index\n");
  return true;

 found_it:
  dbgprintf(DBG_INFO, "pg_sdobSubmitCbBtnSelectJudge: selected judge #%d\n", jindex);
  mask = 1U<<jindex;
  bool prior_state = (sdob_submit_selected_judges_mask & mask) != 0;
  if (prior_state) {
    dbgprintf(DBG_INFO, "previously selected, turning off\n");
    sdob_submit_selected_judges_mask &= ~mask;
    gslc_ElemSetFillEn(pGui, pvElemRef, false);
  }
  else {
    dbgprintf(DBG_INFO, "previously clear, turning on\n");
    sdob_submit_selected_judges_mask |= mask;
    gslc_ElemSetFillEn(pGui, pvElemRef, true);
  }
  sdob_submit_update_judges_str();
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

  gslc_tsElemRef** eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BOX];

  // Create Fullscreen Draw Box
  // Must use a box so redrawing between pages functions correctly
  if ((*eRef = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
       ) != NULL) {
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, *eRef, &pg_sdobSubmitCbDraw);
  }

  /////////////////////
  // Page Defined Elements

  // Video File Text
  eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_VIDEOFILE];
  if ((*eRef = gslc_ElemCreateTxt
       (pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y, (rFullscreen.w - 148), 60},
        (char*)" ", 0, E_FONT_MONO18)
       ) != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, *eRef, true);
    gslc_ElemSetFrameEn(pGui, *eRef, false);
  }


  // Chapter
  eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_CHAPTER];
  if ((*eRef = gslc_ElemCreateTxt
       (pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect){(rFullscreen.x + rFullscreen.w) - 144, rFullscreen.y, 140, 60},
        (char*)" ", 0, E_FONT_MONO18)
       ) != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_ORANGE);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, *eRef, true);
    gslc_ElemSetFrameEn(pGui, *eRef, false);
  }

  // Score Totals Text
  eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_SCORE];
  if ((*eRef = gslc_ElemCreateTxt
       (pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect){(rFullscreen.x + (rFullscreen.w / 3)), rFullscreen.y + 65, (rFullscreen.w / 3), 60},
        (char*)" ", 0, E_FONT_MONO18)
       ) != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, *eRef, true);
    gslc_ElemSetFrameEn(pGui, *eRef, false);
  }

  // Team Number
  eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_TEAMNUM];
  if ((*eRef = gslc_ElemCreateTxt
       (pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y + 65, (rFullscreen.w / 3) - 8, 30},
        (char*)" ", 0, E_FONT_MONO18)
       ) != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, *eRef, true);
    gslc_ElemSetFrameEn(pGui, *eRef, false);
  }

    // Round Number
  eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_ROUNDNUM];
  if ((*eRef = gslc_ElemCreateTxt
       (pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y + 95, (rFullscreen.w / 3) - 8, 30},
        (char*)" ", 0, E_FONT_MONO18)
       ) != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, *eRef, true);
    gslc_ElemSetFrameEn(pGui, *eRef, false);
  }

  if (sdob_submit_num_judges >= 1) {

    int16_t y = rFullscreen.y + 130;
    uint16_t height = 30;
    int16_t base_x = rFullscreen.x + 4;
    uint16_t unitAllocWidth = (rFullscreen.w - 8) / (E_SDOB_SUBMIT_MAX_JUDGES + 2);
    uint16_t inter_unit_gap = 5;

    eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_TXT_JUDGES];
    if ((*eRef = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
                                    (gslc_tsRect){base_x, y, unitAllocWidth*2 - inter_unit_gap, height},
                                    (char*)"Judges:",0,E_FONT_MONO18))
        != NULL)
      {
        gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_GREEN);
        gslc_ElemSetCol(pGui, *eRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
        gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_LEFT);
        gslc_ElemSetFillEn(pGui, *eRef, true);
        gslc_ElemSetFrameEn(pGui, *eRef, false);
      }

    for(int j=0;j<sdob_submit_num_judges;++j) {
      eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_JUDGE0 + j];
      *eRef = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
                                    (gslc_tsRect){base_x + unitAllocWidth *(2 + j), y, unitAllocWidth - inter_unit_gap, height},
                                    sdob_submit_judges_list[j], 0, E_FONT_MONO14, &pg_sdobSubmitCbBtnSelectJudge);
      assert(*eRef);
      gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_WHITE);
      gslc_ElemSetCol(pGui, *eRef, GSLC_COL_RED, GSLC_COL_RED, GSLC_COL_BLACK);
      gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_MID);
      gslc_ElemSetFillEn(pGui, *eRef, false);
      gslc_ElemSetFrameEn(pGui, *eRef, true);
    }
  }

  // Cancel Button
  eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CANCEL];
  if ((*eRef = gslc_ElemCreateBtnTxt
       (pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
        "Cancel", 0, E_FONT_MONO14, &pg_sdobSubmitCbBtnCancel)
       ) != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_RED, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, *eRef, false);
    gslc_ElemSetFrameEn(pGui, *eRef, true);
  }

  // Clear Scorecard Button
  eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_CLEAR];
  if ((*eRef = gslc_ElemCreateBtnTxt
       (pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {(rFullscreen.x + rFullscreen.w) - 100, (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
        "Clear", 0, E_FONT_MONO14, &pg_sdobSubmitCbBtnClear)
       ) != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_PURPLE, GSLC_COL_PURPLE, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, *eRef, false);
    gslc_ElemSetFrameEn(pGui, *eRef, true);
  }

  // Submit Button
  eRef = &pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_BTN_SUBMIT];
  if ((*eRef = gslc_ElemCreateBtnTxt
       (pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rFullscreen.x + ((rFullscreen.w - 100) / 2), (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
        "Submit Score", 0, E_FONT_MONO14, &pg_sdobSubmitCbBtnSubmitScore)
       ) != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, *eRef, false);
    gslc_ElemSetFrameEn(pGui, *eRef, true);
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
  pg_sdob_submit_init_judges_list();

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
  pg_sdob_submit_destroy_judges_list();
}

// Setup Constructor
void __attribute__ ((constructor)) pg_sdobSubmit_setup(void) {
  cbInit[E_PG_SDOB_SUBMIT] = &pg_sdobSubmit_init;
  cbOpen[E_PG_SDOB_SUBMIT] = &pg_sdobSubmit_open;
  cbDestroy[E_PG_SDOB_SUBMIT] = &pg_sdobSubmit_destroy;
}
