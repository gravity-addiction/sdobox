#include <sys/stat.h>
#include <assert.h>

#include "skydiveorbust_disciplinelist.h"

#include "libs/shared.h"

#include "libs/buttons/buttons.h"
// #include "libs/mpv2/mpv2.h"
#include "libs/queue/queue.h"
#include "gui/pages.h"
#include "libs/vlisting/vlisting.h"
#include "libs/dbg/dbg.h"

#include "gui/skydiveorbust/skydiveorbust.h"
#include "gui/keyboard/keyboard.h"


static void refreshDisciplineList(gslc_tsGui *pGui) {
  pg_sdobDisciplineList_loadDisciplines(pGui);
}

void pg_sdobDisciplineListClose(gslc_tsGui *pGui) {
  touchscreenPageGoBack(pGui);
}


////////////////
// Button Callback

bool pg_sdobDisciplineListCbBtnCancel(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Close Menu
  pg_sdobDisciplineListClose(pGui);
  return true;
}

bool pg_sdobDisciplineListCbBtnChangeDiscipline(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (pg_sdobDiscipline_listConfig->cur >= 0 && sdob_judgement != NULL) {
    pg_sdobUpdateScoringSettings(pGui, pg_sdobDiscipline_list[pg_sdobDiscipline_listConfig->cur].ident);
    pg_sdobUpdateVideoDescTwo(pGui, pg_sdobDiscipline_list[pg_sdobDiscipline_listConfig->cur].name);

    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_CLEAN;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    // Close Menu
    pg_sdobDisciplineListClose(pGui);
  }

  return true;
}




//////////////////
// Box Drawing
bool pg_sdobDisciplinelist_cbDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);

  // Generate list of items based on default list info
  char **list = (char**)malloc(pg_sdobDiscipline_listConfig->len * sizeof(char*));
  for (int l = 0; l < pg_sdobDiscipline_listConfig->len; ++l) {
    size_t nameSz = snprintf(NULL, 0, "%s", pg_sdobDiscipline_list[l].name) + 1;
    list[l] = (char *)malloc(nameSz * sizeof(char));
    snprintf(list[l], nameSz, "%s", pg_sdobDiscipline_list[l].name);
  }

  // Use new List
  vlist_sliderDraw(pGui, pg_sdobDiscipline_listConfig, list, 29);

  // Clean list
  for (int l = 0; l < pg_sdobDiscipline_listConfig->len; ++l) {
    free(list[l]);
  }
  free(list);


  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}


// A
bool pg_sdobDisciplinelist_cbBtn_elA(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobDiscipline_listConfig, 0)) { return true; }
  // pg_sdobDisciplineList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// B
bool pg_sdobDisciplinelist_cbBtn_elB(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobDiscipline_listConfig, 1)) { return true; }
  // pg_sdobDisciplineList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// C
bool pg_sdobDisciplinelist_cbBtn_elC(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobDiscipline_listConfig, 2)) { return true; }
  // pg_sdobDisciplineList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// D
bool pg_sdobDisciplinelist_cbBtn_elD(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobDiscipline_listConfig, 3)) { return true; }
  // pg_sdobDisciplineList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// E
bool pg_sdobDisciplinelist_cbBtn_elE(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobDiscipline_listConfig, 4)) { return true; }
  // pg_sdobDisciplineList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_sdobDisciplinelist_cbBtn_sliderPos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsXSlider* pSlider   = (gslc_tsXSlider*)(pElem->pXData);

  // Fetch the new RGB component from the slider
  if (pSlider->eTouch == GSLC_TOUCH_DOWN_IN ||
      pSlider->eTouch == GSLC_TOUCH_MOVE_IN ||
      pSlider->eTouch == GSLC_TOUCH_MOVE_OUT
  ) {
    // Set slider config
    vlist_sliderSetPos(pGui, pg_sdobDiscipline_listConfig, gslc_ElemXSliderGetPos(pGui, pElemRef));
    // Update Visual List
    gslc_ElemSetRedraw(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_REDRAW_FULL);
  }
  return true;
}

