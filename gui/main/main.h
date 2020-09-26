#ifndef _PAGES_MAIN_H_
#define _PAGES_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_MAIN_CLEAN,

  E_MAIN_EL_BOX,
  E_MAIN_EL_CLOCK,
  E_MAIN_EL_BTN_SETTINGS,
  E_MAIN_EL_BTN_CHROMIUM,
  E_MAIN_EL_BTN_GALCULATOR,
  E_MAIN_EL_BTN_FILE_MANAGER,
  E_MAIN_EL_BTN_VSCODE,
  E_MAIN_EL_BTN_SPOTIFY,
  E_MAIN_EL_BTN_SLIDESHOW,
  E_MAIN_EL_BTN_COOKBOOK,
  E_MAIN_EL_BTN_PARACHUTE,
  E_MAIN_EL_VOLUME,
  E_MAIN_EL_VOLUME_DISPLAY,
  E_MAIN_EL_VOLUME_MUTE,

  E_MAIN_EL_MAX
};

#define MAX_ELEM_PG_MAIN      E_MAIN_EL_MAX + 11
#define MAX_ELEM_PG_MAIN_RAM  MAX_ELEM_PG_MAIN

gslc_tsElem m_asPgMainElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef m_asPgMainElemRef[MAX_ELEM_PG_MAIN];

gslc_tsElemRef* pg_mainEl[E_MAIN_EL_MAX];

gslc_tsXSlider m_sXSlider_Volume;
uint16_t m_nPosVolume;
char* m_cPosVolume;

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

void pg_main_loadFolder(gslc_tsGui *pGui, char* folderPath);

void pg_main_init(gslc_tsGui *pGui);
void pg_main_open(gslc_tsGui *pGui);
void pg_main_destroy(gslc_tsGui *pGui);
void pg_main(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_MAIN_H_
