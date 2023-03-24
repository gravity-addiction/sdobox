#include <sys/stat.h>
#include <assert.h>

#include "./skydiveorbust_videolist.h"
#include "./skydiveorbust_zmq.h"

#include "libs/shared.h"

#include "libs/buttons/buttons.h"
// #include "libs/mpv-zmq/mpv-zmq.h"
#include "libs/queue/queue.h"
#include "gui/pages.h"
#include "libs/vlisting/vlisting.h"
#include "libs/dbg/dbg.h"

#include "gui/skydiveorbust/skydiveorbust.h"
#include "gui/keyboard/keyboard.h"

void pg_sdboVideoListGetFiles(char* path) {
  struct fileStruct **files;
  int i = 0;

  // Clear Files List
  for (i = 0; i < sdob_files->size; ++i) {
    CLEAR(sdob_files->list[i], sdob_files->len);
  }

  // Get Recent List Of Files
  sdob_files->size = 0;
  size_t files_count = file_list(path, &files, S_IFREG);
  qsort(files, files_count, sizeof(char *), cint_cmp);
  for (i = 0; i < files_count; i++) {
    strlcpy(sdob_files->list[sdob_files->size], files[i]->name, strlen(files[i]->name) + 1);
    ++sdob_files->size;
    free(files[i]->name);
  }
  free(files);
}

void pg_sdboVideoListGetFolders(char* path) {
  struct fileStruct **folder;
  int i = 0;

  // Clear Folder List
  for (i = 0; i < sdob_folders->size; ++i) {
    CLEAR(sdob_folders->list[i], sdob_folders->len);
  }

  // Get Recent List Of Folders
  sdob_folders->size = 0;
  size_t folder_count = file_list(path, &folder, S_IFDIR);
  qsort(folder, folder_count, sizeof(char *), cint_cmp);
  for (i = 0; i < folder_count; i++) {
    strlcpy(sdob_folders->list[sdob_folders->size], folder[i]->name, strlen(folder[i]->name));
    ++sdob_folders->size;
    free(folder[i]->name);
  }
  free(folder);
}

static void refreshVideoList(gslc_tsGui *pGui) {
  // Load the previous directory if it exists, otherwise VIDEOS_BASEPATH
  // If the previous dir is VIDEOS_BASEPATH AND it was empty, then this
  // will be triggered also -- the only directory that can be empty is
  // VIDEOS_BASEPATH - subdirs will always have "../" in them.
  char* path_to_load = NULL;
  if (pg_sdobVideo_listConfig->len == 0)
    path_to_load = strdup(VIDEOS_BASEPATH);
  else
    path_to_load = strdup(pg_sdobVideo_list[0]->path);
  pg_sdobVideoList_loadFolder(pGui, path_to_load);
  free(path_to_load);
}

void pg_sdobVideoListClose(gslc_tsGui *pGui) {
  touchscreenPageGoBack(pGui);
}

void pg_sdobVideoList_gotoFolderCheck(gslc_tsGui *pGui) {
  if (pg_sdobVideo_listConfig->cur < 0) { return; }
  if (pg_sdobVideo_listConfig->cur >= pg_sdobVideo_listConfig->len) { return; }

  if (strcmp(pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name, "..") == 0) {
    char* p = pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->path;
    char* last_slash = strrchr(p, '/');
    assert(last_slash);         /* there's no way this should be NULL */
    char* filepath = strndup(p, (last_slash - p));
    pg_sdobVideoList_loadFolder(pGui, filepath);
    free(filepath);
  }
  else if (pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->mode & S_IFDIR) {
    size_t filepathSz = snprintf(NULL, 0, "%s/%s", pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->path, pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name) + 1;
    char *filePath = (char *)malloc(filepathSz * sizeof(char));
    snprintf(filePath, filepathSz, "%s/%s", pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->path, pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name);
    pg_sdobVideoList_loadFolder(pGui, filePath);
    free(filePath);
  }
}

