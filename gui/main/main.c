#include <math.h>
#include "main.h"
#include <sys/stat.h>

#include "buttons/buttons.h"
#include "vlisting/vlisting.h"
#include "mpv/mpv.h"
#include "fbcp/fbcp.h"

#include "gui/pages.h"
#include "gui/keyboard/keyboard.h"
#include "gui/skydiveorbust/skydiveorbust.h"


bool pg_main_list_check() {
  // Current index larger than length
  if (pg_main_listConfig->cur < 0 || pg_main_listConfig->cur >= pg_main_listConfig->len) {
    return false;
  }
  size_t pathSz = snprintf(NULL, 0, "%s/%s", pg_main_list[pg_main_listConfig->cur]->path, pg_main_list[pg_main_listConfig->cur]->name) + 1;
  char *path = (char*)malloc(pathSz * sizeof(char));
  snprintf(path, pathSz, "%s/%s", pg_main_list[pg_main_listConfig->cur]->path, pg_main_list[pg_main_listConfig->cur]->name);

  // File exists and is accessable
  if (!file_exists(path)) {
    free(path);
    return false;
  }
  free(path);

  return true;
}
////////////////
// Button Callback
bool pg_main_cbBtn_startX(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  system("export DISPLAY=:0.0; startx &");
  return true;
}

bool pg_main_cbBtn_slideshow(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (!pg_main_list_check()) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SLIDESHOW);

  char* fileExt = file_ext(pg_main_list[pg_main_listConfig->cur]->name);
  size_t cmdSz = snprintf(NULL, 0, "loadlist \"%s/%s\" replace\n", pg_main_list[pg_main_listConfig->cur]->path, pg_main_list[pg_main_listConfig->cur]->name) + 1;
  char *cmd = (char*)malloc(cmdSz * sizeof(char));

  // Load .txt as playlists
  if (strcasecmp(fileExt, "txt") == 0) {
    snprintf(cmd, cmdSz, "loadlist \"%s/%s\" replace\n", pg_main_list[pg_main_listConfig->cur]->path, pg_main_list[pg_main_listConfig->cur]->name);
    mpv_cmd(cmd);
  } else {
    snprintf(cmd, cmdSz, "loadfile \"%s/%s\" replace\n", pg_main_list[pg_main_listConfig->cur]->path, pg_main_list[pg_main_listConfig->cur]->name);
    mpv_cmd(cmd);
  }
  return true;
}

bool pg_main_cbBtn_sdob(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  if (!pg_main_list_check()) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SKYDIVEORBUST);

  // If a file is selected try to load it as video.
  pg_skydiveorbust_loadvideo(pGui, pg_main_list[pg_main_listConfig->cur]->path, pg_main_list[pg_main_listConfig->cur]->name);

  return true;
}

bool pg_main_cbBtn_root(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_main_loadFolder(pGui, "/home/pi/shared");
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_main_cbBtn_usb(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_main_loadFolder(pGui, "/media");
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_main_cbBtn_network(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_main_loadFolder(pGui, "network:///");
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_main_cbBtn_mirror(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  fbcp_toggle();
  return true;
}

bool pg_main_cbBtn_system(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SYSTEM);
  return true;
}

int pg_main_addList(struct fileStruct *ptr) {
  if (pg_main_listConfig->len >= pg_main_listConfig->max) {
    pg_main_listConfig->max = pg_main_listConfig->len + 32;
    struct fileStruct **newPtrs = (struct fileStruct**)realloc(pg_main_list, pg_main_listConfig->max * sizeof(struct fileStruct*));
    pg_main_list = newPtrs;
  }
  pg_main_list[pg_main_listConfig->len] = ptr;
  pg_main_listConfig->len += 1;
  return (pg_main_listConfig->len - 1);
}

void pg_main_setList(struct fileStruct **ptrs, int len) {
  for (int i = 0; i < pg_main_listConfig->len; ++i) {
    free(pg_main_list[i]->name);
    free(pg_main_list[i]);
  }
  free(pg_main_list);

  pg_main_listConfig->len = len;
  pg_main_list = ptrs;
}

void pg_main_resetList() {
  for (int i = 0; i < pg_main_listConfig->len; ++i) {
    free(pg_main_list[i]->name);
    free(pg_main_list[i]);
  }
  free(pg_main_list);
  VLIST_CLEAR_CONFIG(pg_main_listConfig);
}


///////////////////////
// Keyboard Button
void pg_mainCbBtnKeyboard_Callback(gslc_tsGui *pGui, char* str) {
  gslc_ElemSetTxtStr(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], str);
}

bool pg_mainCbBtnKeyboard(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Show Keyboard
  pg_keyboard_show(pGui, 16, gslc_ElemGetTxtStr(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB]), &pg_mainCbBtnKeyboard_Callback);
  return true;
}