bool pg_sdobDisciplinelist_cbBtn_sliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_sdobDiscipline_listConfig, -1);
  gslc_ElemSetRedraw(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

bool pg_sdobDisciplinelist_cbBtn_sliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_sdobDiscipline_listConfig, 1);
  gslc_ElemSetRedraw(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

/////////////////////
// Init Gui Elements
void pg_sdobDisciplineListGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_SDOB_DISCIPLINELIST;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgSdobDisciplinelistElem, MAX_ELEM_SDOB_DISCIPLINELIST, m_asPgSdobDisciplinelistElemRef, MAX_ELEM_SDOB_DISCIPLINELIST);

  // Create Fullscreen Draw Box
  // Must use a box so redrawing between pages functions correctly
  if ((
    pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], &pg_sdobDisciplinelist_cbDraw);
  }



  /////////////////////
  // Page Defined Elements

  // Add Text Element
  if ((
    pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_TXT_TMP] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y, (rFullscreen.w - 108), 60},
          (char*)" ", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_TXT_TMP], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_TXT_TMP], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_TXT_TMP], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_TXT_TMP], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_TXT_TMP], false);
  }


  int xHei = 40;
  gslc_tsRect rListBox = {0,40,420,210};
  // Main View Box
  pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rListBox);
  gslc_ElemSetCol(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BOX], &pg_sdobDisciplinelist_cbDraw);

  // Button A
  pg_sdobDiscipline_listConfig->refs[0] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (0 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobDisciplinelist_cbBtn_elA);
  gslc_ElemSetTxtCol(pGui, pg_sdobDiscipline_listConfig->refs[0], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobDiscipline_listConfig->refs[0], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobDiscipline_listConfig->refs[0], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobDiscipline_listConfig->refs[0], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobDiscipline_listConfig->refs[0], true);

  // Button B
  pg_sdobDiscipline_listConfig->refs[1] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (1 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobDisciplinelist_cbBtn_elB);
  gslc_ElemSetTxtCol(pGui, pg_sdobDiscipline_listConfig->refs[1], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobDiscipline_listConfig->refs[1], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobDiscipline_listConfig->refs[1], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobDiscipline_listConfig->refs[1], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobDiscipline_listConfig->refs[1], true);

  // Button C
  pg_sdobDiscipline_listConfig->refs[2] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (2 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobDisciplinelist_cbBtn_elC);
  gslc_ElemSetTxtCol(pGui, pg_sdobDiscipline_listConfig->refs[2], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobDiscipline_listConfig->refs[2], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobDiscipline_listConfig->refs[2], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobDiscipline_listConfig->refs[2], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobDiscipline_listConfig->refs[2], true);

  // Button D
  pg_sdobDiscipline_listConfig->refs[3] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (3 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobDisciplinelist_cbBtn_elD);
  gslc_ElemSetTxtCol(pGui, pg_sdobDiscipline_listConfig->refs[3], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobDiscipline_listConfig->refs[3], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobDiscipline_listConfig->refs[3], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobDiscipline_listConfig->refs[3], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobDiscipline_listConfig->refs[3], true);

  // Button E
  pg_sdobDiscipline_listConfig->refs[4] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (4 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobDisciplinelist_cbBtn_elE);
  gslc_ElemSetTxtCol(pGui, pg_sdobDiscipline_listConfig->refs[4], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobDiscipline_listConfig->refs[4], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobDiscipline_listConfig->refs[4], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobDiscipline_listConfig->refs[4], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobDiscipline_listConfig->refs[4], true);


  //////////////////////////////////////////
  // Create vertical scrollbar
  pg_sdobDiscipline_listConfig->sliderEl = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO,
      ePage, &pg_sdobDiscipline_listSlider, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, rListBox.y + 55, rFullscreen.w - (rListBox.x + rListBox.w) - 2, rListBox.h - 110},
      0, pg_sdobDiscipline_listConfig->scrollMax, 0, 10, true);
  pg_sdobDiscipline_listConfig->slider = &pg_sdobDiscipline_listSlider; // Assign to listConfig for later access

  gslc_ElemSetCol(pGui, pg_sdobDiscipline_listConfig->sliderEl, GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pGui, pg_sdobDiscipline_listConfig->sliderEl, true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(pGui, pg_sdobDiscipline_listConfig->sliderEl, &pg_sdobDisciplinelist_cbBtn_sliderPos);



  //////////////////////////////////////////
  // Create vertical scrollbar Up Arrow
  pg_sdobDiscipline_listConfig->sliderUpEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, rListBox.y, rFullscreen.w - (rListBox.x + rListBox.w) - 2, 50}, (char*)"^",
      0, E_FONT_MONO18, &pg_sdobDisciplinelist_cbBtn_sliderUp);
  gslc_ElemSetTxtCol(pGui, pg_sdobDiscipline_listConfig->sliderUpEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_sdobDiscipline_listConfig->sliderUpEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_sdobDiscipline_listConfig->sliderUpEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_sdobDiscipline_listConfig->sliderUpEl, GSLC_ALIGN_MID_MID);


  //////////////////////////////////////////
  // Create vertical scrollbar Down Arrow
  pg_sdobDiscipline_listConfig->sliderDownEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, (rListBox.y + rListBox.h) - 50, rFullscreen.w - (rListBox.x + rListBox.w) - 2, 50}, (char*)"v",
      0, E_FONT_MONO18, &pg_sdobDisciplinelist_cbBtn_sliderDown);
  gslc_ElemSetTxtCol(pGui, pg_sdobDiscipline_listConfig->sliderDownEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_sdobDiscipline_listConfig->sliderDownEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_sdobDiscipline_listConfig->sliderDownEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_sdobDiscipline_listConfig->sliderDownEl, GSLC_ALIGN_MID_MID);
  pg_sdobDiscipline_listConfig->sliderDownEl = pg_sdobDiscipline_listConfig->sliderDownEl;




  // Cancel Button
  if ((
    pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CANCEL] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
            "Close", 0, E_FONT_MONO14, &pg_sdobDisciplineListCbBtnCancel)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CANCEL], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CANCEL], GSLC_COL_RED, GSLC_COL_BLACK, GSLC_COL_RED);
    gslc_ElemSetTxtAlign(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CANCEL], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CANCEL], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CANCEL], true);
  }

  // Change Button
  if ((
    pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CHANGE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x + (rFullscreen.w / 3), (rFullscreen.y + rFullscreen.h) - 60, (rFullscreen.w / 3), 60},
            "Load Discipline", 0, E_FONT_MONO14, &pg_sdobDisciplineListCbBtnChangeDiscipline)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CHANGE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CHANGE], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_GREEN);
    gslc_ElemSetTxtAlign(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CHANGE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CHANGE], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_CHANGE], true);
  }


}