////////////////
// Button Callback
bool pg_sdobVideoListCbBtnFolder(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  dbgprintf(DBG_INFO|DBG_VIDEOLIST,"Fetching Folders\n");
  pg_sdboVideoListGetFolders(VIDEOS_BASEPATH);
  dbgprintf(DBG_INFO|DBG_VIDEOLIST, "Got folder count: %d\n", sdob_folders->size);
  for (int i = 0; i < sdob_folders->size; ++i) {
    dbgprintf(DBG_INFO|DBG_VIDEOLIST, "Folder: %s\n", sdob_folders->list[i]);
  }

  return true;
}

bool pg_sdobVideoListCbBtnCancel(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Close Menu
  pg_sdobVideoListClose(pGui);
  return true;
}

bool pg_sdobVideoListCbBtnChangeVideo(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {

  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (pg_sdobVideo_listConfig->cur >= 0) {
    struct pg_sdob_video_data *newVid = PG_SDOB_INIT_VIDEO_DATA();
    printf("Path: %s\n", pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->path); // + 16);
    printf("Name: %s\n", pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name);

//pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->path, 
    size_t serverUrlSz = snprintf(NULL, 0, "https://transq.thegarybox.com/videos/perris-fresh-meet-2023/%s", pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name) + 1;
    // size_t serverUrlSz = snprintf(NULL, 0, "/home/pi/Videos/%s", pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name) + 1; 
    char *serverUrl = (char *)malloc(serverUrlSz * sizeof(char));
    snprintf(serverUrl, serverUrlSz, "https://transq.thegarybox.com/videos/perris-fresh-meet-2023/%s", pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name);
    // snprintf(serverUrl, serverUrlSz, "/home/pi/Videos/%s", pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name); 
    printf("Server Url: %s\n", serverUrl);
    strlcpy(newVid->url, serverUrl, serverUrlSz);
    
    strlcpy(newVid->local_folder, pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->path, 256);
    strlcpy(newVid->video_file, pg_sdobVideo_list[pg_sdobVideo_listConfig->cur]->name, 256);
        
    // Load Video Into Player
    struct queue_head *item = new_qhead();
    item->action = E_Q_ACTION_LOADVIDEO;
    item->data = newVid;
    item->u1.ptr = pGui;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

/*
    // Parse Filename for Comp Info
    struct queue_head *itemParse = new_qhead();
    itemParse->action = E_Q_ACTION_PARSE_VIDEO_FILENAME;
    itemParse->data = newVid;
    itemParse->u1.ptr = pGui;
    pg_sdob_add_action(&itemParse);
    // queue_put(itemParse, pg_sdobQueue, &pg_sdobQueueLen);
*/
/*
    // Notify Slave Devices
    struct queue_head *itemNotify = new_qhead();
    itemNotify->action = E_Q_ACTION_NOTIFY_SLAVES;
    itemNotify->data = newVid;
    itemNotify->u1.ptr = pGui;
    queue_put(itemNotify, pg_sdobQueue, &pg_sdobQueueLen);
*/
    // Close Menu
     pg_sdobVideoListClose(pGui);
  }

  return true;
}




//////////////////
// Box Drawing
bool pg_sdobVideolist_cbDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  // Clean our rectangle with default background color
  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);

  // Generate list of items based on default list info
  char **list = (char**)malloc(pg_sdobVideo_listConfig->len * sizeof(char*));
  for (int l = 0; l < pg_sdobVideo_listConfig->len; ++l) {
    if (pg_sdobVideo_list[l]->mode & S_IFDIR) {
      size_t nameSz = snprintf(NULL, 0, "%s/", pg_sdobVideo_list[l]->name) + 1;
      list[l] = (char *)malloc(nameSz * sizeof(char));
      snprintf(list[l], nameSz, "%s/", pg_sdobVideo_list[l]->name);
    } else {
      size_t nameSz = snprintf(NULL, 0, "%s", pg_sdobVideo_list[l]->name) + 1;
      list[l] = (char *)malloc(nameSz * sizeof(char));
      snprintf(list[l], nameSz, "%s", pg_sdobVideo_list[l]->name);
    }

  }

  // Use new List
  vlist_sliderDraw(pGui, pg_sdobVideo_listConfig, list, 29);

  // Clean list
  for (int l = 0; l < pg_sdobVideo_listConfig->len; ++l) {
    free(list[l]);
  }
  free(list);


  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}


