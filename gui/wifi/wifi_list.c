#include <stdio.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "shared.h"
#include "wifi_list.h"

#include "gui/pages.h"
#include "gui/main/main.h"
#include "gui/keyboard/keyboard.h"






// Initialize Data



void pg_wifi_list_kbPass(gslc_tsGui *pGui, char* str) {
  if (pg_wifi_net_selected == NULL) { return; }
  if (pg_wifi_net_selected->id < 0) { return; }
  // gslc_ElemSetTxtStr(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], str);
  size_t strSz = snprintf(NULL, 0, "PASSWORD %d %s", pg_wifi_net_selected->id, str) + 1;
  char *strCmd = (char *)malloc(strSz * sizeof(char));
  snprintf(strCmd, strSz, "PASSWORD %d %s", pg_wifi_net_selected->id, str);
  pg_wifi_wpaSendCmd(strCmd);
  free(strCmd);
}


int pg_wifi_list_addNetworkList(struct pg_wifi_networkStruct *ptr) {
  if (pg_wifi_list_networkConfig->len >= pg_wifi_list_networkConfig->max) {
    pg_wifi_list_networkConfig->max = pg_wifi_list_networkConfig->len + 32;
    struct pg_wifi_networkStruct **newPtrs = (struct pg_wifi_networkStruct**)realloc(pg_wifi_list_networkList, pg_wifi_list_networkConfig->max * sizeof(struct pg_wifi_networkStruct*));
    pg_wifi_list_networkList = newPtrs;
  }
  pg_wifi_list_networkList[pg_wifi_list_networkConfig->len] = ptr;
  pg_wifi_list_networkConfig->len += 1;
  VLIST_UPDATE_CONFIG(pg_wifi_list_networkConfig);
  return (pg_wifi_list_networkConfig->len - 1);
}

void pg_wifi_list_setNetworkList(struct pg_wifi_networkStruct **ptrs, int len) {
  // VLIST_CLEAR_CONFIG(pg_wifi_list_networkConfig);
  pg_wifi_list_networkConfig->len = len;
  pg_wifi_list_networkList = ptrs;
  VLIST_UPDATE_CONFIG(pg_wifi_list_networkConfig);
}

void pg_wifi_list_resetNetworkList() {
  VLIST_CLEAR_CONFIG(pg_wifi_list_networkConfig);
}



void pg_wifi_list_wpaEvent(char* event) {
  if (strcmp(event, "CTRL-EVENT-SCAN-RESULTS") == 0) {
    pg_wifi_updateAvailableNetworks();
    pg_wifi_list_setNetworkList(pg_wifi_nets_available->ptrs, pg_wifi_nets_available->len);
    vlist_sliderUpdate(&m_gui, pg_wifi_list_networkConfig);
    gslc_ElemSetRedraw(&m_gui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  } else
  if (strcmp(event, "CTRL-EVENT-SCAN-STARTED") == 0) {
    gslc_ElemSetRedraw(&m_gui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  }
}




//////////////////////////////////
//
// GUI CALLBACK BUTTONS
//
bool pg_wifi_list_cbBtn_elA(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_wifi_wpaScanning == 1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_wifi_list_networkConfig, 0)) { return true; }
  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_wifi_list_cbBtn_elB(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_wifi_wpaScanning == 1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_wifi_list_networkConfig, 1)) { return true; }
  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_wifi_list_cbBtn_elC(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_wifi_wpaScanning == 1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_wifi_list_networkConfig, 2)) { return true; }
  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_wifi_list_cbBtn_elD(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_wifi_wpaScanning == 1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_wifi_list_networkConfig, 3)) { return true; }
  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_wifi_list_cbBtn_elE(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_wifi_wpaScanning == 1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_wifi_list_networkConfig, 4)) { return true; }
  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}


bool pg_wifi_list_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_WIFI);

  return true;
}

bool pg_wifi_list_cbBtn_scan(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  pg_wifi_wpaSendCmd("SCAN");
  return true;
}


bool pg_wifi_list_cbBtn_connect(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (pg_wifi_wpaScanning == 1) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (pg_wifi_list_networkConfig->cur > -1) {
    pg_wifi_net_selected = pg_wifi_list_networkList[pg_wifi_list_networkConfig->cur];
    // printf("Selected: %d - %s\n", pg_wifi_list_networkConfig->cur, pg_wifi_net_selected->ssid);
  } else {
    pg_wifi_net_selected = NULL;
  }

  touchscreenPageOpen(pGui, E_PG_WIFI);
  return true;
}


bool pg_wifi_list_cbBtn_sliderPos(void* pvGui, void* pvElemRef, int16_t nPos)
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
    vlist_sliderSetPos(pGui, pg_wifi_list_networkConfig, gslc_ElemXSliderGetPos(pGui, pElemRef));
    // Update Visual List
    gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
  }
  return true;
}

