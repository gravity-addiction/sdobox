#include <stdio.h>
#include <string.h>

#include "keyboard.h"
#include "libs/shared.h"
#include "gui/pages.h"



// Upper Case American Layout
struct pg_keyboard_dataStruct * pg_keyboard_def_upperCase(gslc_tsGui* pGui, struct pg_keyboard_dataStruct *data) {
  // Initalize Default Layout
  int layoutRowWids[5] = { 10, 10, 20, 9, 12 };
  int layoutRows[5] = { 10, 10, 19, 8, 12 };
  int layout[59] = {
    33, 64, 35, 36, 37, 94, 38, 42, 40, 41,
    81, 87, 69, 82, 84, 89, 85, 73, 79, 80,
    -1, 65, 65, 83, 83, 68, 68, 70, 70, 71, 71, 72, 72, 74, 74, 75, 75, 76, 76,
    -1, 90, 88, 67, 86, 66, 78, 77,
    15, 15, -1, 32, 32, 32, 32, 32, 32, -1, 13, 13
  };

  return pg_keyboard_layoutConfig(pGui, data, layoutRowWids, layoutRows, 5, layout, 59);
}

// Lower Case American Layout
struct pg_keyboard_dataStruct * pg_keyboard_def_lowerCase(gslc_tsGui* pGui, struct pg_keyboard_dataStruct *data) {
  // Initalize Default Layout
  int layoutRowWids[5] = { 10, 10, 20, 9, 12 };
  int layoutRows[5] = { 10, 10, 19, 8, 12 };
  int layout[59] = {
    49, 50, 51, 52, 53, 54, 55, 56, 57, 48,
    113, 119, 101, 114, 116, 121, 117, 105, 111, 112,
    -1, 97, 97, 115, 115, 100, 100, 102, 102, 103, 103, 104, 104, 106, 106, 107, 107, 108, 108,
    -1, 122, 120, 99, 118, 98, 110, 109,
    15, 15, -1, 32, 32, 32, 32, 32, 32, -1, 13, 13
  };

  return pg_keyboard_layoutConfig(pGui, data, layoutRowWids, layoutRows, 5, layout, 59);
}

// Lower Case American Layout
struct pg_keyboard_dataStruct * pg_keyboard_def_symbols(gslc_tsGui* pGui, struct pg_keyboard_dataStruct *data) {
  // Initalize Default Layout
  int layoutRowWids[5] = { 10, 10, 20, 9, 12 };
  int layoutRows[5] = { 10, 10, 19, 8, 12 };
  int layout[59] = {
    185, 178, 179, -1, -1, -1, 188, 189, 190, 186,
    96, 126, 123, 125, 124, 91, 93, 61, 43, 45,
    -1, 176, 176, -1, -1, 171, 171, 187, 187, 95, 95, 59, 59, 58, 58, 39, 39, 34, 34,
    -1, 92, 60, 62, 44, 46, 47, 63,
    15, 15, -1, 32, 32, 32, 32, 32, 32, -1, 13, 13
  };

  return pg_keyboard_layoutConfig(pGui, data, layoutRowWids, layoutRows, 5, layout, 59);
}


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
struct pg_keyboard_dataStruct * pg_keyboard_layoutConfig(gslc_tsGui* pGui, struct pg_keyboard_dataStruct *data,
      int *layoutRowsWid, int *layoutRows, int layoutRowsLen,
      int *layout, int layoutLen) {

  // *todo Cleaout old config
  for (int l = 0; l < data->layoutLen; ++l) {
    data->layout[l] = -1;
    gslc_ElemSetTxtStr(pGui, data->layoutEl[l], (char*)" ");
  }
  for (int lR = 0; lR < data->layoutRowsLen; ++lR) {
    data->layoutRowsWid[lR] = -1;
    data->layoutRows[lR] = -1;
  }


  // Make sure enough rows to cover this config
  if (data->layoutRowsLen < layoutRowsLen) {
    int * newRowsWid = (int *)realloc(data->layoutRowsWid, layoutRowsLen * sizeof(int));
    data->layoutRowsWid = newRowsWid;

    int * newRows = (int *)realloc(data->layoutRows, layoutRowsLen * sizeof(int));
    data->layoutRows = newRows;
  }
  data->layoutRowsLen = layoutRowsLen;

  // Set each Row Width and Count
  for (int lR = 0; lR < data->layoutRowsLen; ++lR) {
    data->layoutRowsWid[lR] = layoutRowsWid[lR];
    data->layoutRows[lR] = layoutRows[lR];
  }

  // Make sure enough space for all the keys in layout
  if (data->layoutLen < layoutLen) {
    data->layoutEl = (gslc_tsElemRef**)realloc(data->layoutEl, layoutLen * sizeof(gslc_tsElemRef *));
    data->layout = (int *)realloc(data->layout, layoutLen * sizeof(int));
    for (int lN = data->layoutLen; lN < layoutLen; ++lN) {
      data->layout[lN] = -1;
      data->layoutEl[lN] = pg_keyboard_guiKeyboard_CreateElem(pGui);
    }
  }
  data->layoutLen = layoutLen;

  // remap all layout keys
  for (int l = 0; l < data->layoutLen; ++l) {
    data->layout[l] = layout[l];
  }

  return data;
}




