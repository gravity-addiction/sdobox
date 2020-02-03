#include "skydiveorbust_videolist.h"

#include "buttons/buttons.h"
#include "mpv/mpv.h"
#include "queue/queue.h"
#include "gui/pages.h"

#include "gui/skydiveorbust/skydiveorbust.h"
#include "gui/keyboard/keyboard.h"



void pg_sdboVideoListGetFiles(char* path) {
  char **files;
  int i = 0;

  // Clear Files List
  for (i = 0; i < sdob_files->size; ++i) {
    CLEAR(sdob_files->list[i], sdob_files->len);
  }

  // Get Recent List Of Files
  sdob_files->size = 0;
  size_t files_count = file_list(path, &files);
  qsort(files, files_count, sizeof(char *), cmp_atoi);
  for (i = 0; i < files_count; i++) {
    if (
      strcmp(files[i], ".DS_Store") == 0
      // || strcmp(files[i], ".."
    ) { continue; }
    strlcpy(sdob_files->list[sdob_files->size], files[i], sdob_files->len);
    ++sdob_files->size;
  }
}

void pg_sdboVideoListGetFolders(char* path) {
  char **folder;
  int i = 0;

  // Clear Folder List
  for (i = 0; i < sdob_folders->size; ++i) {
    CLEAR(sdob_folders->list[i], sdob_folders->len);
  }

  // Get Recent List Of Folders
  sdob_folders->size = 0;
  size_t folder_count = folder_list(path, &folder);
  qsort(folder, folder_count, sizeof(char *), cmp_atoi);
  for (i = 0; i < folder_count; i++) {
    if (strcmp(folder[i], ".") == 0 || strcmp(folder[i], "..") == 0) { continue; }
    strlcpy(sdob_folders->list[sdob_folders->size], folder[i], sdob_folders->len);
    ++sdob_folders->size;
  }
}


void pg_sdobVideoListClose(gslc_tsGui *pGui) {
  touchscreenPageClose(pGui, E_PG_SDOB_VIDEOLIST);
  touchscreenPageOpen(pGui, m_page_previous);
}

////////////////
// Button Callback
bool pg_sdobVideoListCbBtnCancel(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  pg_sdobVideoListClose(pGui);
  return true;
}

bool pg_sdobVideoListCbBtnFolder(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  printf("Fetching Folders\n");
  pg_sdboVideoListGetFolders("/home/pi/Videos/");
  printf("Got Cnt: %d\n", sdob_folders->size);
  for (int i = 0; i < sdob_folders->size; ++i) {
    printf("Folder: %s\n", sdob_folders->list[i]);
  }

  return true;
}

bool pg_sdobVideoListCbBtnClear(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Close Submit Menu
  pg_sdobVideoListClose(pGui);
  return true;
}

bool pg_sdobVideoListCbBtnChangeVideo(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Clear Scorecard
  struct queue_head *item = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(item);
  item->action = E_Q_SCORECARD_CLEAR;
  queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

  char* tmpMeet = "SVNH2019";
  char* tmpFile = "Group1-34_2.mp4";
  pg_sdobUpdateMeet(pGui, tmpMeet);
  pg_sdobUpdateVideoDesc(pGui, tmpFile);
  pg_sdobUpdateVideoRate(pGui, mpv_speed(1.0));

  mpv_loadfile(tmpMeet, tmpFile, "replace", "fullscreen=yes");

  pg_sdob_pl_sliderForceUpdate = 1;

  // Close Submit Menu
  pg_sdobVideoListClose(pGui);
  return true;
}




//////////////////
// Box Drawing
bool pg_sdobVideoListCbDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);
  gslc_DrawLine(pGui, pRect.x, pRect.y + 60, pRect.x + pRect.w, pRect.y + 60, GSLC_COL_GRAY);

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}




/////////////////////
// Init Gui Elements
void pg_sdobVideoListGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_SDOB_VIDEOLIST;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgSdobVideolistElem, MAX_ELEM_PG_DEFAULT, m_asPgSdobVideolistElemRef, MAX_ELEM_PG_DEFAULT);

  // Create Fullscreen Draw Box
  // Must use a box so redrawing between pages functions correctly
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], &pg_sdobVideoListCbDraw);
  }



  /////////////////////
  // Page Defined Elements

  // Add Text Element
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_TXT_TMP] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y, (rFullscreen.w - 108), 60},
          (char*)" ", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_TXT_TMP], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_TXT_TMP], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_TXT_TMP], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_TXT_TMP], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_TXT_TMP], false);
  }

  // Meet Button Element
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 150), rFullscreen.y, rFullscreen.w - 150, 60},
          (char*)" ", 0, E_FONT_MONO18, &pg_sdobVideoListCbBtnFolder)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], GSLC_ALIGN_MID_RIGHT);
    gslc_ElemSetFillEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], false);
  }