bool pg_wifi_list_cbBtn_sliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_wifi_list_networkConfig, -1);
  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}
bool pg_wifi_list_cbBtn_sliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_wifi_list_networkConfig, 1);
  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}






// USIS2020FS-4O_1_1234=4O_1_1235=4O_1_1236=4O_1_1237=4O_1_1238=4O_1_1239=4O_1_1235=4O_1_1235=4O_1_1235

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

  if (pg_wifi_wpaScanning == 1) {
    vlist_sliderMessage(pGui, pg_wifi_list_networkConfig, (char*)"... Scanning Wifi Networks ...");
  } else {
    char **list = (char**)malloc(pg_wifi_list_networkConfig->len * sizeof(char*));
    for (int l = 0; l < pg_wifi_list_networkConfig->len; ++l) {
      list[l] = pg_wifi_list_networkList[l]->ssid;
    }
    vlist_sliderDraw(pGui, pg_wifi_list_networkConfig, list, 28);
    free(list);
  }

  // Stop Drawing
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}









// Init Gui
int pg_wifi_list_guiInit(gslc_tsGui *pGui)
{
  // debug_print("%s\n", "Wifi GUI Init");
  int ePage = E_PG_WIFI_LIST;
  gslc_PageAdd(pGui, ePage, pg_wifiListElem, pg_wifiListElTotal, pg_wifiListElemRef, pg_wifiListElTotal);

  gslc_tsRect rListBox = {0,5,425,250};

  // Main View Box
  pg_wifiListEl[E_WIFI_LIST_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rListBox);
  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], &pg_wifi_list_cbDrawBox);




  int xHei = 50;
  pg_wifi_list_networkConfig->refs[0] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (0 * xHei)), rListBox.w, xHei},
          (char*)" ", 0, E_FONT_MONO18, &pg_wifi_list_cbBtn_elA);
  gslc_ElemSetTxtCol(pGui, pg_wifi_list_networkConfig->refs[0], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifi_list_networkConfig->refs[0], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifi_list_networkConfig->refs[0], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifi_list_networkConfig->refs[0], false);
  gslc_ElemSetFrameEn(pGui, pg_wifi_list_networkConfig->refs[0], true);

  pg_wifi_list_networkConfig->refs[1] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (1 * xHei)), rListBox.w, xHei},
          (char*)" ", 0, E_FONT_MONO18, &pg_wifi_list_cbBtn_elB);
  gslc_ElemSetTxtCol(pGui, pg_wifi_list_networkConfig->refs[1], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifi_list_networkConfig->refs[1], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifi_list_networkConfig->refs[1], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifi_list_networkConfig->refs[1], false);
  gslc_ElemSetFrameEn(pGui, pg_wifi_list_networkConfig->refs[1], true);

  pg_wifi_list_networkConfig->refs[2] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (2 * xHei)), rListBox.w, xHei},
          (char*)" ", 0, E_FONT_MONO18, &pg_wifi_list_cbBtn_elC);
  gslc_ElemSetTxtCol(pGui, pg_wifi_list_networkConfig->refs[2], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifi_list_networkConfig->refs[2], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifi_list_networkConfig->refs[2], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifi_list_networkConfig->refs[2], false);
  gslc_ElemSetFrameEn(pGui, pg_wifi_list_networkConfig->refs[2], true);

  pg_wifi_list_networkConfig->refs[3] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (3 * xHei)), rListBox.w, xHei},
          (char*)" ", 0, E_FONT_MONO18, &pg_wifi_list_cbBtn_elD);
  gslc_ElemSetTxtCol(pGui, pg_wifi_list_networkConfig->refs[3], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifi_list_networkConfig->refs[3], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifi_list_networkConfig->refs[3], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifi_list_networkConfig->refs[3], false);
  gslc_ElemSetFrameEn(pGui, pg_wifi_list_networkConfig->refs[3], true);

  pg_wifi_list_networkConfig->refs[4] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rListBox.x, (rListBox.y + (4 * xHei)), rListBox.w, xHei},
          (char*)" ", 0, E_FONT_MONO18, &pg_wifi_list_cbBtn_elE);
  gslc_ElemSetTxtCol(pGui, pg_wifi_list_networkConfig->refs[4], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifi_list_networkConfig->refs[4], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifi_list_networkConfig->refs[4], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifi_list_networkConfig->refs[4], false);
  gslc_ElemSetFrameEn(pGui, pg_wifi_list_networkConfig->refs[4], true);


  //////////////////////////////////////////
  // Create vertical scrollbar
  pg_wifi_list_networkConfig->sliderEl = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO, ePage, &pg_wifi_list_networkSlider,
      (gslc_tsRect){(rListBox.x + rListBox.w) + 5, rListBox.y + 35, rFullscreen.w - (rListBox.x + rListBox.w) - 5, rListBox.h - 70},
      0, pg_wifi_list_networkConfig->scrollMax, 0, 10, true);
      pg_wifi_list_networkConfig->slider = &pg_wifi_list_networkSlider;

  gslc_ElemSetCol(pGui, pg_wifi_list_networkConfig->sliderEl, GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pGui, pg_wifi_list_networkConfig->sliderEl, true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(pGui, pg_wifi_list_networkConfig->sliderEl, &pg_wifi_list_cbBtn_sliderPos);

  //////////////////////////////////////////
  // Create vertical scrollbar Up Arrow
  pg_wifi_list_networkConfig->sliderUpEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 5, rListBox.y, rFullscreen.w - (rListBox.x + rListBox.w) - 5, 30}, (char*)"^",
      0, E_FONT_MONO18, &pg_wifi_list_cbBtn_sliderUp);
  gslc_ElemSetTxtCol(pGui, pg_wifi_list_networkConfig->sliderUpEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_wifi_list_networkConfig->sliderUpEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_wifi_list_networkConfig->sliderUpEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_wifi_list_networkConfig->sliderUpEl, GSLC_ALIGN_MID_MID);


  //////////////////////////////////////////
  // Create vertical scrollbar Down Arrow
  pg_wifi_list_networkConfig->sliderDownEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 5, (rListBox.y + rListBox.h) - 30, rFullscreen.w - (rListBox.x + rListBox.w) - 5, 30}, (char*)"v",
      0, E_FONT_MONO18, &pg_wifi_list_cbBtn_sliderDown);
  gslc_ElemSetTxtCol(pGui, pg_wifi_list_networkConfig->sliderDownEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_wifi_list_networkConfig->sliderDownEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_wifi_list_networkConfig->sliderDownEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_wifi_list_networkConfig->sliderDownEl, GSLC_ALIGN_MID_MID);





  // Select
  pg_wifiListEl[E_WIFI_LIST_EL_CONNECT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + rFullscreen.h - 55), (rFullscreen.w - 220), 50},
          "Select Wifi", 0, E_FONT_MONO14, &pg_wifi_list_cbBtn_connect);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], GSLC_COL_WHITE, GSLC_COL_GREEN, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_CONNECT], true);

  // Scan Key
  pg_wifiListEl[E_WIFI_LIST_EL_SCAN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 210),((rFullscreen.y + rFullscreen.h) - 55),100,50},
          "Scan", 0, E_FONT_MONO14, &pg_wifi_list_cbBtn_scan);
  gslc_ElemSetTxtCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SCAN], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SCAN], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SCAN], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SCAN], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiListEl[E_WIFI_LIST_EL_SCAN], true);

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
  // Dynamically Allocate Page Elements
  pg_wifiListElTotal = E_WIFI_LIST_EL_MAX + 19;
  pg_wifiListElem = (gslc_tsElem *)malloc(pg_wifiListElTotal * sizeof(gslc_tsElem));
  // pg_wifiListElemRef = (gslc_tsElemRef*)calloc(pg_wifiListElTotal, sizeof(gslc_tsElemRef));
  pg_wifiListEl = (gslc_tsElemRef **)malloc(pg_wifiListElTotal * sizeof(gslc_tsElemRef*));

  // Initialize Network list
  pg_wifi_list_networkConfig = VLIST_INIT_CONFIG(5, 32);

  pg_wifi_wpaSendCmd("SCAN");
  pg_wifi_wpaScanning = 1;

  // Create Interface
  pg_wifi_list_guiInit(pGui);

  cbInit[E_PG_WIFI_LIST] = NULL;
}


// GUI Open
void pg_wifi_list_open(gslc_tsGui *pGui) {
  pg_wifi_wpaSetEventCallbackFunc(&pg_wifi_list_wpaEvent);

  gslc_ElemSetRedraw(pGui, pg_wifiListEl[E_WIFI_LIST_EL_BOX], GSLC_REDRAW_FULL);
}

// GUI Close
void pg_wifi_list_close(gslc_tsGui *pGui) {
  pg_wifi_wpaRemoveEventCallbackFunc(0);
}

// GUI Destroy
void pg_wifi_list_destroy() {
  free(pg_wifi_list_networkList);
  free(pg_wifi_list_networkConfig->refs);
  free(pg_wifi_list_networkConfig);
}

void __attribute__ ((constructor)) pg_wifi_list_setup(void) {
  cbInit[E_PG_WIFI_LIST] = &pg_wifi_list_init;
  cbOpen[E_PG_WIFI_LIST] = &pg_wifi_list_open;
  cbThread[E_PG_WIFI_LIST] = NULL;
  cbClose[E_PG_WIFI_LIST] = &pg_wifi_list_close;
  cbDestroy[E_PG_WIFI_LIST] = &pg_wifi_list_destroy;
}