void pg_sdobDisciplineListButtonRotaryCW() {
  // debug_print("%s\n", "SDOB Videolist CW");
}
void pg_sdobDisciplineListButtonRotaryCCW() {
  // debug_print("%s\n", "SDOB Videolist CCW");
}
void pg_sdobDisciplineListButtonLeftPressed() {
  // debug_print("%s\n", "SDOB Videolist Left Pressed");
}
void pg_sdobDisciplineListButtonRightPressed() {
  // debug_print("%s\n", "SDOB Videolist Right Pressed");
}
void pg_sdobDisciplineListButtonRotaryPressed() {
  // debug_print("%s\n", "SDOB Videolist Rotary Pressed");
}
void pg_sdobDisciplineListButtonLeftHeld() {
  // debug_print("%s\n", "SDOB Videolist Left Held");
}
void pg_sdobDisciplineListButtonRightHeld() {
  // debug_print("%s\n", "SDOB Videolist Right Held");
}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_sdobDisciplineListButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_sdobDisciplineListButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_sdobDisciplineListButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_sdobDisciplineListButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_sdobDisciplineListButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_sdobDisciplineListButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_sdobDisciplineListButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_sdobDisciplineListButtonRightHeld);
}

void pg_sdobDisciplineList_free_list() {
  for (int i = 0; i < pg_sdobDiscipline_listConfig->len; i++) {
    free(pg_sdobDiscipline_list[i].name);
    free(pg_sdobDiscipline_list[i].ident);
    // free(pg_sdobDiscipline_list[i]);
  }
}