struct pg_keyboard_dataStruct * PG_KEYBOARD_INIT_DATA(gslc_tsGui* pGui)
{
  struct pg_keyboard_dataStruct *data = (struct pg_keyboard_dataStruct*)malloc(sizeof(struct pg_keyboard_dataStruct));

  data->max = 128;
  data->limit = 100;
  data->len = 0;

  // Initalize Callback Pool
  data->ptr = (char *)calloc(data->max, sizeof(char));
  data->cbMax = 3;
  data->cbLen = 0;
  data->cb = (struct pg_keyboard_dataCbStruct**)malloc(data->cbMax * sizeof(struct pg_keyboard_dataCbStruct));
  for (int c = 0; c < data->cbMax; ++c) {
    data->cb[c] = NULL;
  }


  // Initialize Keyboard Row Layout
  data->layoutRowsLen = 1;
  data->layoutRowsWid = (int *)malloc(data->layoutRowsLen * sizeof(int));
  data->layoutRows = (int *)malloc(data->layoutRowsLen * sizeof(int));
  for (int lR = 0; lR < data->layoutRowsLen; ++lR) {
    data->layoutRowsWid[lR] = -1;
    data->layoutRows[lR] = -1;
  }

  // Initialize Value Values
  data->layoutLen = 1;
  data->layoutEl = (gslc_tsElemRef**)malloc(data->layoutLen * sizeof(gslc_tsElemRef *));
  data->layout = (int *)malloc(data->layoutLen * sizeof(int));
  for (int l = 0; l < data->layoutLen; ++l) {
    data->layout[l] = -1;
    data->layoutEl[l] = pg_keyboard_guiKeyboard_CreateElem(pGui);
  }

  return data;
};


void PG_KEYBOARD_DESTROY_DATA(struct pg_keyboard_dataStruct *data) {

  free(data->layout);

  free(data->layoutEl);

  free(data->layoutRowsWid);
  free(data->layoutRows);

  // free cb
  pg_keyboard_cleanCb(data);
  free(data->cb);

  free(data->ptr);
  free(data);
}





void pg_keyboard_update_input(gslc_tsGui *pGui, char* str) {
  char underscore[] = "_";
  gslc_ElemSetTxtStr(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], strcat(str, underscore));
  
}

// Reset Majority of keyboard information
void pg_keyboard_reset(gslc_tsGui *pGui) {
  // Clear Callbacks
  pg_keyboard_cleanCb(pg_keyboard_data);

  // Clean Input
  CLEAR(pg_keyboard_data->ptr, pg_keyboard_data->max);
  pg_keyboard_update_input(pGui, pg_keyboard_data->ptr);
}





// Public function for showing the keyboard with config settings
void pg_keyboard_show(gslc_tsGui *pGui, int maxLen, char* str, void (*function)(gslc_tsGui *, char *)) {
  touchscreenPageOpen(pGui, E_PG_KEYBOARD);
  pg_keyboard_reset(pGui);
  pg_keyboard_appendCb(pg_keyboard_data, function);

  pg_keyboard_data->limit = maxLen;
  if (pg_keyboard_data->limit > pg_keyboard_data->max) {
    pg_keyboard_data->max = pg_keyboard_data->limit + 10; // 10 chars Extra past max
    char *newBuf = (char *)realloc(pg_keyboard_data->ptr, pg_keyboard_data->max * sizeof(char));
    pg_keyboard_data->ptr = newBuf;
  }

  size_t strSz = 1;
  if (str != NULL && strcmp(str, " ") != 0) {
    strSz = snprintf(NULL, 0, "%s", str) + 1;
    if (strSz > pg_keyboard_data->max) {
      strSz = pg_keyboard_data->max;
    }
  }
  strlcpy(pg_keyboard_data->ptr, str, strSz);
  pg_keyboard_data->len = strSz - 2;
  pg_keyboard_update_input(pGui, pg_keyboard_data->ptr);
  pg_keyboard_limitCheck(pGui);
}



