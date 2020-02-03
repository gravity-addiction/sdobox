#include <stdio.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "shared.h"
#include "wifi_list.h"

#include "gui/pages.h"
#include "gui/main/main.h"
#include "gui/keyboard/keyboard.h"






// Initialize Data
struct pg_wifi_list_data * PG_WIFI_LIST_INIT_DATA() {
  struct pg_wifi_list_data *data = (struct pg_wifi_list_data*)malloc(sizeof(struct pg_wifi_list_data));

  data->max = 32;
  data->len = 0;
  data->ptrs = (char**)malloc(data->max * sizeof(char*));

  data->scrollMax = 3;
  data->scroll = 0;
  return data;
}

// Clear Judgement Data and Scorecard Marks
void PG_WIFI_LIST_CLEAR_DATA(struct pg_wifi_list_data *data)
{
  for (int d = 0; d < data->len; ++d) {
    data->ptrs[d] = NULL;
  }
  data->len = 0;
  data->scrollMax = 1;
  data->scroll = 0;
}



void pg_wifi_list_kbPass(gslc_tsGui *pGui, char* str) {
  // gslc_ElemSetTxtStr(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], str);
  size_t strSz = snprintf(NULL, 0, "PASSWORD %d %s", pg_wifi_net_selected->network_id, str) + 1;
  char *strCmd = (char *)malloc(strSz * sizeof(char));
  snprintf(strCmd, strSz, "PASSWORD %d %s", pg_wifi_net_selected->network_id, str);
  printf("Sending %s\n", strCmd);
  pg_wifi_wpaSendCmd(strCmd);
  free(strCmd);
}


int pg_wifi_list_addNetworkList(pg_wifi_networkStruct *ptr) {
  if (pg_wifi_list_networkList->len >= pg_wifi_list_networkList->max) {
    pg_wifi_list_networkList->max = pg_wifi_list_networkList->len + 32;
    char **newPtrs = (char**)realloc(pg_wifi_list_networkList->ptrs, pg_wifi_list_networkList->max * sizeof(char*));
    pg_wifi_list_networkList->ptrs = newPtrs;
  }
  pg_wifi_list_networkList->ptrs[pg_wifi_list_networkList->len] = ptr;
  pg_wifi_list_networkList->len += 1;
}

void pg_wifi_list_setNetworkList(pg_wifi_networkStruct **ptrs, int len) {
  PG_WIFI_LIST_CLEAR_DATA(pg_wifi_list_networkList);
  pg_wifi_list_networkList->len = len;
  pg_wifi_list_networkList->ptrs = ptrs;
}

void pg_wifi_list_resetNetworkList() {
  PG_WIFI_LIST_CLEAR_DATA(pg_wifi_list_networkList);
}

//////////////////////////////////
//
// GUI CALLBACK BUTTONS
//

bool pg_wifi_list_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_WIFI);

  return true;
}

bool pg_wifi_list_cbBtn_connect(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_keyboard_show(pGui, 16, "", &pg_wifi_list_kbPass);
  return true;
}


bool pg_wifi_list_cbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
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

bool pg_wifi_list_cbBtn_sliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  printf("Up:\n");

  return true;
}
bool pg_wifi_list_cbBtn_sliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  printf("Down:\n");

  return true;
}








/////////////////////////////
//
// GUI DISPLAY FUNCTIONS
bool pg_wifi_list_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Start Drawing
  gslc_DrawFillRect(pGui,pRect,pElem->colElemFill);

printf("Listing Networks: %d\n", pg_wifi_list_networkList->len);
  for (int i = 0; i < 5; ++i) {
    if (i < pg_wifi_list_networkList->len) {
      gslc_ElemSetTxtStr(pGui, pg_wifiListEl[E_WIFI_LIST_EL_A + i], pg_wifi_list_networkList[i]->ssid);
    } else {
      gslc_ElemSetTxtStr(pGui, pg_wifiListEl[E_WIFI_LIST_EL_A + i], (char*)" ");
    }
  }

  // Stop Drawing
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}




void pg_wifi_list_sliderSetCurPos(gslc_tsGui *pGui, int slot_scroll) {
  pg_wifi_list_networkList->scroll = slot_scroll;
  gslc_ElemXSliderSetPos(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER], pg_wifi_list_networkList->scroll);
}

// Updated Scorecard Slider Position
void pg_wifi_list_sliderChangeCurPos(gslc_tsGui *pGui, int amt, bool redraw) {
  // Save Current Slider POS as i_slot_old
  int i_slot_old = gslc_ElemXSliderGetPos(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER]);
  i_slot_old = i_slot_old + amt;
  if (i_slot_old < 0) { i_slot_old = pg_wifi_list_networkList->scrollMax; }
  else if (i_slot_old > pg_wifi_list_networkList->scrollMax) { i_slot_old = 0; }

  // Set Slider Pos
  pg_wifi_list_sliderSetCurPos(pGui, i_slot_old);

  // Up scroller indicator
  if (pg_wifi_list_networkList->scroll == 0) {
    gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP], GSLC_COL_GRAY_LT1);
    gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  } else {
    gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }

  // Down Scroller indicator
  if (pg_wifi_list_networkList->scroll == pg_wifi_list_networkList->scrollMax) {
    gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN], GSLC_COL_GRAY_LT1);
    gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  } else {
    gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }

  if (redraw) {
    gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  }
}

