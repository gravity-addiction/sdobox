#include <stdio.h>
#include <string.h>

#include "shared.h"
#include "lib_wifi_wpa.h"
#include "lib_wifi_networks.h"




struct pg_wifi_networkAddStruct * PG_WIFI_INIT_INPUT()
{
  struct pg_wifi_networkAddStruct *inp = (struct pg_wifi_networkAddStruct*)malloc(sizeof(struct pg_wifi_networkAddStruct));

  inp->ssidMax = 256;
  inp->ssidLen = 0;
  inp->ssidPtr = (char*)calloc(inp->ssidMax, sizeof(char));

  inp->passMax = 256;
  inp->passLen = 0;
  inp->passPtr = (char*)calloc(inp->passMax, sizeof(char));

  return inp;
};

void PG_WIFI_DESTROY_INPUT(struct pg_wifi_networkAddStruct *inp) {

  free(inp->ssidPtr);
  free(inp->passPtr);

  free(inp);
}



struct pg_wifi_statusStruct * PG_WIFI_INIT_STATUS()
{
  struct pg_wifi_statusStruct *ws = (struct pg_wifi_statusStruct*)malloc(sizeof(struct pg_wifi_statusStruct));

  ws->network_id = -1;
  
  ws->ip_address = (char *)calloc(4, sizeof(char));
  strlcpy(ws->ip_address, "IP:", 4);

  ws->bssid = (char *)calloc(5, sizeof(char));
  strlcpy(ws->bssid, "MAC:", 5);

  ws->freq = (char *)calloc(9, sizeof(char));
  strlcpy(ws->freq, "CHANNEL:", 9);

  ws->ssid = (char *)calloc(9, sizeof(char));
  strlcpy(ws->ssid, "NETWORK:", 9);

  ws->mode = (char *)calloc(6, sizeof(char));
  strlcpy(ws->mode, "MODE:", 6);

  ws->wpa_state = (char *)calloc(12, sizeof(char));
  strlcpy(ws->wpa_state, "CONNECTION:", 12);

  ws->pairwise_cipher = (char *)calloc(1, sizeof(char));
  ws->group_cipher = (char *)calloc(1, sizeof(char));
  ws->key_mgmt = (char *)calloc(1, sizeof(char));
  ws->p2p_device_address = (char *)calloc(1, sizeof(char));
  ws->address = (char *)calloc(1, sizeof(char));
  ws->uuid = (char *)calloc(1, sizeof(char));

  return ws;
};

void PG_WIFI_DESTROY_STATUS(struct pg_wifi_statusStruct *ws) {

  free(ws->ip_address);
  free(ws->bssid);
  free(ws->freq);
  free(ws->ssid);
  free(ws->mode);
  free(ws->wpa_state);
  free(ws->pairwise_cipher);
  free(ws->group_cipher);
  free(ws->key_mgmt);
  free(ws->p2p_device_address);
  free(ws->address);
  free(ws->uuid);

  free(ws);
}



struct pg_wifi_networkStruct * PG_WIFI_INIT_NETWORK()
{
  struct pg_wifi_networkStruct *net = (struct pg_wifi_networkStruct*)malloc(sizeof(struct pg_wifi_networkStruct));

  net->network_id = -1;
  net->bssid = (char *)calloc(1, sizeof(char));
  net->freq = (char *)calloc(1, sizeof(char));
  net->dBm = (char *)calloc(1, sizeof(char));
  net->flags = (char *)calloc(1, sizeof(char));
  net->ssid = (char *)calloc(1, sizeof(char));

  return net;
}

void PG_WIFI_DESTROY_NETWORK(struct pg_wifi_networkStruct *wn)
{
  free(wn->bssid);
  free(wn->freq);
  free(wn->dBm);
  free(wn->flags);
  free(wn->ssid);
  free(wn); 
}




struct pg_wifi_networksStruct * PG_WIFI_INIT_NETWORKS() {
  struct pg_wifi_networksStruct *nets = (struct pg_wifi_networksStruct*)malloc(sizeof(struct pg_wifi_networksStruct));
  nets->max = 32;
  nets->len = 0;
  nets->networks = (struct pg_wifi_networkStruct**)malloc(nets->max * sizeof(struct pg_wifi_networkStruct*));

  return nets;
}

