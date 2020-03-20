#include <stdio.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "libs/shared.h"
#include "wifi_list_saved.h"

#include "gui/pages.h"
#include "gui/main/main.h"
#include "gui/keyboard/keyboard.h"






// Initialize Data
struct pg_wifi_list_saved_data * PG_WIFI_LIST_SAVED_INIT_DATA() {
  struct pg_wifi_list_saved_data *data = (struct pg_wifi_list_saved_data*)malloc(sizeof(struct pg_wifi_list_saved_data));

  data->max = 32;
  data->len = 0;
  data->cur = -1;
  data->ptrs = (struct pg_wifi_networkStruct**)malloc(data->max * sizeof(struct pg_wifi_networkStruct*));

  data->scrollMax = 3;
  data->scroll = 0;
  return data;
}

// Clear Judgement Data and Scorecard Marks
void PG_WIFI_LIST_SAVED_CLEAR_DATA(struct pg_wifi_list_saved_data *data)
{
  for (int d = 0; d < data->len; ++d) {
    data->ptrs[d] = NULL;
  }
  data->len = 0;
  data->cur = -1;
  data->scrollMax = 1;
  data->scroll = 0;
}





int pg_wifi_list_saved_addNetworkList(struct pg_wifi_networkStruct *ptr) {
  if (pg_wifi_list_saved_networkList->len >= pg_wifi_list_saved_networkList->max) {
    pg_wifi_list_saved_networkList->max = pg_wifi_list_saved_networkList->len + 32;
    struct pg_wifi_networkStruct **newPtrs = (struct pg_wifi_networkStruct**)realloc(pg_wifi_list_saved_networkList->ptrs, pg_wifi_list_saved_networkList->max * sizeof(struct pg_wifi_networkStruct*));
    pg_wifi_list_saved_networkList->ptrs = newPtrs;
  }
  pg_wifi_list_saved_networkList->ptrs[pg_wifi_list_saved_networkList->len] = ptr;
  pg_wifi_list_saved_networkList->len += 1;
  return (pg_wifi_list_saved_networkList->len - 1);
}

void pg_wifi_list_saved_setNetworkList(struct pg_wifi_networkStruct **ptrs, int len) {
  // PG_WIFI_LIST_SAVED_CLEAR_DATA(pg_wifi_list_saved_networkList);
  pg_wifi_list_saved_networkList->len = len;
  pg_wifi_list_saved_networkList->ptrs = ptrs;


}

void pg_wifi_list_saved_resetNetworkList() {
  PG_WIFI_LIST_SAVED_CLEAR_DATA(pg_wifi_list_saved_networkList);
}



void pg_wifi_list_saved_wpaEvent(char* event) {
  printf("Got Event: --%s--\n", event);
  /*if (strcmp(event, "CTRL-EVENT-SCAN-STARTED") == 0) {
    // debug_print("%s\n", "Scan Started!");
  } else if (strcmp(event, "CTRL-EVENT-SCAN-RESULTS") == 0) {
    pg_wifi_updateSavedNetworks();
    pg_wifi_list_saved_setNetworkList(pg_wifi_nets_saved->ptrs, pg_wifi_nets_saved->len);
  }
  */
}




