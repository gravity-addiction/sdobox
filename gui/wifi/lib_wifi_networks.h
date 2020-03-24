#ifndef _WIFI_NETWORKS_H_
#define _WIFI_NETWORKS_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


struct pg_wifi_networkAddStruct {
 int id;
 size_t ssidMax;
 size_t ssidLen;
 char *ssidPtr;
 size_t passMax;
 size_t passLen;
 char *passPtr;
};

struct pg_wifi_statusStruct {
  int id;
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
  int id;
  char* bssid;
  char* freq;
  char* dBm;
  char* flags;
  char* ssid;
  char* txt;
};

struct pg_wifi_networksStruct {
  int max;
  int len;
  struct pg_wifi_networkStruct **ptrs;
};
struct pg_wifi_networksStruct *pg_wifi_nets_available;
struct pg_wifi_networksStruct *pg_wifi_nets_saved;
struct pg_wifi_networksStruct *pg_wifi_nets_combined;
struct pg_wifi_networkStruct *pg_wifi_net_selected;

struct pg_wifi_networkAddStruct * PG_WIFI_INIT_INPUT();
void PG_WIFI_DESTROY_INPUT(struct pg_wifi_networkAddStruct *inp);

struct pg_wifi_statusStruct * PG_WIFI_INIT_STATUS();
void PG_WIFI_DESTROY_STATUS(struct pg_wifi_statusStruct *ws);

struct pg_wifi_networkStruct * PG_WIFI_INIT_NETWORK();
void PG_WIFI_DESTROY_NETWORK(struct pg_wifi_networkStruct *wn);

struct pg_wifi_networksStruct * PG_WIFI_INIT_NETWORKS();
void PG_WIFI_DESTROY_NETWORKS(struct pg_wifi_networksStruct *wns);

void pg_wifi_clearNetworks(struct pg_wifi_networksStruct **wns);
int pg_wifi_appendNetwork(struct pg_wifi_networkStruct *wifi, struct pg_wifi_networksStruct **wns);
void pg_wifi_destroyNetwork(struct pg_wifi_networksStruct *wns, int i);

void pg_wifi_addNetSaved(char *buf, size_t sz, size_t cnt);
void pg_wifi_addNetAvailable(char *buf, size_t sz, size_t cnt);
void pg_wifi_updateStatus(char *buf, size_t sz, size_t cnt);
void pg_wifi_setInterface(char* interface);
void pg_wifi_combineNetworks();
void pg_wifi_updateSavedNetworks();
void pg_wifi_updateAvailableNetworks();
int pg_wifi_getStatus();


void wifiWpaEvent(char* event);

void pg_wifi_disableNetworkAll();
void pg_wifi_enableNetworkAll();
void pg_wifi_disableNetwork(int id);
void pg_wifi_enableNetwork(int id);
void pg_wifi_selectNetwork(int id);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WIFI_NETWORKS_H_