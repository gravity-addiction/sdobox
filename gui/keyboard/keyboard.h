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
  E_KEYBOARD_EL_INPUT,
  E_KEYBOARD_EL_DELETE,

  E_KEYBOARD_EL_MAX
};

struct pg_keyboard_dataCbStruct {
  int id;
  void (*ptr)(gslc_tsGui *, char*);
};

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

struct pg_keyboard_dataStruct * pg_keyboard_def_upperCase(struct pg_keyboard_dataStruct *data);
struct pg_keyboard_dataStruct * pg_keyboard_def_lowerCase(struct pg_keyboard_dataStruct *data);

struct pg_keyboard_dataStruct * pg_keyboard_layoutConfig(struct pg_keyboard_dataStruct *data, 
      int *layoutRowsWid, int *layoutRows, int layoutRowsLen,
      int *layout, int layoutLen);


struct pg_keyboard_dataStruct * PG_KEYBOARD_INIT_DATA();
void PG_KEYBOARD_DESTROY_DATA(struct pg_keyboard_dataStruct *data);

int pg_keyboard_appendCb(struct pg_keyboard_dataStruct *data, void (*function)(gslc_tsGui *, char*));
void pg_keyboard_runCb(gslc_tsGui *pGui, struct pg_keyboard_dataStruct *data);
void pg_keyboard_cleanCb(struct pg_keyboard_dataStruct *data);

void pg_keyboard_reset(gslc_tsGui *pGui);
void pg_keyboard_show(gslc_tsGui *pGui, int maxLen, char* str, void (*function)(gslc_tsGui *, char *));
void pg_keyboard_limitCheck(gslc_tsGui *pGui);

bool pg_keyboard_cbBtn_shift(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_keyboard_cbBtn_enter(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_keyboard_cbBtn_spacebar(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_keyboard_cbBtn_delete(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_keyboard_cbBtn(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);

bool pg_keyboard_draw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);
int pg_keyboard_guiKeyboardUpdate(gslc_tsGui* pGui);
int pg_keyboard_guiKeyboard(gslc_tsGui* pGui);
int pg_keyboard_guiInit(gslc_tsGui* pGui);

void pg_keyboard_init(gslc_tsGui *pGui);
void pg_keyboard_open(gslc_tsGui *pGui);
void pg_keyboard_close(gslc_tsGui *pGui);
void pg_keyboard_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_keyboard_setup(void);

bool CbBtnKeyboard(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _VKEYBOARD_H_