#ifndef _PAGES_STARTUP_H_
#define _PAGES_STARTUP_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_STARTUP_EL_IPBOX,
  E_STARTUP_EL_IP_ADDRESS,
  E_STARTUP_EL_IPBOX_UP,
  E_STARTUP_EL_IPBOX_DOWN,
  E_STARTUP_EL_BTN_CONFIG,
  E_STARTUP_EL_BTN_WIFI,

  E_STARTUP_EL_MAX
};

#define MAX_ELEM_PG_STARTUP      E_STARTUP_EL_MAX
#define MAX_ELEM_PG_STARTUP_RAM  MAX_ELEM_PG_STARTUP

gslc_tsElem m_asPgStartupElem[MAX_ELEM_PG_STARTUP_RAM];
gslc_tsElemRef m_asPgStartupElemRef[MAX_ELEM_PG_STARTUP];

gslc_tsElemRef* pg_startupEl[E_STARTUP_EL_MAX];

#define PG_STARTUP_IPBOX_ROWS 10
#define PG_STARTUP_IPBOX_COLS 43
gslc_tsXTextbox pg_startupIpboxTextbox;

void pg_startupButtonRotaryCW();
void pg_startupButtonRotaryCCW();
void pg_startupButtonLeftPressed();
void pg_startupButtonRightPressed();
void pg_startupButtonRotaryPressed();
void pg_startupButtonLeftHeld();
void pg_startupButtonRightHeld();
void pg_startupButtonRotaryHeld();
void pg_startupButtonDoubleHeld();
void pg_startupButtonSetFuncs();

void pg_startup_init(gslc_tsGui *pGui);
void pg_startup_open(gslc_tsGui *pGui);
void pg_startup_destroy(gslc_tsGui *pGui);
void pg_startup_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_STARTUP_H_