void pg_sdobDisciplineList_loadDisciplines(gslc_tsGui *pGui) {
  if (pg_sdobDiscipline_listConfig->len > 0) {
    pg_sdobDisciplineList_free_list();
    free(pg_sdobDiscipline_list);
  }

  // VLIST_CLEAR_CONFIG(pg_sdobDiscipline_listConfig);

  pg_sdobDiscipline_listConfig->len = 4;
  pg_sdobDiscipline_list = (struct pg_sdobDisciplineStruct*)malloc(pg_sdobDiscipline_listConfig->len * sizeof(struct pg_sdobDisciplineStruct));
  
  pg_sdobDiscipline_list[0].name = strdup("2-way Sequential");
  pg_sdobDiscipline_list[0].ident = strdup("CF2");

  pg_sdobDiscipline_list[1].name = strdup("4-way Sequential");
  pg_sdobDiscipline_list[1].ident = strdup("CF4");

  pg_sdobDiscipline_list[2].name = strdup("4-way Rotes");
  pg_sdobDiscipline_list[2].ident = strdup("CF4R");

  pg_sdobDiscipline_list[3].name = strdup("FS");
  pg_sdobDiscipline_list[3].ident = strdup("FS");

  // INIT_SDOB_DISCIPLINESTRUCT("2-way Sequential", "CF2", pg_sdobDiscipline_list[0]);
  // INIT_SDOB_DISCIPLINESTRUCT("4-way Sequential", "CF4", pg_sdobDiscipline_list[1]);
  // INIT_SDOB_DISCIPLINESTRUCT("4-way Rotes", "CF4R", pg_sdobDiscipline_list[2]);
  // INIT_SDOB_DISCIPLINESTRUCT("FS", "FS", pg_sdobDiscipline_list[3]);

  VLIST_UPDATE_CONFIG(pg_sdobDiscipline_listConfig);
  vlist_sliderUpdate(pGui, pg_sdobDiscipline_listConfig);

}

// GUI Init
void pg_sdobDisciplineList_init(gslc_tsGui *pGui) {
  pg_sdobDiscipline_listConfig = VLIST_INIT_CONFIG(5, 32);

  pg_sdobDisciplineListGuiInit(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_SDOB_DISCIPLINELIST] = NULL;
}


// GUI Open
void pg_sdobDisciplineList_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_sdobDisciplineListButtonSetFuncs();

  refreshDisciplineList(pGui);

  gslc_ElemSetTxtStr(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_TXT_TMP], "Discipline List");
  // gslc_ElemSetTxtStr(pGui, pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_BTN_FOLDER], sdob_judgement->video->local_folder);
}


// GUI Destroy
void pg_sdobDisciplineList_destroy(gslc_tsGui *pGui) {
  // pg_sdobDisciplineList_free_filelist();
}

// Setup Constructor
void __attribute__ ((constructor)) pg_sdobDisciplineList_setup(void) {
  cbInit[E_PG_SDOB_DISCIPLINELIST] = &pg_sdobDisciplineList_init;
  cbOpen[E_PG_SDOB_DISCIPLINELIST] = &pg_sdobDisciplineList_open;
  cbDestroy[E_PG_SDOB_DISCIPLINELIST] = &pg_sdobDisciplineList_destroy;
}