void PG_WIFI_DESTROY_NETWORKS(struct pg_wifi_networksStruct *wns)
{
  for (int i = 0; i < wns->len; ++i) {
    PG_WIFI_DESTROY_NETWORK(wns->networks[i]);
  }
  free(wns->networks);
  free(wns);
}








int pg_wifi_appendNetwork(struct pg_wifi_networkStruct *wifi, struct pg_wifi_networksStruct **wns) {
  // Exists? replace
  for (int w = 0; w < (*wns)->len; ++w) {
    if (wifi->ssid == NULL) { break; }
    if (strcmp((*wns)->networks[w]->ssid, wifi->ssid) == 0) {
      (*wns)->networks[w] = wifi;
      return w;
    }
  }

  // Add additionally
  if ((*wns)->len < (*wns)->max ) {
    (*wns)->networks[(*wns)->len] = wifi;
    (*wns)->len += 1;
    return (*wns)->len - 1;
  }
  return -1;
}

void pg_wifi_clearNetworks(struct pg_wifi_networksStruct **wns) {
  int wnsLen = (*wns)->len;
  for (int i = 0; i < wnsLen; ++i) {
    PG_WIFI_DESTROY_NETWORK((*wns)->networks[i]);
  }
  (*wns)->len = 0;
}




void pg_wifi_addNetSaved(char *buf, size_t sz, size_t cnt) {
  if (cnt == 0) { // has header line (skip for now)
    // printf("Got Header Line: --%s--\n", buf);
    return;
  }

  char *t[4];
  t[0] = malloc(sizeof(int));
  t[1] = malloc(sz + 1);
  t[2] = malloc(sz + 1);
  t[3] = malloc(sz + 1);

  parseTabbedData(buf, t, 4);
  // printf("--%s--\nN: %s\nSSID: %s\nBSSID: %s\nFLAGS: %s\n\n", buf, t[0], t[1], t[2], t[3]);

  struct pg_wifi_networkStruct *wifi = PG_WIFI_INIT_NETWORK();
  wifi->network_id = atoi(t[0]);

  // SSID
  size_t ssidSz = snprintf(NULL, 0, "%s", t[1]) + 1;
  char *newSsid = (char *)realloc(wifi->ssid, ssidSz * sizeof(char));
  snprintf(newSsid, ssidSz, "%s", t[1]);
  wifi->ssid = newSsid;

  // BSSID
  size_t bssidSz = snprintf(NULL, 0, "%s", t[2]) + 1;
  char *newBssid = (char *)realloc(wifi->bssid, bssidSz * sizeof(char));
  snprintf(newBssid, bssidSz, "%s", t[2]);
  wifi->bssid = newBssid;

  // Flags
  size_t flagsSz = snprintf(NULL, 0, "%s", t[3]) + 1;
  char *newFlags = (char *)realloc(wifi->flags, flagsSz * sizeof(char));
  snprintf(newFlags, flagsSz, "%s", t[3]);
  wifi->flags = newFlags;

  pg_wifi_appendNetwork(wifi, &pg_wifi_nets_saved);
  free(t[0]);
  free(t[1]);
  free(t[2]);
  free(t[3]);
}