void pg_wifi_list_sliderResetCurPos(gslc_tsGui *pGui) {
  int i_slot_old = gslc_ElemXSliderGetPos(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER]);
  pg_wifi_list_sliderChangeCurPos(&m_gui, (i_slot_old * -1), true);
}








// Init Gui
int pg_wifi_list_guiInit(gslc_tsGui *pGui)
{
  // debug_print("%s\n", "Wifi GUI Init");
  int ePage = E_PG_WIFI_LIST;
  gslc_PageAdd(pGui, ePage, pg_wifiListElem, MAX_ELEM_PG_WIFI_LIST_RAM, pg_wifiListElemRef, MAX_ELEM_PG_WIFI_LIST);

  // Set Background to a flat color
  gslc_SetBkgndColor(pGui, GSLC_COL_BLACK);

  gslc_tsRect rListBox = {0,3,425,260};

  // Main View Box
  pg_wifiListEl[E_WIFI_LIST_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rListBox);
  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], &pg_wifi_list_cbDrawBox);




  int xHei = 50;
  pg_wifiListEl[E_WIFI_LIST_EL_A] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (0 * xHei)), rListBox.w, xHei},
          (char*)"000", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_A], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_A], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListEl[E_WIFI_LIST_EL_A], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_A], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_A], true);

  pg_wifiListEl[E_WIFI_LIST_EL_B] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (1 * xHei)), rListBox.w, xHei},
          (char*)"111", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_B], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_B], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListEl[E_WIFI_LIST_EL_B], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_B], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_B], true);

  pg_wifiListEl[E_WIFI_LIST_EL_C] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (2 * xHei)), rListBox.w, xHei},
          (char*)"222", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_C], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_C], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListEl[E_WIFI_LIST_EL_C], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_C], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_C], true);

  pg_wifiListEl[E_WIFI_LIST_EL_D] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (3 * xHei)), rListBox.w, xHei},
          (char*)"333", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_D], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_D], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListEl[E_WIFI_LIST_EL_D], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_D], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_D], true);

  pg_wifiListEl[E_WIFI_LIST_EL_E] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (4 * xHei)), rListBox.w, xHei},
          (char*)"444", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_E], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_E], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiListEl[E_WIFI_LIST_EL_E], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_E], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_E], true);


  //////////////////////////////////////////
  // Create vertical scrollbar
  pg_wifiListEl[E_WIFI_LIST_EL_SLIDER] = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO,
      ePage, &pg_wifi_list_slider, (gslc_tsRect){(rListBox.x + rListBox.w) + 5, rListBox.y + 35, rFullscreen.w - (rListBox.x + rListBox.w) - 5, rListBox.h - 70},
      0, pg_wifi_list_networkList->scrollMax, 0, 10, true);

  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER], GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER], true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER], &pg_wifi_list_cbSlidePos);

  //////////////////////////////////////////
  // Create vertical scrollbar Up Arrow
  pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 5, rListBox.y, rFullscreen.w - (rListBox.x + rListBox.w) - 5, 30}, (char*)"^",
      0, E_FONT_MONO18, &pg_wifi_list_cbBtn_sliderUp);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP], GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP], true);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_UP], GSLC_ALIGN_MID_MID);

  //////////////////////////////////////////
  // Create vertical scrollbar Down Arrow
  pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 5, (rListBox.y + rListBox.h) - 30, rFullscreen.w - (rListBox.x + rListBox.w) - 5, 30}, (char*)"v",
      0, E_FONT_MONO18, &pg_wifi_list_cbBtn_sliderDown);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN], GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN], true);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SLIDER_DOWN], GSLC_ALIGN_MID_MID);





  // Select
  pg_wifiListEl[E_WIFI_LIST_EL_CONNECT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x + 10), (rFullscreen.y + rFullscreen.h - 55), (rFullscreen.w - 120), 50},
          "Select Wifi", 0, E_FONT_MONO14, &pg_wifi_list_cbBtn_connect);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], GSLC_COL_WHITE, GSLC_COL_GREEN, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], true);


  // Close Key
  pg_wifiListEl[E_WIFI_LIST_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 100),((rFullscreen.y + rFullscreen.h) - 55),100,50},
          "Close", 0, E_FONT_MONO14, &pg_wifi_list_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CLOSE], true);



  return 1;
}





// GUI Init
void pg_wifi_list_init(gslc_tsGui *pGui) {
  // Initialize Network list
  pg_wifi_list_networkList = PG_WIFI_LIST_INIT_DATA();
  
  pg_wifi_updateAvailableNetworks();
  pg_wifi_list_setNetworkList(pg_wifi_nets_available->ptrs, pg_wifi_nets_available->len);

  // Create Interface
  pg_wifi_list_guiInit(pGui);

  cbInit[E_PG_WIFI_LIST] = NULL;
}


// GUI Open
void pg_wifi_list_open(gslc_tsGui *pGui) {
  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
}

// GUI Close
void pg_wifi_list_close(gslc_tsGui *pGui) {

}

// GUI Destroy
void pg_wifi_list_destroy() {

}

void __attribute__ ((constructor)) pg_wifi_list_setup(void) {
  cbInit[E_PG_WIFI_LIST] = &pg_wifi_list_init;
  cbOpen[E_PG_WIFI_LIST] = &pg_wifi_list_open;
  cbThread[E_PG_WIFI_LIST] = NULL;
  cbClose[E_PG_WIFI_LIST] = &pg_wifi_list_close;
  cbDestroy[E_PG_WIFI_LIST] = &pg_wifi_list_destroy;
}