// A
bool pg_sdobVideolist_cbBtn_elA(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobVideo_listConfig, 0)) { return true; }
  pg_sdobVideoList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// B
bool pg_sdobVideolist_cbBtn_elB(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobVideo_listConfig, 1)) { return true; }
  pg_sdobVideoList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// C
bool pg_sdobVideolist_cbBtn_elC(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobVideo_listConfig, 2)) { return true; }
  pg_sdobVideoList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// D
bool pg_sdobVideolist_cbBtn_elD(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobVideo_listConfig, 3)) { return true; }
  pg_sdobVideoList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}
// E
bool pg_sdobVideolist_cbBtn_elE(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!vlist_clickBtn(pg_sdobVideo_listConfig, 4)) { return true; }
  pg_sdobVideoList_gotoFolderCheck(pGui);

  // Update GUI list Box
  gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

bool pg_sdobVideolist_cbBtn_sliderPos(void* pvGui, void* pvElemRef, int16_t nPos)
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
    vlist_sliderSetPos(pGui, pg_sdobVideo_listConfig, gslc_ElemXSliderGetPos(pGui, pElemRef));
    // Update Visual List
    gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);
  }
  return true;
}

bool pg_sdobVideolist_cbBtn_sliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_sdobVideo_listConfig, -1);
  gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

bool pg_sdobVideolist_cbBtn_sliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  vlist_sliderChangeCurPos(pGui, pg_sdobVideo_listConfig, 1);
  gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