void pg_wifi_addNetAvailable(char *buf, size_t sz, size_t cnt) {
  printf("%s\n", buf);
  if (cnt == 0) { // has header line (skip for now)
    // bssid / frequency / signal level / flags / ssid
    // printf("Got Header Line: --%s--\n", buf);
    return;
  }

  char *t[5];
  t[0] = malloc(sz + 1);
  t[1] = malloc(sz + 1);
  t[2] = malloc(sz + 1);
  t[3] = malloc(sz + 1);
  t[4] = malloc(sz + 1);

  parseTabbedData(buf, t, 5);
  printf("--%s--\nN: %s\nSSID: %s\nBSSID: %s\nFLAGS: %s\n\n", buf, t[0], t[1], t[2], t[3]);

  struct pg_wifi_networkStruct *wifi = PG_WIFI_INIT_NETWORK();

  // BSSID
  size_t bssidSz = snprintf(NULL, 0, "%s", t[0]) + 1;
  char *newBssid = (char *)realloc(wifi->bssid, bssidSz * sizeof(char));
  snprintf(newBssid, bssidSz, "%s", t[0]);
  wifi->bssid = newBssid;

  // Freq
  size_t freqSz = snprintf(NULL, 0, "%s", t[1]) + 1;
  char *newFreq = (char *)realloc(wifi->freq, freqSz * sizeof(char));
  snprintf(newFreq, freqSz, "%s", t[1]);
  wifi->freq = newFreq;

  // dBm
  size_t dBmSz = snprintf(NULL, 0, "%s", t[2]) + 1;
  char *newDBm = (char *)realloc(wifi->dBm, dBmSz * sizeof(char));
  snprintf(newDBm, dBmSz, "%s", t[2]);
  wifi->dBm = newDBm;
  
  // Flags
  size_t flagsSz = snprintf(NULL, 0, "%s", t[3]) + 1;
  char *newFlags = (char *)realloc(wifi->flags, flagsSz * sizeof(char));
  snprintf(newFlags, flagsSz, "%s", t[3]);
  wifi->flags = newFlags;

  // SSID
  size_t ssidSz = snprintf(NULL, 0, "%s", t[4]) + 1;
  char *newSsid = (char *)realloc(wifi->ssid, ssidSz * sizeof(char));
  snprintf(newSsid, ssidSz, "%s", t[4]);
  wifi->ssid = newSsid;

  pg_wifi_appendNetwork(wifi, &pg_wifi_nets_available);

  free(t[0]);
  free(t[1]);
  free(t[2]);
  free(t[3]);
  free(t[4]);
}





void pg_wifi_updateStatus(char *buf, size_t sz, size_t cnt) {
  // printf("Check Len: %d\nStatus: %s\n", sz, buf);
  char *key = (char *)calloc(128, sizeof(char));
  char *val = (char *)calloc(sz, sizeof(char));

  sscanf(buf, "%127[^=]=%s", key, val);
  // printf("Status Key: %s\nVal: %s\n", key, val);

  // IP Address
  if (strcmp(key, "ip_address") == 0) {
    char* ipStr = "IP:         %s";
    size_t ipSz = snprintf(NULL, 0, ipStr, val) + 1;
    char *newIp = realloc(pg_wifi_status->ip_address, ipSz);
    snprintf(newIp, ipSz, ipStr, val);
    pg_wifi_status->ip_address = newIp;


  // MAC
  } else if (strcmp(key, "bssid") == 0) {
    char* macStr = "MAC:        %s";
    size_t macSz = snprintf(NULL, 0, macStr, val) + 1;
    char *newMac = realloc(pg_wifi_status->bssid, macSz);
    snprintf(newMac, macSz, macStr, val);
    pg_wifi_status->bssid = newMac;

  // Freqency
  } else if (strcmp(key, "freq") == 0) {
    char* freqStr = "CHANNEL:    %s";
    size_t freqSz = snprintf(NULL, 0, freqStr, val) + 1;
    char *newFreq = realloc(pg_wifi_status->freq, freqSz);
    snprintf(newFreq, freqSz, freqStr, val);
    pg_wifi_status->freq = newFreq;

  // SSID (Network)
  } else if (strcmp(key, "ssid") == 0) {
    char* networkStr = "NETWORK:    %s";
    size_t networkSz = snprintf(NULL, 0, networkStr, val) + 1;
    char *newNetwork = realloc(pg_wifi_status->ssid, networkSz);
    snprintf(newNetwork, networkSz, networkStr, val);
    pg_wifi_status->ssid = newNetwork;

  // Mode
  } else if (strcmp(key, "mode") == 0) {
    char* modeStr = "MODE:       %s";
    size_t modeSz = snprintf(NULL, 0, modeStr, val) + 1;
    char *newMode = realloc(pg_wifi_status->mode, modeSz);
    snprintf(newMode, modeSz, modeStr, val);
    pg_wifi_status->mode = newMode;

  // Connection
  } else if (strcmp(key, "wpa_state") == 0) {
    char* connectionStr = "CONNECTION: %s";
    size_t connectionSz = snprintf(NULL, 0, connectionStr, val) + 1;
    char *newConnection = realloc(pg_wifi_status->wpa_state, connectionSz);
    snprintf(newConnection, connectionSz, connectionStr, val);
    pg_wifi_status->wpa_state = newConnection;


  }/* else if (strcmp(key, "id") == 0) {
    // sscanf(buf, "%128[^=]=%d", key, &vaI);
    //pg_wifi_status->network_id = vaI;
  } else if (strcmp(key, "pairwise_cipher") == 0) {
    // pg_wifi_status->pairwise_cipher = val;
  } else if (strcmp(key, "group_cipher") == 0) {
    // pg_wifi_status->group_cipher = val;
  } else if (strcmp(key, "key_mgmt") == 0) {
    // pg_wifi_status->key_mgmt = val;
  } else if (strcmp(key, "p2p_device_address") == 0) {
    // pg_wifi_status->p2p_device_address = val;
  } else if (strcmp(key, "address") == 0) {
    // pg_wifi_status->address = val;
  } else if (strcmp(key, "uuid") == 0) {
    // pg_wifi_status->uuid = val;
  }
  */

  free(key);
  free(val);
}