// Check user supplied charactor limit on text
void pg_keyboard_limitCheck(gslc_tsGui *pGui) {
  if (pg_keyboard_data->len > pg_keyboard_data->limit) {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_COL_RED_DK1);
  } else {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_COL_GREEN);
  }
}


void pg_keyboard_setCase(gslc_tsGui *pGui) {
  // Set Keys to value
  if (!pg_keyboard_shiftOn) {
    // Set Upper Case
    pg_keyboard_data = pg_keyboard_def_upperCase(pGui, pg_keyboard_data);
  } else {
    // Set Lower Case
    pg_keyboard_data = pg_keyboard_def_lowerCase(pGui, pg_keyboard_data);
  }
}

void pg_keyboard_setSymbols(gslc_tsGui *pGui, int doSymbols) {
  // do toggling
  if (doSymbols && pg_keyboard_symbolOn) {
    doSymbols = 0;
  }

  if (doSymbols) {
    pg_keyboard_data = pg_keyboard_def_symbols(pGui, pg_keyboard_data);
    pg_keyboard_symbolOn = 1;
    gslc_ElemSetFillEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_SYMBOLS], true);
  } else {
    pg_keyboard_symbolOn = 0;
    gslc_ElemSetFillEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_SYMBOLS], false);
    pg_keyboard_setCase(pGui);
  }
}

void pg_keyboard_setLowercase(gslc_tsGui *pGui) {
  pg_keyboard_shiftOn = 0;
  pg_keyboard_guiKeyboardUpdate(pGui);
}

void pg_keyboard_setUppercase(gslc_tsGui *pGui) {
  pg_keyboard_shiftOn = 1;
  pg_keyboard_guiKeyboardUpdate(pGui);
}








////////////////////////////
// CALLBACK MANAGEMENT
// Append callback, returning assigned id
int pg_keyboard_appendCb(struct pg_keyboard_dataStruct *data, void (*function)(gslc_tsGui *, char*)) {

  if (data->cbLen < data->cbMax) {
    struct pg_keyboard_dataCbStruct *cb = (struct pg_keyboard_dataCbStruct*)malloc(sizeof(struct pg_keyboard_dataCbStruct));
    cb->id = data->cbLen;
    cb->ptr = function;
    data->cb[data->cbLen] = cb;
    return data->cbLen++;
  }
  return -1;
}

// Run Callback
void pg_keyboard_runCb(gslc_tsGui *pGui, struct pg_keyboard_dataStruct *data) {
  for (size_t i = 0; i < data->cbLen; ++i) {
    if (data->cb[i]->ptr != NULL) {
      data->cb[i]->ptr(pGui, data->ptr);
    }
  }
}

// Clean Callback
void pg_keyboard_cleanCb(struct pg_keyboard_dataStruct *data) {
  for (size_t i = 0; i < data->cbLen; ++i) {
    free(data->cb[i]);
    data->cb[i] = NULL;
  }
  data->cbLen = 0;
}















////////////////////////////
// KEYBOARD BUTTON ACTIONS
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
  pg_keyboard_data->ptr[pg_keyboard_data->len + 1] = '\0';
  pg_keyboard_runCb(pGui, pg_keyboard_data);
  touchscreenPageGoBack(pGui);
  return true;
}


// Delete Key
bool pg_keyboard_cbBtn_delete(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (pg_keyboard_data->len > -1) {
    pg_keyboard_data->ptr[pg_keyboard_data->len] = '\0';
    pg_keyboard_data->len -= 1;
    pg_keyboard_update_input(pGui, pg_keyboard_data->ptr);
    pg_keyboard_limitCheck(pGui);
  }


  return true;
}

// Cancel Key
bool pg_keyboard_cbBtn_cancel(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageGoBack(pGui);
  return true;
}

// Symbols Key
bool pg_keyboard_cbBtn_symbols(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_keyboard_setSymbols(pGui, 1);

  pg_keyboard_guiKeyboardUpdate(pGui);
  return true;
}

