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

#include "wifi_wpa.h"


//////////////////
// GUI Page
#define MAX_ELEM_PG_WIFI      100
#define MAX_ELEM_PG_WIFI_RAM  MAX_ELEM_PG_WIFI

enum {
  E_WIFI_EL_CLOSE,
  E_WIFI_EL_BOX,

  E_WIFI_EL_LIST_NETWORKS,
  E_WIFI_EL_SCAN_NETWORKS,
  E_WIFI_EL_REASSOCIATE,
  E_WIFI_EL_DISABLE_NETWORK,
  E_WIFI_EL_ENABLE_NETWORK,

  E_WIFI_EL_STATUS_SSID,
  E_WIFI_EL_STATUS_STATE,
  E_WIFI_EL_STATUS_IP,
  E_WIFI_EL_STATUS_FREQ,
  E_WIFI_EL_STATUS_MODE,

  E_WIFI_EL_MAX
};

gslc_tsElem pg_wifiElem[MAX_ELEM_PG_WIFI];
gslc_tsElemRef pg_wifiElemRef[MAX_ELEM_PG_WIFI_RAM];

gslc_tsElemRef* pg_wifiEl[E_WIFI_EL_MAX];



struct pg_wifi_statusStruct {
  int network_id;
  char* bssid;
  char* freq;
  char* ssid;
  char* mode;
  char* pairwise_cipher;
  char* group_cipher;
  char* key_mgmt;
  char* wpa_state;
  char* ip_address;
  char* p2p_device_address;
  char* address;
  char* uuid;
};
struct pg_wifi_statusStruct *pg_wifi_status;


struct pg_wifi_networkStruct {
  int network_id;
  char* bssid;
  char* freq;
  char* dBm;
  char* flags;
  char* ssid;
};

struct pg_wifi_networksStruct {
  int max;
  int len;
  struct pg_wifi_networkStruct **networks;
};
struct pg_wifi_networksStruct *pg_wifi_nets_available;
struct pg_wifi_networksStruct *pg_wifi_nets_saved;








int m_wifi_show_status;
int m_wifi_show_list;
int m_wifi_list_selected;
int m_wifi_list_page;
int m_wifi_list_start;

int m_wifi_wpa_func_id;

int draw_wifi_flag;
void (*cbWifi[1])(gslc_tsGui *, char*) ;





struct pg_wifi_statusStruct * PG_WIFI_INIT_STATUS();
void PG_WIFI_DESTROY_STATUS(struct pg_wifi_statusStruct *ws);

struct pg_wifi_networkStruct * PG_WIFI_INIT_NETWORK();
void PG_WIFI_DESTROY_NETWORK(struct pg_wifi_networkStruct *wn);

struct pg_wifi_networksStruct * PG_WIFI_INIT_NETWORKS();
void PG_WIFI_DESTROY_NETWORKS(struct pg_wifi_networksStruct *wns);

int pg_wifi_appendNetwork(struct pg_wifi_networkStruct *wifi, struct pg_wifi_networksStruct **wns);
void pg_wifi_destroyNetwork(struct pg_wifi_networksStruct *wns, int i);

void pg_wifi_addNetSaved(char *buf, size_t sz, size_t cnt);
void pg_wifi_addNetAvailable(char *buf, size_t sz, size_t cnt);
void pg_wifi_updateStatus(char *buf, size_t sz, size_t cnt);
void pg_wifi_updateSavedNetworks();
void pg_wifi_updateAvailableNetworks();
int pg_wifi_getStatus();

void pg_wifi_showStatus(gslc_tsGui *pGui);
void pg_wifi_disableNetworkAll();
void pg_wifi_enableNetworkAll();
void pg_wifi_disableNetwork(int network_id);
void pg_wifi_enableNetwork(int network_id);


bool pg_wifi_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_savedNetworks(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_availableNetworks(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_reassociate(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_disableNetwork(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_wifi_cbBtn_enableNetwork(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);





void pg_wifi_listShow(gslc_tsGui *pGui, int arrStart);
void pg_wifi_listSetSelected(gslc_tsGui *pGui, int selected);
void wifiWpaEvent(char* event);


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