//////////////////
// Box Drawing
bool pg_main_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);

  // Generate list of items based on default list info
  char **list = (char**)malloc(pg_main_listConfig->len * sizeof(char*));
  for (int l = 0; l < pg_main_listConfig->len; ++l) {
    if (pg_main_list[l]->mode & S_IFDIR) {
      size_t nameSz = snprintf(NULL, 0, "%s/", pg_main_list[l]->name) + 1;
      list[l] = (char *)malloc(nameSz * sizeof(char));
      snprintf(list[l], nameSz, "%s/", pg_main_list[l]->name);
    } else {
      size_t nameSz = snprintf(NULL, 0, "%s", pg_main_list[l]->name) + 1;
      list[l] = (char *)malloc(nameSz * sizeof(char));
      snprintf(list[l], nameSz, "%s", pg_main_list[l]->name);
    }

  }

  // Use new List
  vlist_sliderDraw(pGui, pg_main_listConfig, list, 29);

  // Clean list
  for (int l = 0; l < pg_main_listConfig->len; ++l) {
    free(list[l]);
  }
  free(list);


  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}

void pg_main_gotoFolderCheck(gslc_tsGui *pGui) {
  if (pg_main_list[pg_main_listConfig->cur]->mode & S_IFDIR) {
    size_t filepathSz = snprintf(NULL, 0, "%s/%s", pg_main_list[pg_main_listConfig->cur]->path, pg_main_list[pg_main_listConfig->cur]->name) + 1;
    char *filePath = (char *)malloc(filepathSz * sizeof(char));
    snprintf(filePath, filepathSz, "%s/%s", pg_main_list[pg_main_listConfig->cur]->path, pg_main_list[pg_main_listConfig->cur]->name);
    pg_main_loadFolder(pGui, filePath);
    free(filePath);
  }
}
// A
bool pg_main_cbBtn_elA(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_main_listConfig, 0)) { return true; }
  pg_main_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// B
bool pg_main_cbBtn_elB(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_main_listConfig, 1)) { return true; }
  pg_main_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// C
bool pg_main_cbBtn_elC(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_main_listConfig, 2)) { return true; }
  pg_main_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// D
bool pg_main_cbBtn_elD(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_main_listConfig, 3)) { return true; }
  pg_main_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// E
bool pg_main_cbBtn_elE(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_main_listConfig, 4)) { return true; }
  pg_main_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// F
bool pg_main_cbBtn_elF(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_main_listConfig, 5)) { return true; }
  pg_main_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// G
bool pg_main_cbBtn_elG(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_main_listConfig, 6)) { return true; }
  pg_main_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_main_cbBtn_sliderPos(void* pvGui, void* pvElemRef, int16_t nPos)
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
    vlist_sliderSetPos(pGui, pg_main_listConfig, gslc_ElemXSliderGetPos(pGui, pElemRef));
    // Update Visual List
    gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);
  }
  return true;
}

bool pg_main_cbBtn_sliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_main_listConfig, -1);
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

bool pg_main_cbBtn_sliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_main_listConfig, 1);
  gslc_ElemSetRedraw(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}