// All Else
bool pg_keyboard_cbBtn(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = pElemRef->pElem;


  if ( eTouch == GSLC_TOUCH_UP_IN ) {

    // Find Matching Layout GSLC_ID
    int keyId = -1;
    for (int k = 0; k < pg_keyboard_data->layoutLen; ++k) {
      if (pg_keyboard_data->layout[k] == -1) { continue; }

      if (pg_keyboard_data->layoutEl[k]->pElem->nId == pElem->nId) {
        keyId = k;
        break;
      }
    }

    // Got Match, cat value
    if (keyId > -1) {
      // printf("Ascii: %d\n", pg_keyboard_data->layout[keyId]);
      int xLen = 2;
      char* x = NULL;
      switch (pg_keyboard_data->layout[keyId]) {
        case -1:
          return 1;
        break;
        case 13: // System Enter
          // Do Enter Stuff
          return pg_keyboard_cbBtn_enter(pvGui, pvElemRef, eTouch, nX, nY);
        break;
        case 15: // Shift
          // Determine Shift Toggle
          if (pg_keyboard_data->layout[keyId] == 15 && pg_keyboard_shiftOn) {
            pg_keyboard_shiftOn = 0;
          } else if (pg_keyboard_data->layout[keyId] == 15) {
            pg_keyboard_shiftOn = 1;
          }

          // Set Keys to value
          pg_keyboard_setSymbols(pGui, 0);
          // pg_keyboard_setCase(pGui); // false symbols runs setCase
          pg_keyboard_guiKeyboardUpdate(pGui);
          return 1;
        break;
        case 8:
        case 127: // Delete
          if (pg_keyboard_data->len > -1) {
            pg_keyboard_data->ptr[pg_keyboard_data->len] = '\0';
            pg_keyboard_data->len -= 1;
          }
        break;
        default: // Everything Else
          xLen = snprintf(NULL, 0, "%c", pg_keyboard_data->layout[keyId]) + 1;
          x = (char *)calloc(xLen, sizeof(char));
          snprintf(x, xLen, "%c", pg_keyboard_data->layout[keyId]);
          pg_keyboard_data->len += (xLen - 1);

          // realloc ptr
          if (pg_keyboard_data->len >= pg_keyboard_data->max) {
            pg_keyboard_data->max = pg_keyboard_data->len + 128;
            char *newBuf = (char *)realloc(pg_keyboard_data->ptr, pg_keyboard_data->max * sizeof(char));
            pg_keyboard_data->ptr = newBuf;
          }

          pg_keyboard_data->ptr[pg_keyboard_data->len] = *x;
          // strlcat(pg_keyboard_data->ptr, x, pg_keyboard_data->len);

          free(x);
        break;
      }

      // update viewport with new pointer value
      pg_keyboard_update_input(pGui, pg_keyboard_data->ptr);
      pg_keyboard_limitCheck(pGui);
    }

  }

  return 1;
}















////////////////////////////
// DRAW KEYBOARD ELEMENTS
// Create GUISlice KeyBtn Element
gslc_tsElemRef* pg_keyboard_guiKeyboard_CreateElem(gslc_tsGui* pGui) {
  int ePage = E_PG_KEYBOARD;

  gslc_tsElemRef* layoutEl = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {0, 0, 1, 1},
          (char*)"", 0, E_FONT_MONO24, &pg_keyboard_cbBtn);
  gslc_ElemSetTxtCol(pGui, layoutEl, GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, layoutEl, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLUE);
  gslc_ElemSetTxtAlign(pGui, layoutEl, GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, layoutEl, true);
  gslc_ElemSetFrameEn(pGui, layoutEl, true);
  return layoutEl;
}







