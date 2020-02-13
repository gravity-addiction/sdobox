#ifndef _WIFI_LIST_H_
#define _WIFI_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdio.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "shared.h"
#include "gui/pages.h"
#include "vlisting/vlisting.h"

#include "lib_wifi_wpa.h"
#include "lib_wifi_networks.h"

//////////////////
// GUI Page
enum {
  E_WIFI_LIST_EL_BOX,
  E_WIFI_LIST_EL_CLOSE,
  E_WIFI_LIST_EL_CONNECT,
  E_WIFI_LIST_EL_SCAN,

  E_WIFI_LIST_EL_SSID,
  E_WIFI_LIST_EL_DBM,

  E_WIFI_LIST_EL_MAX
};

gslc_tsElem *pg_wifiListElem;
gslc_tsElemRef pg_wifiListElemRef[E_WIFI_LIST_EL_MAX + 19];

gslc_tsElemRef **pg_wifiListEl;
int pg_wifiListElTotal;

struct vlist_config *pg_wifi_list_networkConfig;
struct pg_wifi_networkStruct **pg_wifi_list_networkList;
gslc_tsXSlider pg_wifi_list_networkSlider;

void pg_wifi_list_kbPass(gslc_tsGui *pGui, char* str);
int pg_wifi_list_addNetworkList(struct pg_wifi_networkStruct *ptr);
void pg_wifi_list_setNetworkList(struct pg_wifi_networkStruct **ptrs, int len);
void pg_wifi_list_resetNetworkList();
void pg_wifi_list_wpaEvent(char* event);

bool pg_wifi_list_cbBtn_elA(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_list_cbBtn_elB(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_list_cbBtn_elC(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_list_cbBtn_elD(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_list_cbBtn_elE(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);

bool pg_wifi_list_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_list_cbBtn_scan(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_list_cbBtn_connect(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);

bool pg_wifi_list_cbBtn_sliderPos(void* pvGui, void* pvElemRef, int16_t nPos);
bool pg_wifi_list_cbBtn_sliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_list_cbBtn_sliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);

bool pg_wifi_list_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);
int pg_wifi_list_guiInit(gslc_tsGui *pGui);

void pg_wifi_list_init(gslc_tsGui *pGui);
void pg_wifi_list_open(gslc_tsGui *pGui);
void pg_wifi_list_close(gslc_tsGui *pGui);
void pg_wifi_list_destroy();
void __attribute__ ((constructor)) pg_wifi_list_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WIFI_H_