#ifndef _PAGES_MAIN_H_
#define _PAGES_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_MAIN_EL_BOX,
  E_MAIN_EL_OPEN_STARTX,
  E_MAIN_EL_OPEN_SLIDESHOW,
  E_MAIN_EL_OPEN_SKYDIVEORBUST,
  E_MAIN_EL_TXT_TMP,
  E_MAIN_EL_BTN_TMP,
  E_MAIN_EL_BTN_TMPKB,

  E_MAIN_EL_MAX
};

gslc_tsElem m_asPgMainElem[MAX_ELEM_PG_DEFAULT_RAM];
gslc_tsElemRef m_asPgMainElemRef[MAX_ELEM_PG_DEFAULT];

gslc_tsElemRef* pg_mainEl[E_MAIN_EL_MAX];

void pg_mainButtonRotaryCW();
void pg_mainButtonRotaryCCW();
void pg_mainButtonLeftPressed();
void pg_mainButtonRightPressed();
void pg_mainButtonRotaryPressed();
void pg_mainButtonLeftHeld();
void pg_mainButtonRightHeld();
void pg_mainButtonRotaryHeld();
void pg_mainButtonDoubleHeld();
void pg_mainButtonSetFuncs();

void pg_main_init(gslc_tsGui *pGui);
void pg_main_open(gslc_tsGui *pGui);
void pg_main_destroy(gslc_tsGui *pGui);
void pg_main(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_MAIN_H_