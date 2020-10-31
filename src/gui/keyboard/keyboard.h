#ifndef _VKEYBOARD_H_
#define _VKEYBOARD_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

//////////////////
// GUI Page
#define MAX_ELEM_PG_KEYBOARD      100
#define MAX_ELEM_PG_KEYBOARD_RAM  MAX_ELEM_PG_KEYBOARD

enum {
  E_KEYBOARD_EL_BOX,
  E_KEYBOARD_EL_CANCEL,
  E_KEYBOARD_EL_INPUT,
  E_KEYBOARD_EL_DELETE,
  E_KEYBOARD_EL_SYMBOLS,

  E_KEYBOARD_EL_MAX
};

// struct for holding callback info when enter is pressed
struct pg_keyboard_dataCbStruct {
  int id;
  void (*ptr)(gslc_tsGui *, char*);
};

// Current layout configuration for keyboard
struct pg_keyboard_dataStruct {
  int max;
  int limit;
  int len;
  char *ptr;

  int cbMax;
  int cbLen;
  struct pg_keyboard_dataCbStruct **cb;


  int layoutRowsLen;
  int *layoutRowsWid;
  int *layoutRows;

  int layoutLen;
  int *layout;

  gslc_tsElemRef **layoutEl;
};
struct pg_keyboard_dataStruct *pg_keyboard_data;
struct pg_keyboard_dataStruct *pg_keyboard_default;

gslc_tsElem pg_keyboardElem[MAX_ELEM_PG_KEYBOARD_RAM];
gslc_tsElemRef pg_keyboardElemRef[MAX_ELEM_PG_KEYBOARD];

gslc_tsElemRef* pg_keyboardEl[E_KEYBOARD_EL_MAX];


int pg_keyboard_shiftOn;
int pg_keyboard_symbolOn;


#include <stdio.h>
#include <string.h>

#include "keyboard.h"
#include "libs/shared.h"
#include "gui/pages.h"



// Upper Case American Layout
struct pg_keyboard_dataStruct * pg_keyboard_def_upperCase(gslc_tsGui* pGui, struct pg_keyboard_dataStruct *data);
// Lower Case American Layout
struct pg_keyboard_dataStruct * pg_keyboard_def_lowerCase(gslc_tsGui* pGui, struct pg_keyboard_dataStruct *data);
// Symbol Layout
struct pg_keyboard_dataStruct * pg_keyboard_def_symbols(gslc_tsGui* pGui, struct pg_keyboard_dataStruct *data);

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
      int *layout, int layoutLen);

struct pg_keyboard_dataStruct * PG_KEYBOARD_INIT_DATA(gslc_tsGui* pGui);
void PG_KEYBOARD_DESTROY_DATA(struct pg_keyboard_dataStruct *data);

////////////////////////////
// CALLBACK MANAGEMENT
int pg_keyboard_appendCb(struct pg_keyboard_dataStruct *data, void (*function)(gslc_tsGui *, char*));
void pg_keyboard_runCb(gslc_tsGui *pGui, struct pg_keyboard_dataStruct *data);
void pg_keyboard_cleanCb(struct pg_keyboard_dataStruct *data);

void pg_keyboard_reset(gslc_tsGui *pGui);
void pg_keyboard_show(gslc_tsGui *pGui, int maxLen, char* str, void (*function)(gslc_tsGui *, char *));
void pg_keyboard_limitCheck(gslc_tsGui *pGui);
void pg_keyboard_setCase();

////////////////////////////
// KEYBOARD BUTTON CALLBACKS
bool pg_keyboard_cbBtn_shift(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_keyboard_cbBtn_enter(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_keyboard_cbBtn_delete(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_keyboard_cbBtn_cancel(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_keyboard_cbBtn(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);

////////////////////////////
// DRAW KEYBOARD ELEMENTS
// Create GUISlice KeyBtn Element
gslc_tsElemRef* pg_keyboard_guiKeyboard_CreateElem(gslc_tsGui* pGui);
// Draw ticker
bool pg_keyboard_draw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);

// Update Keyboard Buttons with Latest Display and Locations
int pg_keyboard_guiKeyboardUpdate(gslc_tsGui* pGui);
// Initalize GUIslice Elements needed for buttons (pooled)
int pg_keyboard_guiKeyboard(gslc_tsGui* pGui);

void pg_keyboard_setLowercase(gslc_tsGui *pGui);
void pg_keyboard_setUppercase(gslc_tsGui *pGui);


////////////////////////////
// KEYBOARD BASELINE GUISLICE LAYOUT
int pg_keyboard_guiInit(gslc_tsGui* pGui);

////////////////////////////
// KEYBOARD DEFAULT INIT FUNCTIONS
void pg_keyboard_init(gslc_tsGui *pGui);
void pg_keyboard_open(gslc_tsGui *pGui);
void pg_keyboard_close(gslc_tsGui *pGui);
void pg_keyboard_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_keyboard_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _VKEYBOARD_H_