static bool pg_sdobVideoListCbBtnRefresh(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  refreshVideoList(pGui);

  // Redraw
  gslc_ElemSetRedraw(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

/////////////////////
// Init Gui Elements
void pg_sdobVideoListGuiInit(gslc_tsGui *pGui) {

  // Define page enum (gui/pages.h)
  int ePage = E_PG_SDOB_VIDEOLIST;

  // Create Page in guislice
  gslc_PageAdd(pGui, ePage, m_asPgSdobVideolistElem, MAX_ELEM_SDOB_VIDEOLIST, m_asPgSdobVideolistElemRef, MAX_ELEM_SDOB_VIDEOLIST);

  // Create Fullscreen Draw Box
  // Must use a box so redrawing between pages functions correctly
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], &pg_sdobVideolist_cbDraw);
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


  int xHei = 40;
  gslc_tsRect rListBox = {0,40,420,210};
  // Main View Box
  pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rListBox);
  gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BOX], &pg_sdobVideolist_cbDraw);

  // Button A
  pg_sdobVideo_listConfig->refs[0] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (0 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobVideolist_cbBtn_elA);
  gslc_ElemSetTxtCol(pGui, pg_sdobVideo_listConfig->refs[0], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobVideo_listConfig->refs[0], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobVideo_listConfig->refs[0], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobVideo_listConfig->refs[0], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobVideo_listConfig->refs[0], true);

  // Button B
  pg_sdobVideo_listConfig->refs[1] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (1 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobVideolist_cbBtn_elB);
  gslc_ElemSetTxtCol(pGui, pg_sdobVideo_listConfig->refs[1], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobVideo_listConfig->refs[1], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobVideo_listConfig->refs[1], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobVideo_listConfig->refs[1], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobVideo_listConfig->refs[1], true);

  // Button C
  pg_sdobVideo_listConfig->refs[2] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (2 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobVideolist_cbBtn_elC);
  gslc_ElemSetTxtCol(pGui, pg_sdobVideo_listConfig->refs[2], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobVideo_listConfig->refs[2], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobVideo_listConfig->refs[2], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobVideo_listConfig->refs[2], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobVideo_listConfig->refs[2], true);

  // Button D
  pg_sdobVideo_listConfig->refs[3] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (3 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobVideolist_cbBtn_elD);
  gslc_ElemSetTxtCol(pGui, pg_sdobVideo_listConfig->refs[3], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobVideo_listConfig->refs[3], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobVideo_listConfig->refs[3], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobVideo_listConfig->refs[3], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobVideo_listConfig->refs[3], true);

  // Button E
  pg_sdobVideo_listConfig->refs[4] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
        (gslc_tsRect) {rListBox.x, (rListBox.y + (4 * xHei)), rListBox.w, xHei},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobVideolist_cbBtn_elE);
  gslc_ElemSetTxtCol(pGui, pg_sdobVideo_listConfig->refs[4], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_sdobVideo_listConfig->refs[4], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_sdobVideo_listConfig->refs[4], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_sdobVideo_listConfig->refs[4], false);
  gslc_ElemSetFrameEn(pGui, pg_sdobVideo_listConfig->refs[4], true);


  //////////////////////////////////////////
  // Create vertical scrollbar
  pg_sdobVideo_listConfig->sliderEl = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO,
      ePage, &pg_sdobVideo_listSlider, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, rListBox.y + 55, rFullscreen.w - (rListBox.x + rListBox.w) - 2, rListBox.h - 110},
      0, pg_sdobVideo_listConfig->scrollMax, 0, 10, true);
  pg_sdobVideo_listConfig->slider = &pg_sdobVideo_listSlider; // Assign to listConfig for later access

  gslc_ElemSetCol(pGui, pg_sdobVideo_listConfig->sliderEl, GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pGui, pg_sdobVideo_listConfig->sliderEl, true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(pGui, pg_sdobVideo_listConfig->sliderEl, &pg_sdobVideolist_cbBtn_sliderPos);



  //////////////////////////////////////////
  // Create vertical scrollbar Up Arrow
  pg_sdobVideo_listConfig->sliderUpEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, rListBox.y, rFullscreen.w - (rListBox.x + rListBox.w) - 2, 50}, (char*)"^",
      0, E_FONT_MONO18, &pg_sdobVideolist_cbBtn_sliderUp);
  gslc_ElemSetTxtCol(pGui, pg_sdobVideo_listConfig->sliderUpEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_sdobVideo_listConfig->sliderUpEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_sdobVideo_listConfig->sliderUpEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_sdobVideo_listConfig->sliderUpEl, GSLC_ALIGN_MID_MID);


  //////////////////////////////////////////
  // Create vertical scrollbar Down Arrow
  pg_sdobVideo_listConfig->sliderDownEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
      ePage, (gslc_tsRect){(rListBox.x + rListBox.w) + 2, (rListBox.y + rListBox.h) - 50, rFullscreen.w - (rListBox.x + rListBox.w) - 2, 50}, (char*)"v",
      0, E_FONT_MONO18, &pg_sdobVideolist_cbBtn_sliderDown);
  gslc_ElemSetTxtCol(pGui, pg_sdobVideo_listConfig->sliderDownEl, GSLC_COL_GREEN);
  gslc_ElemSetCol(pGui, pg_sdobVideo_listConfig->sliderDownEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pGui, pg_sdobVideo_listConfig->sliderDownEl, true);
  gslc_ElemSetTxtAlign(pGui, pg_sdobVideo_listConfig->sliderDownEl, GSLC_ALIGN_MID_MID);
  pg_sdobVideo_listConfig->sliderDownEl = pg_sdobVideo_listConfig->sliderDownEl;




  // Cancel Button
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + rFullscreen.h) - 60, 100, 60},
            "Close", 0, E_FONT_MONO14, &pg_sdobVideoListCbBtnCancel)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], GSLC_COL_RED, GSLC_COL_BLACK, GSLC_COL_RED);
    gslc_ElemSetTxtAlign(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CANCEL], true);
  }


  // Change Button
  if ((
    pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x + (rFullscreen.w / 3), (rFullscreen.y + rFullscreen.h) - 60, (rFullscreen.w / 3), 60},
            "Load Video", 0, E_FONT_MONO14, &pg_sdobVideoListCbBtnChangeVideo)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_GREEN);
    gslc_ElemSetTxtAlign(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], true);
    gslc_ElemSetFrameEn(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_CHANGE], true);
  }

  // Re-fresh Button
  gslc_tsElemRef** eRef = &pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_REFRESH];
  if ((*eRef = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
                                     (gslc_tsRect){rFullscreen.x + rFullscreen.w - 1 - 100, (rFullscreen.y + rFullscreen.h) - 60,
                                                     100, 60},
                                     "Refresh", 0, E_FONT_MONO14, &pg_sdobVideoListCbBtnRefresh))
      != NULL) {
    gslc_ElemSetTxtCol(pGui, *eRef, GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, *eRef, GSLC_COL_CYAN, GSLC_COL_BLACK, GSLC_COL_CYAN);
    gslc_ElemSetTxtAlign(pGui, *eRef, GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, *eRef, true);
    gslc_ElemSetFrameEn(pGui, *eRef, true);
  }
}