//////////////////////////////////
//
// GUI CALLBACK BUTTONS
//
bool pg_wifi_list_saved_cbBtn_elA(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_list_saved_networkList->cur = 0 + (pg_wifi_list_saved_networkList->scroll * 5);
  if (pg_wifi_list_saved_networkList->cur >= pg_wifi_list_saved_networkList->len) { pg_wifi_list_saved_networkList->cur = -1; }
  gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_wifi_list_saved_cbBtn_elB(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_list_saved_networkList->cur = 1 + (pg_wifi_list_saved_networkList->scroll * 5);
  if (pg_wifi_list_saved_networkList->cur >= pg_wifi_list_saved_networkList->len) { pg_wifi_list_saved_networkList->cur = -1; }
  gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_wifi_list_saved_cbBtn_elC(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_list_saved_networkList->cur = 2 + (pg_wifi_list_saved_networkList->scroll * 5);
  if (pg_wifi_list_saved_networkList->cur >= pg_wifi_list_saved_networkList->len) { pg_wifi_list_saved_networkList->cur = -1; }
  gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_wifi_list_saved_cbBtn_elD(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_list_saved_networkList->cur = 3 + (pg_wifi_list_saved_networkList->scroll * 5);
  if (pg_wifi_list_saved_networkList->cur >= pg_wifi_list_saved_networkList->len) { pg_wifi_list_saved_networkList->cur = -1; }
  gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_wifi_list_saved_cbBtn_elE(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_list_saved_networkList->cur = 4 + (pg_wifi_list_saved_networkList->scroll * 5);
  if (pg_wifi_list_saved_networkList->cur >= pg_wifi_list_saved_networkList->len) { pg_wifi_list_saved_networkList->cur = -1; }
  gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}


bool pg_wifi_list_saved_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageGoBack(pGui);

  return true;
}



bool pg_wifi_list_saved_cbBtn_connect(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (pg_wifi_list_saved_networkList->cur > -1) {
    pg_wifi_net_selected = pg_wifi_list_saved_networkList->ptrs[pg_wifi_list_saved_networkList->cur];
    printf("Selected: %d - %s\n", pg_wifi_list_saved_networkList->cur, pg_wifi_net_selected->ssid);
  } else {
    pg_wifi_net_selected = NULL;
  }

  touchscreenPageGoBack(pGui);
  return true;
}


bool pg_wifi_list_saved_cbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider* pSlider   = (gslc_tsXSlider*)(pElem->pXData);

  // // debug_print("Slider Ref: %d,  %d\n", pElem->nId, pSlider->eTouch);
  // Fetch the new RGB component from the slider
  switch (pElem->nId) {
    case E_SLIDER_SCORECARD:
      // Moved by Touchscreen, Force Redraw
      if (pSlider->eTouch == GSLC_TOUCH_DOWN_IN ||
          pSlider->eTouch == GSLC_TOUCH_MOVE_IN ||
          pSlider->eTouch == GSLC_TOUCH_MOVE_OUT
      ) {
        printf("Scroll: %d\n", gslc_ElemXSliderGetPos(pGui,pElemRef));
      }
    default:
      break;
  }
  return true;
}

bool pg_wifi_list_saved_cbBtn_sliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  printf("Up:\n");
  pg_wifi_list_saved_networkList->scroll -= 1;
  if (pg_wifi_list_saved_networkList->scroll < 0) {  pg_wifi_list_saved_networkList->scroll = 0; }
  gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}
bool pg_wifi_list_saved_cbBtn_sliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  printf("Down:\n");
  if (pg_wifi_list_saved_networkList->scroll > pg_wifi_list_saved_networkList->scrollMax) { pg_wifi_list_saved_networkList->scroll = pg_wifi_list_saved_networkList->scrollMax; }
  gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}








/////////////////////////////
//
// GUI DISPLAY FUNCTIONS
bool pg_wifi_list_saved_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Start Drawing
  gslc_DrawFillRect(pGui,pRect,pElem->colElemFill);

  int pgAdd = pg_wifi_list_saved_networkList->scroll * 5;
  printf("Listing Networks: %d + %d\n", pg_wifi_list_saved_networkList->len, pgAdd);

  for (int i = 0; i < 5; ++i) {
    // Fill Display Line
    if (i < pg_wifi_list_saved_networkList->len) {
      size_t newStrSz = snprintf(NULL, 0, "%d %s", pg_wifi_list_saved_networkList->ptrs[i + pgAdd]->id, pg_wifi_list_saved_networkList->ptrs[i + pgAdd]->ssid) + 1;
      char *newStr = (char*)malloc(newStrSz * sizeof(char));
      snprintf(newStr, newStrSz, "%d %s",  pg_wifi_list_saved_networkList->ptrs[i + pgAdd]->id, pg_wifi_list_saved_networkList->ptrs[i + pgAdd]->ssid);
      gslc_ElemSetTxtStr(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A + i], newStr);
      free(newStr);
    } else {
      gslc_ElemSetTxtStr(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A + i], (char*)" ");
    }

    // Fill Selected
    if ((i + pgAdd) == pg_wifi_list_saved_networkList->cur) {
      gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A + i], true);
    } else {
      gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A + i], false);
    }
  }

  // Stop Drawing
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}