void pg_wifi_updateSavedNetworks() {

  size_t len = 4096;
  char *buf = (char *)calloc(len, sizeof(char));
  // pg_wifi_wpaShownList = E_WIFI_WPA_LIST_NETWORKS;

  pg_wifi_wpaSendCmdBuf("LIST_NETWORKS", buf, &len);
  if (!len) {
    free(buf);
    return;
  }
  // id / ssid / bssid / flags
  // pg_wifi_clearNetworks(&pg_wifi_nets_saved);
  sgetlines_withcb(buf, len, &pg_wifi_addNetSaved);
  free(buf);
}

void pg_wifi_updateAvailableNetworks() {

  size_t len = 4096;
  char *buf = (char *)calloc(len, sizeof(char));
  // pg_wifi_wpaShownList = E_WIFI_WPA_SCAN_NETWORKS;

  pg_wifi_wpaSendCmdBuf("SCAN_RESULTS", buf, &len);
  if (!len) {
    free(buf);
    return;
  }
  // id / ssid / bssid / flags
  // pg_wifi_clearNetworks(&pg_wifi_nets_available);
  printf("%s\n", buf);
  sgetlines_withcb(buf, len, &pg_wifi_addNetAvailable);
  free(buf);

  printf("Got Available Networks: %d\n", pg_wifi_nets_available->len);
}



int pg_wifi_getStatus() {
  size_t len = 4096;
  char *buf = (char *)calloc(len, sizeof(char));

  pg_wifi_wpaSendCmdBuf("STATUS", buf, &len);
  if (!len) {
    free(buf);
    return 0;
  }
  sgetlines_withcb(buf, len, &pg_wifi_updateStatus);
  free(buf);
  return 1;
}


void pg_wifi_disableNetworkAll() {
  pg_wifi_wpaSendCmd("DISABLE_NETWORK all");
}

void pg_wifi_enableNetworkAll() {
  pg_wifi_wpaSendCmd("ENABLE_NETWORK all");
}

void pg_wifi_disableNetwork(int network_id) {
  size_t cmdMem = snprintf(NULL, 0, "DISABLE_NETWORK %d", network_id);
  char *cmd = malloc(cmdMem + 1);
  snprintf(cmd, cmdMem + 1, "DISABLE_NETWORK %d", network_id);
  // debug_print("\n\n%s\n", cmd);
  // pg_wifi_wpaSendCmd(cmd);
}
void pg_wifi_enableNetwork(int network_id) {
  size_t cmdMem = snprintf(NULL, 0, "ENABLE_NETWORK %d", network_id);
  char *cmd = malloc(cmdMem + 1);
  snprintf(cmd, cmdMem + 1, "ENABLE_NETWORK %d", network_id);
  // debug_print("\n\n%s\n", cmd);
  // pg_wifi_wpaSendCmd(cmd);
}

// pg_wifi_wpaSetEventCallbackFunc ?
void wifiWpaEvent(char* event) {
  // debug_print("Got Event: --%s--\n", event);
  if (strcmp(event, "CTRL-EVENT-SCAN-STARTED ") == 0) {
    // debug_print("%s\n", "Scan Started!");
  }
  /*
  char *newTxt = malloc(strlen(str) + 1);
  strlcpy(newTxt, str, strlen(str) + 1);
  scorecard->judge = newTxt;
  gslc_ElemSetTxtStr(pGui, m_pElemScorecardJudge, str);
  */
}