int pg_sdobVideoListButtonRotaryCW() {
  // debug_print("%s\n", "SDOB Videolist CW");
  return 0;
}
int pg_sdobVideoListButtonRotaryCCW() {
  // debug_print("%s\n", "SDOB Videolist CCW");
  return 0;
}
int pg_sdobVideoListButtonLeftPressed() {
  // debug_print("%s\n", "SDOB Videolist Left Pressed");
  return 0;
}
int pg_sdobVideoListButtonRightPressed() {
  // debug_print("%s\n", "SDOB Videolist Right Pressed");
  return 0;
}
int pg_sdobVideoListButtonRotaryPressed() {
  // debug_print("%s\n", "SDOB Videolist Rotary Pressed");
  return 0;
}
int pg_sdobVideoListButtonLeftHeld() {
  // debug_print("%s\n", "SDOB Videolist Left Held");
  return 0;
}
int pg_sdobVideoListButtonRightHeld() {
  // debug_print("%s\n", "SDOB Videolist Right Held");
  return 0;
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
}

void pg_sdobVideoList_free_filelist() {
  for (int i = 0; i < pg_sdobVideo_listConfig->len; i++) {
    free(pg_sdobVideo_list[i]->name);
    free(pg_sdobVideo_list[i]->path);
    free(pg_sdobVideo_list[i]);
  }
}

void pg_sdobVideoList_loadFolder(gslc_tsGui *pGui, char* folderPath) {
  if (pg_sdobVideo_listConfig->len > 0) {
    pg_sdobVideoList_free_filelist();
    free(pg_sdobVideo_list);
  }

  // VLIST_CLEAR_CONFIG(pg_sdobVideo_listConfig);

  if (strcmp(folderPath, VIDEOS_BASEPATH) == 0) {
    pg_sdobVideo_listConfig->len = file_list(folderPath, &pg_sdobVideo_list, -1);
  } else {
    pg_sdobVideo_listConfig->len = file_list_w_up(folderPath, &pg_sdobVideo_list, -1);
  }

  qsort(pg_sdobVideo_list, pg_sdobVideo_listConfig->len, sizeof(char *), fileStruct_cmpName);
  VLIST_UPDATE_CONFIG(pg_sdobVideo_listConfig);
  vlist_sliderUpdate(pGui, pg_sdobVideo_listConfig);
}

// GUI Init
void pg_sdobVideoList_init(gslc_tsGui *pGui) {
  pg_sdobVideo_listConfig = VLIST_INIT_CONFIG(5, 32);

  pg_sdobVideoListGuiInit(pGui);

  // Cleanup so Init is only ran once
  cbInit[E_PG_SDOB_VIDEOLIST] = NULL;
}


// GUI Open
void pg_sdobVideoList_open(gslc_tsGui *pGui) {
  // Setup button function callbacks every time page is opened / reopened
  pg_sdobVideoListButtonSetFuncs();

  refreshVideoList(pGui);

  gslc_ElemSetTxtStr(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_TXT_TMP], "Video List");
  gslc_ElemSetTxtStr(pGui, pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_BTN_FOLDER], sdob_judgement->video->local_folder);
}


// GUI Destroy
void pg_sdobVideoList_destroy(gslc_tsGui *pGui) {
  // pg_sdobVideoList_free_filelist();
}

// Setup Constructor
void __attribute__ ((constructor)) pg_sdobVideoList_setup(void) {
  cbInit[E_PG_SDOB_VIDEOLIST] = &pg_sdobVideoList_init;
  cbOpen[E_PG_SDOB_VIDEOLIST] = &pg_sdobVideoList_open;
  cbDestroy[E_PG_SDOB_VIDEOLIST] = &pg_sdobVideoList_destroy;
}
