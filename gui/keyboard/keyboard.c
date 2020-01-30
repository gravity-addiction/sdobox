#include <stdio.h>
#include <string.h>

#include "keyboard.h"
#include "shared.h"
#include "gui/pages.h"

/*
static int keyboardLayout[36] = {
  49, 50, 51, 52, 53, 54, 55, 56, 57, 48,
  81, 87, 69, 82, 84, 89, 85, 73, 79, 80,
  65, 83, 68, 70, 71, 72, 74, 75, 76,
  90, 88, 67, 86, 66, 78, 77
};
*/





struct pg_keyboard_dataStruct * PG_KEYBOARD_INIT_DATA()
{
  struct pg_keyboard_dataStruct *data = (struct pg_keyboard_dataStruct*)malloc(sizeof(struct pg_keyboard_dataStruct));
  
  data->max = 128;
  data->limit = 100;
  data->len = 0;

  data->ptr = (char *)calloc(data->max, sizeof(char));
  data->cbMax = 3;
  data->cbLen = 0;
  data->cb = (struct pg_keyboard_dataCbStruct**)malloc(data->cbMax * sizeof(struct pg_keyboard_dataCbStruct));


  ////////////////////////////
  // KEYBOARD LAYOUT CONFIGURATION
  //
  // layoutRowsLen - Number of total keyboard rows inside box
  // layoutRowsWid - Array aligned with rows used to calculate width of keys (width / layoutRowsWid[0])
  // layoutRows - Array aligned with rows and ->layout array for real number of key values in layout array
  //              this array should total layoutLen
  // layoutLen - total number of values in layout
  // layoutEl - gslc_tsElemRef pointers for keys
  // layout - ascii layout values
  data->layoutRowsLen = 5;
  int layoutRowsWid[5] = { 10, 10, 20, 9, 12 };
  int layoutRows[5] = { 10, 10, 19, 8, 12 };

  data->layoutRowsWid = (int *)malloc(data->layoutRowsLen * sizeof(int));
  data->layoutRows = (int *)malloc(data->layoutRowsLen * sizeof(int));
  for (int lR = 0; lR < data->layoutRowsLen; ++lR) {
    data->layoutRowsWid[lR] = layoutRowsWid[lR];
    data->layoutRows[lR] = layoutRows[lR];
  }

  data->layoutLen = 59;
  data->layoutEl = (gslc_tsElemRef**)malloc(data->layoutLen * sizeof(gslc_tsElemRef *));
  int layout[59] = {
    49, 50, 51, 52, 53, 54, 55, 56, 57, 48,
    81, 87, 69, 82, 84, 89, 85, 73, 79, 80,
    -1, 65, 65, 83, 83, 68, 68, 70, 70, 71, 71, 72, 72, 74, 74, 75, 75, 76, 76,
    -1, 90, 88, 67, 86, 66, 78, 77,
    15, 15, -1, 32, 32, 32, 32, 32, 32, -1, 13, 13
  };

  data->layout = (int *)malloc(data->layoutLen * sizeof(int));
  for (int l = 0; l < data->layoutLen; ++l) {
    data->layout[l] = layout[l];
  }

  return data;
};


void PG_KEYBOARD_DESTROY_DATA(struct pg_keyboard_dataStruct *data) {

  free(data->ptr);

  free(data);
}



int pg_keyboard_appendCb(void (*function)(gslc_tsGui *, char*)) {

  if (pg_keyboard_data->cbLen < pg_keyboard_data->cbMax) {
    struct pg_keyboard_dataCbStruct* cb = (struct pg_keyboard_dataCbStruct*)malloc(sizeof(struct pg_keyboard_dataCbStruct));
    cb->id = pg_keyboard_data->cbLen;
    cb->ptr = function;
    pg_keyboard_data->cb[pg_keyboard_data->cbLen] = cb;
    ++pg_keyboard_data->cbLen;
    return pg_keyboard_data->cbLen - 1;
  }
  return -1;
}