/////////////////////
// Init Gui Elements
void pg_mainGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_MAIN;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgMainElem, MAX_ELEM_PG_MAIN, m_asPgMainElemRef, MAX_ELEM_PG_MAIN);


  // Playlist Playing
  if ((
    pg_mainEl[E_MAIN_EL_PLAYLIST] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {100, 0, 380, 35},
            "Currently Playing: ", 0, E_FONT_MONO18, &pg_main_cbBtn_slideshow)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_PLAYLIST], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_PLAYLIST], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_PLAYLIST], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetTxtMarginXY(pGui, pg_mainEl[E_MAIN_EL_PLAYLIST], 10, 0);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_PLAYLIST], false);
    gslc_ElemSetGlowEn(pGui, pg_mainEl[E_MAIN_EL_PLAYLIST], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_PLAYLIST], false);
  }
  if ((
    pg_mainEl[E_MAIN_EL_PLAYLIST_UL] = gslc_ElemCreateLine(pGui, GSLC_ID_AUTO, ePage, 100, 35, 480, 35)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_PLAYLIST_UL], GSLC_COL_WHITE, GSLC_COL_WHITE, GSLC_COL_WHITE);
  }


  if ((
    pg_mainEl[E_MAIN_EL_BTN_A] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 15, 100, 50},
            "Slideshow", 0, E_FONT_MONO14, &pg_main_cbBtn_slideshow)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_A], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_A], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_A], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_A], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_A], true);
  }
  // Button B
  if ((
    pg_mainEl[E_MAIN_EL_BTN_B] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 65, 100, 50},
            "SDOB", 0, E_FONT_MONO14, &pg_main_cbBtn_sdob)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_B], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_B], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_B], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_B], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_B], true);
  }
  // Button C
  if ((
    pg_mainEl[E_MAIN_EL_BTN_C] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 115, 100, 50},
            "Root", 0, E_FONT_MONO14, &pg_main_cbBtn_root)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_C], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_C], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_C], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_C], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_C], true);
  }
  // Button D
  if ((
    pg_mainEl[E_MAIN_EL_BTN_D] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 165, 100, 50},
            "USB", 0, E_FONT_MONO14, &pg_main_cbBtn_usb)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_D], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_D], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_D], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_D], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_D], true);
  }
  // Button E
  if ((
    pg_mainEl[E_MAIN_EL_BTN_E] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 215, 100, 50},
            "Mirror", 0, E_FONT_MONO14, &pg_main_cbBtn_mirror)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_E], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_E], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_E], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_E], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_E], true);
  }
  // Button F
  if ((
    pg_mainEl[E_MAIN_EL_BTN_F] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0, 265, 100, 50},
            "System", 0, E_FONT_MONO14, &pg_main_cbBtn_system)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_F], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_F], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_F], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_F], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_F], true);
  }



  int xHei = 40;
  gslc_tsRect rListBox = {103,40,320,280};
  // Main View Box
  pg_mainEl[E_MAIN_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rListBox);
  gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_mainEl[E_MAIN_EL_BOX], &pg_main_cbDrawBox);

  // Button A
  pg_main_listConfig->refs[0] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (0 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_main_cbBtn_elA);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->refs[0], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->refs[0], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_main_listConfig->refs[0], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_main_listConfig->refs[0], false);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->refs[0], true);

  // Button B
  pg_main_listConfig->refs[1] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (1 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_main_cbBtn_elB);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->refs[1], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->refs[1], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_main_listConfig->refs[1], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_main_listConfig->refs[1], false);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->refs[1], true);

  // Button C
  pg_main_listConfig->refs[2] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (2 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_main_cbBtn_elC);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->refs[2], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->refs[2], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_main_listConfig->refs[2], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_main_listConfig->refs[2], false);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->refs[2], true);

  // Button D
  pg_main_listConfig->refs[3] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (3 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_main_cbBtn_elD);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->refs[3], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->refs[3], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_main_listConfig->refs[3], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_main_listConfig->refs[3], false);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->refs[3], true);

  // Button E
  pg_main_listConfig->refs[4] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (4 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_main_cbBtn_elE);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->refs[4], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->refs[4], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_main_listConfig->refs[4], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_main_listConfig->refs[4], false);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->refs[4], true);

  // Button F
  pg_main_listConfig->refs[5] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (5 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_main_cbBtn_elF);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->refs[5], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->refs[5], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_main_listConfig->refs[5], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_main_listConfig->refs[5], false);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->refs[5], true);

  // Button G
  pg_main_listConfig->refs[6] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (6 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_main_cbBtn_elG);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->refs[6], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->refs[6], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_main_listConfig->refs[6], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_main_listConfig->refs[6], false);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->refs[6], true);


  //////////////////////////////////////////
  // Create vertical scrollbar
  pg_main_listConfig->sliderEl = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO,
      ePage, &pg_main_listSlider, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, rListBox.y + 55, rFullscreen.w - (rListBox.x + rListBox.w) - 2, rListBox.h - 110},
      0, pg_main_listConfig->scrollMax, 0, 10, true);
  pg_main_listConfig->slider = &pg_main_listSlider; // Assign to listConfig for later access

  gslc_ElemSetCol(pGui, pg_main_listConfig->sliderEl, GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pGui, pg_main_listConfig->sliderEl, true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(pGui, pg_main_listConfig->sliderEl, &pg_main_cbBtn_sliderPos);



  //////////////////////////////////////////
  // Create vertical scrollbar Up Arrow
  pg_main_listConfig->sliderUpEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, rListBox.y, rFullscreen.w - (rListBox.x + rListBox.w) - 2, 50}, (char*)"^",
      0, E_FONT_MONO18, &pg_main_cbBtn_sliderUp);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->sliderUpEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_main_listConfig->sliderUpEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->sliderUpEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->sliderUpEl, GSLC_ALIGN_MID_MID);


  //////////////////////////////////////////
  // Create vertical scrollbar Down Arrow
  pg_main_listConfig->sliderDownEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, (rListBox.y + rListBox.h) - 50, rFullscreen.w - (rListBox.x + rListBox.w) - 2, 50}, (char*)"v",
      0, E_FONT_MONO18, &pg_main_cbBtn_sliderDown);
  gslc_ElemSetTxtCol(pGui, pg_main_listConfig->sliderDownEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_main_listConfig->sliderDownEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_main_listConfig->sliderDownEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_main_listConfig->sliderDownEl, GSLC_ALIGN_MID_MID);
  pg_main_listConfig->sliderDownEl = pg_main_listConfig->sliderDownEl;

