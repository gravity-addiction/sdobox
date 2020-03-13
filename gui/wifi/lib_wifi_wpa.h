#ifndef _WIFI_WPA_H_
#define _WIFI_WPA_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "wpa/wpa_ctrl.h"
#include "wpa/common.h"
#include "wiringPi.h"

enum {
  E_WIFI_WPA_LIST_NETWORKS,
  E_WIFI_WPA_SCAN_NETWORKS
};

struct wpa_ctrl *pg_wifi_wpa_conn;
struct wpa_ctrl *pg_wifi_wpa_events;

int pg_wifi_wpaScanning;
int pg_wifi_wpaShownList;

int pg_wifi_wpaEventThreadRunning;
int pg_wifi_wpaEventThreadStop;

void (*cbEvent[1])(char*) ;

void *pg_wifi_wpaEventThread (UNU void *dummy);

void pg_wifi_wpaEventCB(char *buf, size_t len);

int pg_wifi_wpaEvents(char* wpa_interface);
int pg_wifi_wpaOpen(char* wpa_interface);
int pg_wifi_wpaTestConnection();
int pg_wifi_wpaSendCmdBuf(char* cmd, char ** buf);
int pg_wifi_wpaSendCmd(char* cmd);
void pg_wifi_wpaClose();

int pg_wifi_wpaSetEventCallbackFunc(void (*function)(char *));
void pg_wifi_wpaRemoveEventCallbackFunc(int funcId);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WIFI_WPA_H_