/*
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 150), rFullscreen.y, rFullscreen.w - 150, 60},
          (char*)" ", 0, E_FONT_MONO18, &pg_sdobVideoListCbBtnFolder)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], GSLC_ALIGN_MID_RIGHT);
    gslc_ElemSetFillEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], false);
  }
*/



  // Cancel Button
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
            "Cancel", 0, E_FONT_MONO14, &pg_sdobVideoListCbBtnCancel)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], GSLC_COL_RED, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], false);
    gslc_ElemSetFrameEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], true);
  }

  // Clear Scorecard Button
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CLEAR] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {(rFullscreen.x + rFullscreen.w) - 100, (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
            "Clear", 0, E_FONT_MONO14, &pg_sdobVideoListCbBtnClear)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CLEAR], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CLEAR], GSLC_COL_PURPLE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CLEAR], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CLEAR], false);
    gslc_ElemSetFrameEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CLEAR], true);
  }

  // Change Button
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x + (rFullscreen.w / 3), (rFullscreen.y + rFullscreen.h) - 60, (rFullscreen.w / 3), 60},
            "Change Video", 0, E_FONT_MONO14, &pg_sdobVideoListCbBtnChangeVideo)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], false);
    gslc_ElemSetFrameEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], true);
  }

}


void pg_sdobVideoListButtonRotaryCW() {
  // debug_print("%s\n", "SDOB Videolist CW");
}
void pg_sdobVideoListButtonRotaryCCW() {
  // debug_print("%s\n", "SDOB Videolist CCW");
}
void pg_sdobVideoListButtonLeftPressed() {
  // debug_print("%s\n", "SDOB Videolist Left Pressed");
}
void pg_sdobVideoListButtonRightPressed() {
  // debug_print("%s\n", "SDOB Videolist Right Pressed");
}
void pg_sdobVideoListButtonRotaryPressed() {
  // debug_print("%s\n", "SDOB Videolist Rotary Pressed");
}
void pg_sdobVideoListButtonLeftHeld() {
  // debug_print("%s\n", "SDOB Videolist Left Held");
}
void pg_sdobVideoListButtonRightHeld() {
  // debug_print("%s\n", "SDOB Videolist Right Held");
}
void pg_sdobVideoListButtonRotaryHeld() {
  // debug_print("%s\n", "SDOB Videolist Rotary Held");
}
void pg_sdobVideoListButtonDoubleHeld() {
  // debug_print("%s\n", "SDOB Videolist Double Held");
  // systemMenuOpen();
}

// Setup Button Functions
// Rem out any you don't want to overwrite, only one function cb per enum
void pg_sdobVideoListButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_sdobVideoListButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_sdobVideoListButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_sdobVideoListButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_sdobVideoListButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_sdobVideoListButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_sdobVideoListButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_sdobVideoListButtonRightHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, &pg_sdobVideoListButtonRotaryHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, &pg_sdobVideoListButtonDoubleHeld);
}


// GUI Init
void pg_sdobVideoList_init(gslc_tsGui *pGui) {
  pg_sdobVideoListGuiInit(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_SDOB_VIDEOLIST] = NULL;
}


// GUI Open
void pg_sdobVideoList_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  // pg_sdobVideoListButtonSetFuncs();

  // // Clear File and Folder lists
  // for (i = 0; i < sdob_folders->max; ++i) { CLEAR(sdob_folders->list[i], sdob_folders->len); }
  // for (i = 0; i < sdob_files->max; ++i) { CLEAR(sdob_files->list[i], sdob_files->len); }

  gslc_ElemSetTxtStr(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_TXT_TMP], "Video List");
  gslc_ElemSetTxtStr(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], sdob_judgement->meet);

  size_t fullpathSz = snprintf(NULL, 0, "/home/pi/Videos/%s/", sdob_judgement->meet) + 1;
  if (fullpathSz > 0) {
    char *fullpath = (char*)malloc(fullpathSz + sizeof(char));
    snprintf(fullpath, fullpathSz, "/home/pi/Videos/%s/", sdob_judgement->meet);
    pg_sdboVideoListGetFiles(fullpath);
    printf("Got File Cnt: %d\n", sdob_files->size);
    for (int i = 0; i < sdob_files->size; ++i) {
      printf("File: %s\n", sdob_files->list[i]);
    }

  }
}


// GUI Destroy
void pg_sdobVideoList_destroy(gslc_tsGui *pGui) {

}

// Setup Constructor
void __attribute__ ((constructor)) pg_sdobVideoList_setup(void) {
  cbInit[E_PG_SDOB_VIDEOLIST] = &pg_sdobVideoList_init;
  cbOpen[E_PG_SDOB_VIDEOLIST] = &pg_sdobVideoList_open;
  cbDestroy[E_PG_SDOB_VIDEOLIST] = &pg_sdobVideoList_destroy;
}