// Update Keyboard Buttons with Latest Display and Locations
int pg_keyboard_guiKeyboardUpdate(gslc_tsGui* pGui) {

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
          gslc_ElemSetTxtStr(pGui, pg_keyboard_data->layoutEl[keyId], (char*)" ");
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
        gslc_ElemSetTxtStr(pGui, pg_keyboard_data->layoutEl[keyId], (char*)" ");
      } else {
        // New Key In Row
        pg_keyboard_data->layoutEl[keyId]->pElem->rElem = (gslc_tsRect) {(rFullscreen.x + (l * xWid)) - offsetX, (keyY + (lR * xHei)), xWid + offsetX, xHei};
        gslc_ElemSetTxtStr(pGui, pg_keyboard_data->layoutEl[keyId], x);
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

  gslc_PageAdd(&m_gui, ePage, pg_keyboardElem, MAX_ELEM_PG_KEYBOARD_RAM, pg_keyboardElemRef, MAX_ELEM_PG_KEYBOARD);

  pg_keyboardEl[E_KEYBOARD_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullscreen);
  gslc_ElemSetCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_BOX], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);

  // Keyboard Input Box
  if ((
    pg_keyboardEl[E_KEYBOARD_EL_INPUT] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect){(rFullscreen.x + 4), rFullscreen.y, (rFullscreen.w - 112), 60},
          (char *)"", 0, E_FONT_MONO18)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_GRAY);
    gslc_ElemSetTxtAlign(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFillEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], true);
    gslc_ElemSetFrameEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_INPUT], false);
  }

  // Delete Key
  if ((
    pg_keyboardEl[E_KEYBOARD_EL_DELETE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 65),(rFullscreen.y + 5),60,50},
            (char *)"<--", 10, E_FONT_MONO14, &pg_keyboard_cbBtn_delete)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_GRAY);
    gslc_ElemSetTxtAlign(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], false);
    gslc_ElemSetFrameEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_DELETE], true);
  }

  // Cancel Key
  if ((
    pg_keyboardEl[E_KEYBOARD_EL_CANCEL] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 56),(rFullscreen.y + 213),56,50},
            (char *)"Cancel", 10, E_FONT_MONO14, &pg_keyboard_cbBtn_cancel)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_CANCEL], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_CANCEL], GSLC_COL_WHITE, GSLC_COL_GRAY, GSLC_COL_GRAY);
    gslc_ElemSetTxtAlign(pGui, pg_keyboardEl[E_KEYBOARD_EL_CANCEL], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_CANCEL], false);
    gslc_ElemSetFrameEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_CANCEL], true);
  }

  // Symbols Key
  if ((
    pg_keyboardEl[E_KEYBOARD_EL_SYMBOLS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {rFullscreen.x,(rFullscreen.y + 213),56,50},
            (char *)"Symbol", 10, E_FONT_MONO14, &pg_keyboard_cbBtn_symbols)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_SYMBOLS], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_keyboardEl[E_KEYBOARD_EL_SYMBOLS], GSLC_COL_WHITE, GSLC_COL_GRAY, GSLC_COL_GRAY);
    gslc_ElemSetTxtAlign(pGui, pg_keyboardEl[E_KEYBOARD_EL_SYMBOLS], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_SYMBOLS], false);
    gslc_ElemSetFrameEn(pGui, pg_keyboardEl[E_KEYBOARD_EL_SYMBOLS], true);
  }

  return 1;
}





// GUI Init
void pg_keyboard_init(gslc_tsGui *pGui) {
  pg_keyboard_guiInit(pGui);
  pg_keyboard_data = PG_KEYBOARD_INIT_DATA(pGui);
  pg_keyboard_shiftOn = 0;

  cbInit[E_PG_KEYBOARD] = NULL;
}


// GUI Open
void pg_keyboard_open(gslc_tsGui *pGui) {

  if (!pg_keyboard_shiftOn) {
    // Set Lower Case
    pg_keyboard_data = pg_keyboard_def_lowerCase(pGui, pg_keyboard_data);
    pg_keyboard_shiftOn = 0;
  } else {
    // Set Upper Case
    pg_keyboard_data = pg_keyboard_def_upperCase(pGui, pg_keyboard_data);
    pg_keyboard_shiftOn = 1;
  }
  // Display
  pg_keyboard_guiKeyboardUpdate(pGui);
}

void pg_keyboard_close(gslc_tsGui *pGui) {

}


// GUI Destroy
void pg_keyboard_destroy(gslc_tsGui *pGui) {
  PG_KEYBOARD_DESTROY_DATA(pg_keyboard_data);
}

void __attribute__ ((constructor)) pg_keyboard_setup(void) {
  cbInit[E_PG_KEYBOARD] = &pg_keyboard_init;
  cbOpen[E_PG_KEYBOARD] = &pg_keyboard_open;
  cbClose[E_PG_KEYBOARD] = &pg_keyboard_close;
  cbDestroy[E_PG_KEYBOARD] = &pg_keyboard_destroy;
}
