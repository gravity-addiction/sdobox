#include "skydiveorbust_roundlist.h"

#include "buttons/buttons.h"
#include "queue/queue.h"
#include "gui/pages.h"

#include "gui/skydiveorbust/skydiveorbust.h"
#include "gui/keyboard/keyboard.h"

struct pg_sdob_round_info {
  char formatted_label_str[SDOB_NBUTTONS][128];
} round_info;

void pg_sdobRoundClose(gslc_tsGui *pGui) {
  touchscreenPageGoBack(pGui);
}

////////////////
// Button Callback
static bool pg_sdobRoundCbBtn(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {

  if (eTouch == GSLC_TOUCH_UP_IN) {

    int roundIndex = -1;
    for(int i=0;i<SDOB_NBUTTONS;++i) {
      if (pvElemRef == pg_sdobRoundEl[E_SDOB_ROUND_EL_BTN_0 + i]) {
        roundIndex = i;
        break;
      }
    }

    if (roundIndex < sdob_num_current_rounds)
      sdob_selectEventTeamRound(pvGui, roundIndex);

    pg_sdobRoundClose((gslc_tsGui*)pvGui);
  }
  return true;
}

//////////////////
// Box Drawing
static bool pg_sdobRoundCbDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);

  // "render" the strings:
  size_t nrounds = MIN((size_t)SDOB_NBUTTONS, sdob_num_current_rounds);
  size_t i=0;
  for(; i < nrounds; ++i)
    snprintf(round_info.formatted_label_str[i], sizeof(round_info.formatted_label_str[i]),
             "%s team %s, round %s",
             sdob_current_rounds[i].eventname,
             sdob_current_rounds[i].teamnumber,
             sdob_current_rounds[i].round);

  for(; i < SDOB_NBUTTONS; ++i)
    round_info.formatted_label_str[i][0] = '\0'; /* empty string */

  for(i=0;i<SDOB_NBUTTONS;++i)
    gslc_ElemSetTxtStr(pGui, pg_sdobRoundEl[E_SDOB_ROUND_EL_BTN_0+i], round_info.formatted_label_str[i]);

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}

/////////////////////
// Init Gui Elements
void pg_sdobRoundGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_SDOB_ROUNDLIST;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgSdobRoundElem, E_SDOB_ROUND_EL_MAX, m_asPgSdobRoundElemRef, E_SDOB_ROUND_EL_MAX);

  // Create Fullscreen Draw Box
  // Must use a box so redrawing between pages functions correctly
  if ((
    pg_sdobRoundEl[E_SDOB_ROUND_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_sdobRoundEl[E_SDOB_ROUND_EL_BOX], GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, pg_sdobRoundEl[E_SDOB_ROUND_EL_BOX], &pg_sdobRoundCbDraw);
  }

  // +4 on either end, divide the height by SDOB_NBUTTONS, and subtract, say, 5 pixels for gap betwen them all
  const unsigned inter_button_gap = 5;
  const unsigned nbuttons = SDOB_NBUTTONS;

  const unsigned bheight = (rFullscreen.h - (nbuttons + 1) * inter_button_gap) / nbuttons;
  const unsigned bwidth = rFullscreen.w - 8;

  unsigned y = inter_button_gap;
  for(int i=0;i<nbuttons;++i) {
    gslc_tsElemRef* elptr =
      pg_sdobRoundEl[E_SDOB_ROUND_EL_BTN_0 + i] =
      gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
                            (gslc_tsRect){4, y, bwidth, bheight},
                            (char*)" ", 0, E_FONT_MONO18, pg_sdobRoundCbBtn);

    gslc_ElemSetTxtCol(pGui, elptr, GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, elptr, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_GRAY);
    gslc_ElemSetTxtAlign(pGui, elptr, GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, elptr, true);
    gslc_ElemSetFrameEn(pGui, elptr, true);

    y += bheight + inter_button_gap;
  }
}

// GUI Init
void pg_sdobRound_init(gslc_tsGui *pGui) {
  pg_sdobRoundGuiInit(pGui);

  CLEAR(&round_info, sizeof(round_info));

  // Cleanup so Init is only ran once
  cbInit[E_PG_SDOB_ROUNDLIST] = NULL;
}


// GUI Open
void pg_sdobRound_open(gslc_tsGui *pGui) {
  // gslc_ElemSetFillEn(pGui, pg_sdobRoundEl[E_SDOB_ROUND_EL_BTN_CLEAR], false);
}

// GUI Destroy
void pg_sdobRound_destroy(gslc_tsGui *pGui) {
}

void pg_roundlist_show(gslc_tsGui *pGui) {
  touchscreenPageOpen(pGui, E_PG_SDOB_ROUNDLIST);
}

// Setup Constructor
void __attribute__ ((constructor)) pg_sdobRound_setup(void) {
  cbInit[E_PG_SDOB_ROUNDLIST] = &pg_sdobRound_init;
  cbOpen[E_PG_SDOB_ROUNDLIST] = &pg_sdobRound_open;
  cbDestroy[E_PG_SDOB_ROUNDLIST] = &pg_sdobRound_destroy;
}