void pg_keyboard_runCb(gslc_tsGui *pGui) {
  for (size_t i = 0; i < pg_keyboard_data->cbLen; ++i) {
    pg_keyboard_data->cb[i]->ptr(pGui, pg_keyboard_data->ptr);
  }
}


void pg_keyboard_reset(gslc_tsGui *pGui) {
  // Clear Callbacks
  if (pg_keyboard_data != NULL && pg_keyboard_data->cbLen > 0) {
    for (size_t i = 0; i < pg_keyboard_data->cbLen; ++i) {
      pg_keyboard_data->cb[i]->ptr = NULL;
    }
    pg_keyboard_data->cbLen = 0;
  }

  // Clean Input
  CLEAR(pg_keyboard_data->ptr, pg_keyboard_data->max);
  gslc_ElemSetTxtStr(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], pg_keyboard_data->ptr);
  pg_keyboard_limitCheck(pGui);
}

void pg_keyboard_show(gslc_tsGui *pGui, int maxLen, char* str, void (*function)(gslc_tsGui *, char *)) {
  touchscreenPageOpen(pGui, E_PG_KEYBOARD);
  pg_keyboard_reset(pGui);
  pg_keyboard_appendCb(function);

  pg_keyboard_data->limit = maxLen;
  if (pg_keyboard_data->limit > pg_keyboard_data->max) {
    pg_keyboard_data->max = pg_keyboard_data->limit + 10; // 10 chars Extra past max
    char *newBuf = (char *)realloc(pg_keyboard_data->ptr, pg_keyboard_data->max * sizeof(char));
    pg_keyboard_data->ptr = newBuf;
  }

  size_t strSz = snprintf(NULL, 0, "%s", str) + 1;
  if (strSz > pg_keyboard_data->max) {
    strSz = pg_keyboard_data->max;
  }
  strlcpy(pg_keyboard_data->ptr, str, strSz);
  pg_keyboard_data->len = strSz - 1;
  gslc_ElemSetTxtStr(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], pg_keyboard_data->ptr);
  pg_keyboard_limitCheck(pGui);
}


void pg_keyboard_close(gslc_tsGui *pGui) {
  if (m_page_previous > -1) {
    pg_keyboard_reset(pGui);
    touchscreenPageClose(pGui, E_PG_KEYBOARD);
    touchscreenPageOpen(pGui, m_page_previous);
  }
}


void pg_keyboard_limitCheck(gslc_tsGui *pGui) {
  if (pg_keyboard_data->len > pg_keyboard_data->limit) {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_COL_RED_DK1);
  } else {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_COL_GREEN);
  }
}



////////////////////////////
// Keyboard PAge Button callbacks


// Shift Key
bool pg_keyboard_cbBtn_shift(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  return true;
}

// Enter Key
bool pg_keyboard_cbBtn_enter(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // pg_keyboardInput[strlen(pg_keyboardInput)] = '\0';
  pg_keyboard_runCb(pGui);
  pg_keyboard_close(pGui);
  return true;
}



// Spacebar Key
bool pg_keyboard_cbBtn_spacebar(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
  if (pg_keyboard_data->len < pg_keyboard_data->max) {
    strlcat(pg_keyboard_data->ptr, " ", 1);
    ++pg_keyboard_data->len;
    gslc_ElemSetTxtStr(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], pg_keyboard_data->ptr);
  }

  return true;
}


// Delete Key
bool pg_keyboard_cbBtn_delete(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (pg_keyboard_data->len > 0) {
    pg_keyboard_data->ptr[pg_keyboard_data->len - 1] = '\0';
    --pg_keyboard_data->len;
    gslc_ElemSetTxtStr(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], pg_keyboard_data->ptr);
    pg_keyboard_limitCheck(pGui);
  }
  
  
  return true;
}