void pg_wifi_list_saved_sliderSetCurPos(gslc_tsGui *pGui, int slot_scroll) {
  pg_wifi_list_saved_networkList->scroll = slot_scroll;
  gslc_ElemXSliderSetPos(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER], pg_wifi_list_saved_networkList->scroll);
}

// Updated Scorecard Slider Position
void pg_wifi_list_saved_sliderChangeCurPos(gslc_tsGui *pGui, int amt, bool redraw) {
  // Save Current Slider POS as i_slot_old
  int i_slot_old = gslc_ElemXSliderGetPos(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER]);
  i_slot_old = i_slot_old + amt;
  if (i_slot_old < 0) { i_slot_old = pg_wifi_list_saved_networkList->scrollMax; }
  else if (i_slot_old > pg_wifi_list_saved_networkList->scrollMax) { i_slot_old = 0; }

  // Set Slider Pos
  pg_wifi_list_saved_sliderSetCurPos(pGui, i_slot_old);

  // Up scroller indicator
  if (pg_wifi_list_saved_networkList->scroll == 0) {
    gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP], GSLC_COL_GRAY_LT1);
    gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  } else {
    gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }

  // Down Scroller indicator
  if (pg_wifi_list_saved_networkList->scroll == pg_wifi_list_saved_networkList->scrollMax) {
    gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN], GSLC_COL_GRAY_LT1);
    gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  } else {
    gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }

  if (redraw) {
    gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);
  }
}

void pg_wifi_list_saved_sliderResetCurPos(gslc_tsGui *pGui) {
  int i_slot_old = gslc_ElemXSliderGetPos(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER]);
  pg_wifi_list_saved_sliderChangeCurPos(&m_gui, (i_slot_old * -1), true);
}








