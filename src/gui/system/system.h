#ifndef _WIFI_H_
#define _WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdio.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "libs/shared.h"
#include "gui/pages.h"


//////////////////
// GUI Page
enum {
  E_SYSTEM_EL_BOX,
  E_SYSTEM_EL_CLOSE,
  E_SYSTEM_EL_REBOOT,
  E_SYSTEM_EL_EXIT,
  E_SYSTEM_EL_WIFI,
  E_SYSTEM_EL_POWERCYCLEHDMI,
  E_SYSTEM_EL_UPGRADE,
  E_SYSTEM_EL_STARTUP,
  E_SYSTEM_EL_RECONFIGBUTTONS,
  E_SYSTEM_EL_REGISTERDEVICE,

  E_SYSTEM_EL_MAX
};

#define MAX_ELEM_PG_SYSTEM      E_SYSTEM_EL_MAX + 1
#define MAX_ELEM_PG_SYSTEM_RAM  MAX_ELEM_PG_SYSTEM

gslc_tsElem pg_systemElem[MAX_ELEM_PG_SYSTEM];
gslc_tsElemRef pg_systemElemRef[MAX_ELEM_PG_SYSTEM_RAM];

gslc_tsElemRef* pg_systemEl[E_SYSTEM_EL_MAX];

int pg_system_apt_upgrade;

bool pg_system_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_system_cbBtn_reboot(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_system_cbBtn_exit(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_system_cbBtn_wifi(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_system_cbBtn_powercycleHdmi(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_system_cbBtn_upgrade(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_system_cbBtn_startup(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);

bool pg_system_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);

int pg_system_guiInit(gslc_tsGui *pGui);
void pg_system_init(gslc_tsGui *pGui);
void pg_system_open(gslc_tsGui *pGui);
void pg_system_destroy();
void __attribute__ ((constructor)) pg_system_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WIFI_H_