// All Else
bool pg_keyboard_cbBtn(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = pElemRef->pElem;

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // printf("Pressed: %d\n", pElem->nId);
    int keyId = -1;
    for (int k = 0; k < pg_keyboard_data->layoutLen; ++k) {
      if (pg_keyboard_data->layout[k] == -1) { continue; }
      if (pg_keyboard_data->layoutEl[k] == NULL) { continue; }

      if (pg_keyboard_data->layoutEl[k]->pElem->nId == pElem->nId) {
        keyId = k;
        break;
      }
    }

    if (keyId > -1) {
      int xLen = 2;
      char* x = NULL;
      switch (pg_keyboard_data->layout[keyId]) {
        case -1:
          return 1;
        break;
        case 8: // Backspace
          return pg_keyboard_cbBtn_delete(pvGui, pvElemRef, eTouch, nX, nY);
        break;
        case 13: // System Enter
          // Do Enter Stuff
          return pg_keyboard_cbBtn_enter(pvGui, pvElemRef, eTouch, nX, nY);
        break;
        case 15: // system Shift

          return 1;
        break;
        default: // Everything Else
          xLen = snprintf(NULL, 0, "%c", pg_keyboard_data->layout[keyId]) + 1;
          x = (char *)calloc(xLen, sizeof(char));
          snprintf(x, xLen, "%c", pg_keyboard_data->layout[keyId]);

          // realloc ptr
          if (pg_keyboard_data->len >= pg_keyboard_data->max) {
            pg_keyboard_data->max = pg_keyboard_data->len + 128;
            char *newBuf = (char *)realloc(pg_keyboard_data->ptr, pg_keyboard_data->max * sizeof(char));
            pg_keyboard_data->ptr = newBuf;
          }

          
          if (pg_keyboard_data->len == 0) {
            pg_keyboard_data->len += xLen;
            strlcpy(pg_keyboard_data->ptr, x, pg_keyboard_data->len);
          } else {
            pg_keyboard_data->len += (xLen - 1);
            strlcat(pg_keyboard_data->ptr, x, pg_keyboard_data->len);
          }
        break;
      }

      // // debug_print("%d - %d -- %s - Id: %d - %d\n", strlen(pg_keyboard_data->ptr), sizeof(*pg_keyboard_data->ptr), x, keyboardLayout[klI], pElem->nId);
      gslc_ElemSetTxtStr(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], pg_keyboard_data->ptr);
      pg_keyboard_limitCheck(pGui);
    }
    
  }

  return 1;
}





bool pg_keyboard_draw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  gslc_DrawFillRect(pGui,pRect,pElem->colElemFill);
  gslc_DrawLine(pGui, pRect.x, pRect.y + 60, pRect.x + pRect.w, pRect.y + 60, GSLC_COL_GRAY);

  pg_keyboard_limitCheck(pGui);

  gslc_ElemDrawByRef(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_REDRAW_FULL);




  gslc_ElemSetRedraw(pGui, pg_keyboardEl[E_KEYBOARD_EL_BOX], GSLC_REDRAW_NONE);
  return true;
}