/*

  if ((
    pg_mainEl[E_MAIN_EL_BTN_FOLDER_A] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {100, 40, 260, 40},
            "Previous Folder", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_A], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_A], GSLC_COL_WHITE, GSLC_COL_BLUE, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_A], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetTxtMarginXY(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_A], 10, 0);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_A], false);
    gslc_ElemSetGlowEn(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_A], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_A], true);
  }
  if ((
    pg_mainEl[E_MAIN_EL_BTN_FOLDER_AA] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {360, 40, 120, 40},
            "0.0gB", 0, E_FONT_MONO14)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_AA], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_AA], GSLC_COL_WHITE, GSLC_COL_BLUE, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_AA], GSLC_ALIGN_MID_RIGHT);
    gslc_ElemSetTxtMarginXY(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_AA], 10, 0);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_AA], false);
    gslc_ElemSetGlowEn(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_AA], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_FOLDER_AA], true);
  }
*/
/*

  // Create Fullscreen Draw Box
  // Must use a box so redrawing between pages functions correctly
  if ((
    pg_mainEl[E_MAIN_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BOX], GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, pg_mainEl[E_MAIN_EL_BOX], &pg_main_cbDraw);
  }



  /////////////////////
  // Page Defined Elements

  // Add Text Element
  if ((
    pg_mainEl[E_MAIN_EL_TXT_TMP] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y, (rFullscreen.w - 108), 60},
          (char*)"Some Text", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], true);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_TXT_TMP], false);
  }


  // Add Text Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_TMP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.w / 3, rFullscreen.h / 3, rFullscreen.w / 3, rFullscreen.h / 3},
            "Wifi", 0, E_FONT_MONO14, &pg_mainCbBtn)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_TMP], true);
  }


  // Add Text Keyboard Button
  if ((
    pg_mainEl[E_MAIN_EL_BTN_TMPKB] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect){(rFullscreen.w - 100), rFullscreen.y, 100, 60},
            "Change Text", 0, E_FONT_MONO14, &pg_mainCbBtnKeyboard)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_BTN_TMPKB], true);
  }


  // StartX
  if ((
    pg_mainEl[E_MAIN_EL_OPEN_STARTX] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {30, 100, 100, 60},
            "X Windows", 0, E_FONT_MONO14, &pg_main_cbBtn_startX)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_STARTX], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_STARTX], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_OPEN_STARTX], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_STARTX], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_STARTX], true);
  }


  // Open Slideshow
  if ((
    pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {30, 200, 100, 60},
            "Slideshow", 0, E_FONT_MONO14, &pg_mainCbBtnSlideshow)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_SLIDESHOW], true);
  }

  // Open Skydive or Bust
  if ((
    pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {350, 200, 100, 60},
            "Skydive Or Bust", 0, E_FONT_MONO14, &pg_mainCbBtnSDOB)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], false);
    gslc_ElemSetFrameEn(pGui, pg_mainEl[E_MAIN_EL_OPEN_SKYDIVEORBUST], true);
  }
*/
}