// Init Gui
int pg_wifi_list_saved_guiInit(gslc_tsGui *pGui)
{
  // debug_print("%s\n", "Wifi GUI Init");
  int ePage = E_PG_WIFI_LIST_SAVED;
  gslc_PageAdd(pGui, ePage, pg_wifiListSavedElem, MAX_ELEM_PG_WIFI_LIST_SAVED_RAM, pg_wifiListSavedElemRef, MAX_ELEM_PG_WIFI_LIST_SAVED);

  gslc_tsRect rListBox = {0,5,425,250};

  // Main View Box
  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rListBox);
  gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], &pg_wifi_list_saved_cbDrawBox);




  int xHei = 50;
  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (0 * xHei)), rListBox.w, xHei},
          (char*)"000", 0, E_FONT_MONO18, &pg_wifi_list_saved_cbBtn_elA);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_A], true);

  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_B] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (1 * xHei)), rListBox.w, xHei},
          (char*)"111", 0, E_FONT_MONO18, &pg_wifi_list_saved_cbBtn_elB);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_B], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_B], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_B], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_B], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_B], true);

  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_C] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (2 * xHei)), rListBox.w, xHei},
          (char*)"222", 0, E_FONT_MONO18, &pg_wifi_list_saved_cbBtn_elC);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_C], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_C], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_C], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_C], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_C], true);

  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_D] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (3 * xHei)), rListBox.w, xHei},
          (char*)"333", 0, E_FONT_MONO18, &pg_wifi_list_saved_cbBtn_elD);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_D], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_D], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_D], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_D], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_D], true);

  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_E] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (4 * xHei)), rListBox.w, xHei},
          (char*)"444", 0, E_FONT_MONO18, &pg_wifi_list_saved_cbBtn_elE);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_E], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_E], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_E], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_E], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_E], true);


  //////////////////////////////////////////
  // Create vertical scrollbar
  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER] = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO,
      ePage, &pg_wifi_list_saved_slider, (gslc_tsRect){(rListBox.x + rListBox.w) + 5, rListBox.y + 35, rFullscreen.w - (rListBox.x + rListBox.w) - 5, rListBox.h - 70},
      0, pg_wifi_list_saved_networkList->scrollMax, 0, 10, true);

  gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER], GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER], true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER], &pg_wifi_list_saved_cbSlidePos);

  //////////////////////////////////////////
  // Create vertical scrollbar Up Arrow
  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 5, rListBox.y, rFullscreen.w - (rListBox.x + rListBox.w) - 5, 30}, (char*)"^",
      0, E_FONT_MONO18, &pg_wifi_list_saved_cbBtn_sliderUp);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP], GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP], true);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_UP], GSLC_ALIGN_MID_MID);

  //////////////////////////////////////////
  // Create vertical scrollbar Down Arrow
  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 5, (rListBox.y + rListBox.h) - 30, rFullscreen.w - (rListBox.x + rListBox.w) - 5, 30}, (char*)"v",
      0, E_FONT_MONO18, &pg_wifi_list_saved_cbBtn_sliderDown);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN], GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN], true);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SLIDER_DOWN], GSLC_ALIGN_MID_MID);





  // Open Saved WIFI
  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CONNECT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x + 10), (rFullscreen.y + rFullscreen.h - 55), (rFullscreen.w - 230), 50},
          "Open", 0, E_FONT_MONO14, &pg_wifi_list_saved_cbBtn_connect);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CONNECT], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CONNECT], GSLC_COL_WHITE, GSLC_COL_GREEN, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CONNECT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CONNECT], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CONNECT], true);
/*
  // Refresh Key
  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SCAN] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 210),((rFullscreen.y + rFullscreen.h) - 55),100,50},
          "Scan", 0, E_FONT_MONO14);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SCAN], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SCAN], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SCAN], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SCAN], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_SCAN], true);
*/
  // Close Key
  pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 100),((rFullscreen.y + rFullscreen.h) - 55),100,50},
          "Close", 0, E_FONT_MONO14, &pg_wifi_list_saved_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_CLOSE], true);



  return 1;
}






// GUI Init
void pg_wifi_list_saved_init(gslc_tsGui *pGui) {
  // Initialize Network list
  pg_wifi_list_saved_networkList = PG_WIFI_LIST_SAVED_INIT_DATA();

  // Create Interface
  pg_wifi_list_saved_guiInit(pGui);

  cbInit[E_PG_WIFI_LIST_SAVED] = NULL;
}


// GUI Open
void pg_wifi_list_saved_open(gslc_tsGui *pGui) {

  pg_wifi_updateSavedNetworks();
  pg_wifi_list_saved_setNetworkList(pg_wifi_nets_saved->ptrs, pg_wifi_nets_saved->len);

  gslc_ElemSetRedraw(pGui, pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_BOX], GSLC_REDRAW_FULL);
}

// GUI Close
void pg_wifi_list_saved_close(gslc_tsGui *pGui) {

}

// GUI Destroy
void pg_wifi_list_saved_destroy() {
  free(pg_wifi_list_saved_networkList->ptrs);
  free(pg_wifi_list_saved_networkList);
}

void __attribute__ ((constructor)) pg_wifi_list_saved_setup(void) {
  cbInit[E_PG_WIFI_LIST_SAVED] = &pg_wifi_list_saved_init;
  cbOpen[E_PG_WIFI_LIST_SAVED] = &pg_wifi_list_saved_open;
  cbThread[E_PG_WIFI_LIST_SAVED] = NULL;
  cbClose[E_PG_WIFI_LIST_SAVED] = &pg_wifi_list_saved_close;
  cbDestroy[E_PG_WIFI_LIST_SAVED] = &pg_wifi_list_saved_destroy;
}
