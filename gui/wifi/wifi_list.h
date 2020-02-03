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

#include "lib_wifi_wpa.h"
#include "lib_wifi_networks.h"

//////////////////
// GUI Page
enum {
  E_WIFI_LIST_EL_BOX,
  E_WIFI_LIST_EL_CLOSE,
  E_WIFI_LIST_EL_CONNECT,

  E_WIFI_LIST_EL_SLIDER,
  E_WIFI_LIST_EL_SLIDER_DOWN,
  E_WIFI_LIST_EL_SLIDER_UP,

  E_WIFI_LIST_EL_A,
  E_WIFI_LIST_EL_B,
  E_WIFI_LIST_EL_C,
  E_WIFI_LIST_EL_D,
  E_WIFI_LIST_EL_E,
  E_WIFI_LIST_EL_F,

  E_WIFI_LIST_EL_SSID,
  E_WIFI_LIST_EL_DBM,

  E_WIFI_LIST_EL_MAX
};

#define MAX_ELEM_PG_WIFI_LIST      E_WIFI_LIST_EL_MAX
#define MAX_ELEM_PG_WIFI_LIST_RAM  MAX_ELEM_PG_WIFI_LIST

gslc_tsElem pg_wifiListElem[MAX_ELEM_PG_WIFI_LIST];
gslc_tsElemRef pg_wifiListElemRef[MAX_ELEM_PG_WIFI_LIST_RAM];

gslc_tsElemRef* pg_wifiListEl[E_WIFI_LIST_EL_MAX];

gslc_tsXSlider pg_wifi_list_slider;

struct pg_wifi_list_data {
  int max;
  int len;
  int scrollMax;
  int scroll;
  pg_wifi_networkStruct **ptrs;
};
struct pg_wifi_list_data *pg_wifi_list_networkList;
struct pg_wifi_list_data * PG_WIFI_LIST_INIT_DATA();
void PG_WIFI_LIST_CLEAR_DATA(struct pg_wifi_list_data *data);

int pg_wifi_list_scroll_max; // max positions of vertical scrollbar
int pg_wifi_list_scroll; // position of vertical scrollbar

bool pg_wifi_list_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);

int pg_wifi_list_guiInit(gslc_tsGui *pGui);
void pg_wifi_list_init(gslc_tsGui *pGui);
void pg_wifi_list_open(gslc_tsGui *pGui);
void pg_wifi_list_destroy();
void __attribute__ ((constructor)) pg_wifi_list_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WIFI_H_