void pg_mainButtonRotaryCW() {
  // debug_print("%s\n", "Main CW");
}
void pg_mainButtonRotaryCCW() {
  // debug_print("%s\n", "Main CCW");
}
void pg_mainButtonLeftPressed() {
  // debug_print("%s\n", "Main Left Pressed");
}
void pg_mainButtonRightPressed() {
  // debug_print("%s\n", "Main Right Pressed");
}
void pg_mainButtonRotaryPressed() {
  // debug_print("%s\n", "Main Rotary Pressed");
}
void pg_mainButtonLeftHeld() {
  // debug_print("%s\n", "Main Left Held");
}
void pg_mainButtonRightHeld() {
  // debug_print("%s\n", "Main Right Held");
}
void pg_mainButtonRotaryHeld() {
  // debug_print("%s\n", "Main Rotary Held");
}
void pg_mainButtonDoubleHeld() {
  // debug_print("%s\n", "Main Double Held");
  // systemMenuOpen();
}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_mainButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_mainButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_mainButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_mainButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_mainButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_mainButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_mainButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_mainButtonRightHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, &pg_mainButtonRotaryHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, &pg_mainButtonDoubleHeld);
}

void pg_main_refreshCurrentFolder(gslc_tsGui* pGui) {
  pg_main_resetList();
  pg_main_listConfig->len = file_list(pg_main_currentFolderPath, &pg_main_list, -1);
  qsort(pg_main_list, pg_main_listConfig->len, sizeof(char *), fileStruct_cmpName);
  VLIST_UPDATE_CONFIG(pg_main_listConfig);
  vlist_sliderUpdate(pGui, pg_main_listConfig);
}

void pg_main_loadFolder(gslc_tsGui *pGui, char* folderPath) {
  free(pg_main_currentFolderPath);
  pg_main_currentFolderPath = strdup(folderPath);
  pg_main_refreshCurrentFolder(pGui);
}


// GUI Init
void pg_main_init(gslc_tsGui *pGui) {
  // Initialize Network list
  pg_main_listConfig = VLIST_INIT_CONFIG(7, 32);

  pg_mainGuiInit(pGui);

  pg_main_currentFolderPath = NULL;

  pg_main_loadFolder(pGui, "/home/pi/shared");

  // Cleanup so Init is only ran once
  cbInit[E_PG_MAIN] = NULL;
}


// GUI Open
void pg_main_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_mainButtonSetFuncs();

  pg_main_refreshCurrentFolder(pGui);
}


// GUI Destroy
void pg_main_destroy(gslc_tsGui *pGui) {
  for (int i = 0; i < pg_main_listConfig->len; ++i) {
    free(pg_main_list[i]->name);
    free(pg_main_list[i]);
  }
  free(pg_main_list);
  free(pg_main_listConfig->refs);
  free(pg_main_listConfig);
  free(pg_main_currentFolderPath);
}

// Setup Constructor
void __attribute__ ((constructor)) pg_main_setup(void) {
  cbInit[E_PG_MAIN] = &pg_main_init;
  cbOpen[E_PG_MAIN] = &pg_main_open;
  cbDestroy[E_PG_MAIN] = &pg_main_destroy;
}
