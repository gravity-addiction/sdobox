#ifndef _WIFI_LIST_SAVED_H_
#define _WIFI_LIST_SAVED_H_

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
  E_WIFI_LIST_SAVED_EL_BOX,
  E_WIFI_LIST_SAVED_EL_CLOSE,
  E_WIFI_LIST_SAVED_EL_CONNECT,
  E_WIFI_LIST_SAVED_EL_SCAN,

  E_WIFI_LIST_SAVED_EL_SLIDER,
  E_WIFI_LIST_SAVED_EL_SLIDER_DOWN,
  E_WIFI_LIST_SAVED_EL_SLIDER_UP,

  E_WIFI_LIST_SAVED_EL_A,
  E_WIFI_LIST_SAVED_EL_B,
  E_WIFI_LIST_SAVED_EL_C,
  E_WIFI_LIST_SAVED_EL_D,
  E_WIFI_LIST_SAVED_EL_E,
  E_WIFI_LIST_SAVED_EL_F,

  E_WIFI_LIST_SAVED_EL_SSID,
  E_WIFI_LIST_SAVED_EL_DBM,

  E_WIFI_LIST_SAVED_EL_MAX
};

#define MAX_ELEM_PG_WIFI_LIST_SAVED      E_WIFI_LIST_SAVED_EL_MAX
#define MAX_ELEM_PG_WIFI_LIST_SAVED_RAM  MAX_ELEM_PG_WIFI_LIST_SAVED

gslc_tsElem pg_wifiListSavedElem[MAX_ELEM_PG_WIFI_LIST_SAVED];
gslc_tsElemRef pg_wifiListSavedElemRef[MAX_ELEM_PG_WIFI_LIST_SAVED_RAM];

gslc_tsElemRef* pg_wifiListSavedEl[E_WIFI_LIST_SAVED_EL_MAX];

gslc_tsXSlider pg_wifi_list_saved_slider;

struct pg_wifi_list_saved_data {
  int max;
  int len;
  int cur;
  int scrollMax;
  int scroll;
  struct pg_wifi_networkStruct **ptrs;
};
struct pg_wifi_list_saved_data *pg_wifi_list_saved_networkList;
struct pg_wifi_list_saved_data * PG_WIFI_LIST_SAVED_INIT_DATA();
void PG_WIFI_LIST_SAVED_CLEAR_DATA(struct pg_wifi_list_saved_data *data);

bool pg_wifi_list_saved_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);

int pg_wifi_list_saved_guiInit(gslc_tsGui *pGui);
void pg_wifi_list_saved_init(gslc_tsGui *pGui);
void pg_wifi_list_saved_open(gslc_tsGui *pGui);
void pg_wifi_list_saved_destroy();
void __attribute__ ((constructor)) pg_wifi_list_saved_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WIFI_LIST_SAVED_H_