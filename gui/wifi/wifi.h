#ifndef _WIFI_H_
#define _WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdio.h>
#include <string.h> 
#include <wiringPi.h> // Gordons Wiring Pi

#include "shared.h"
#include "gui/pages.h"

#include "lib_wifi_wpa.h"
#include "lib_wifi_networks.h"


//////////////////
// GUI Page
enum {
  E_WIFI_EL_BOX,
  E_WIFI_EL_BOX_SSID_TXT,
  E_WIFI_EL_BOX_SSID_BTN,
  E_WIFI_EL_BOX_PASS_TXT,
  E_WIFI_EL_BOX_PASS_BTN,

  E_WIFI_EL_BTN_FINDNET,
  E_WIFI_EL_BTN_SAVEDNET,
  E_WIFI_EL_BTN_CONNECT,

  E_WIFI_EL_CLOSE,
  E_WIFI_EL_LIST_NETWORKS,
  E_WIFI_EL_SCAN_NETWORKS,
  E_WIFI_EL_REFRESH_NETWORKS,
  E_WIFI_EL_REASSOCIATE,
  E_WIFI_EL_REFRESH,

  E_WIFI_EL_DISABLE_NETWORK,
  E_WIFI_EL_ENABLE_NETWORK,

  E_WIFI_EL_STATUS_SSID,
  E_WIFI_EL_STATUS_STATE,
  E_WIFI_EL_STATUS_IP,
  E_WIFI_EL_STATUS_FREQ,
  E_WIFI_EL_STATUS_MODE,
  E_WIFI_EL_MAX
};

#define MAX_ELEM_PG_WIFI      E_WIFI_EL_MAX
#define MAX_ELEM_PG_WIFI_RAM  MAX_ELEM_PG_WIFI

gslc_tsElem pg_wifiElem[MAX_ELEM_PG_WIFI];
gslc_tsElemRef pg_wifiElemRef[MAX_ELEM_PG_WIFI_RAM];

gslc_tsElemRef* pg_wifiEl[E_WIFI_EL_MAX];


struct pg_wifi_networkAddStruct *pg_wifi_addInput;

char *pg_wifi_ssidDisp;
char *pg_wifi_passDisp;


bool pg_wifi_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_savedNetworks(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_availableNetworks(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_reassociate(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_disableNetwork(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_enableNetwork(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);


void pg_wifi_showStatus(gslc_tsGui *pGui);
void pg_wifi_showList(gslc_tsGui *pGui, int arrStart);




bool pg_wifi_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);

int pg_wifi_guiInit(gslc_tsGui *pGui);
void pg_wifi_init(gslc_tsGui *pGui);
void pg_wifi_open(gslc_tsGui *pGui);
void pg_wifi_destroy();
void __attribute__ ((constructor)) pg_wifi_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WIFI_H_