int pg_keyboard_guiKeyboard(gslc_tsGui* pGui) {
  int ePage = E_PG_KEYBOARD;

  gslc_tsRect rFullscreen = {0,0,480,320};

  int keyY = 60;

  // W/out System Row at Bottom
  int xHei = ((rFullscreen.h - 65) / (pg_keyboard_data->layoutRowsLen));

  int keyId = 0;
  int offsetX = 0;

  for (int lR = 0; lR < pg_keyboard_data->layoutRowsLen; ++lR) {
    // New Row
    int rowLen = pg_keyboard_data->layoutRows[lR];
    int xWid = (rFullscreen.w / pg_keyboard_data->layoutRowsWid[lR]);

    for (int l = 0; l < rowLen; ++l) {
      int xLen = 2;
      char* x = NULL;

      switch (pg_keyboard_data->layout[keyId]) {
        case -1:
          ++keyId;
          continue;
        break;
        case 13: // System Enter
          xLen = snprintf(NULL, 0, "%s", "Enter") + 1;
          x = (char *)calloc(xLen, sizeof(char));
          snprintf(x, xLen, "%s", "Enter");
        break;
        case 15: // system Shift
          xLen = snprintf(NULL, 0, "%s", "Shift") + 1;
          x = (char *)calloc(xLen, sizeof(char));
          snprintf(x, xLen, "%s", "Shift");
        break;
        default: // Everything Else
          xLen = snprintf(NULL, 0, "%c", pg_keyboard_data->layout[keyId]) + 1;
          x = (char *)calloc(xLen, sizeof(char));
          snprintf(x, xLen, "%c", pg_keyboard_data->layout[keyId]);
        break;
      }

      if (keyId < pg_keyboard_data->layoutLen - 1
         && pg_keyboard_data->layout[keyId + 1] == pg_keyboard_data->layout[keyId]) {
        offsetX += xWid;
        pg_keyboard_data->layoutEl[keyId] = NULL;
      } else {
        // New Key In Row
        pg_keyboard_data->layoutEl[keyId] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
                (gslc_tsRect) {(rFullscreen.x + (l * xWid)) - offsetX, (keyY + (lR * xHei)), xWid + offsetX, xHei},
                x, xLen, E_FONT_MONO14, &pg_keyboard_cbBtn);
        gslc_ElemSetTxtCol(pGui, pg_keyboard_data->layoutEl[keyId], GSLC_COL_WHITE);
        gslc_ElemSetCol(pGui, pg_keyboard_data->layoutEl[keyId], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
        gslc_ElemSetTxtAlign(pGui, pg_keyboard_data->layoutEl[keyId], GSLC_ALIGN_MID_MID);
        gslc_ElemSetFillEn(pGui, pg_keyboard_data->layoutEl[keyId], true);
        gslc_ElemSetFrameEn(pGui, pg_keyboard_data->layoutEl[keyId], true);

        offsetX = 0;
      }

      if (x != NULL) { free(x); }
      ++keyId;
    }

  }

  return 1;
}

int pg_keyboard_guiInit(gslc_tsGui* pGui)
{

  int ePage = E_PG_KEYBOARD;
  gslc_tsRect rFullscreen = {0,0,480,320};

  gslc_PageAdd(&m_gui, ePage, pg_keyboardElem, MAX_ELEM_PG_KEYBOARD_RAM, pg_keyboardElemRef, MAX_ELEM_PG_KEYBOARD);
  // gslc_SetBkgndColor(&m_gui, GSLC_COL_BLACK);

  // Overall Redraw Box
  if ((
    pg_keyboardEl[E_KEYBOARD_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_BOX], GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetDrawFunc(pGui, pg_keyboardEl[E_KEYBOARD_EL_BOX], &pg_keyboard_draw);
  }

  // Keyboard Input Box
  if ((
    pg_keyboardEl[E_KEYBOARD_EL_INPUT] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y, (rFullscreen.w - 108), 60},
          (char *)"", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], true);
    gslc_ElemSetFrameEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], false);
  }

  // Delete Key
  if ((
    pg_keyboardEl[E_KEYBOARD_EL_DELETE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 100),(rFullscreen.y + 5),100,50},
            (char *)"Backspace", 10, E_FONT_MONO14, &pg_keyboard_cbBtn_delete)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], false);
    gslc_ElemSetFrameEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], true);
  }

  return 1;
}





// GUI Init
void pg_keyboard_init(gslc_tsGui *pGui) {
  pg_keyboard_default = PG_KEYBOARD_INIT_DATA();
  pg_keyboard_guiInit(pGui);

  cbInit[E_PG_KEYBOARD] = NULL;
}


// GUI Open
void pg_keyboard_open(gslc_tsGui *pGui) {
  pg_keyboard_data = PG_KEYBOARD_INIT_DATA();
  pg_keyboard_guiKeyboard(pGui);
  // gslc_ElemSetTxtStr(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], pg_keyboard_data->ptr);
}


// GUI Destroy
void pg_keyboard_destroy(gslc_tsGui *pGui) {
  PG_KEYBOARD_DESTROY_DATA(pg_keyboard_data);
}

void __attribute__ ((constructor)) pg_keyboard_setup(void) {
  cbInit[E_PG_KEYBOARD] = &pg_keyboard_init;
  cbOpen[E_PG_KEYBOARD] = &pg_keyboard_open;
  cbDestroy[E_PG_KEYBOARD] = &pg_keyboard_destroy;
}
