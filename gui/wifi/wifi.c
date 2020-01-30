#include <stdio.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "shared.h"
#include "wifi_wpa.h"
#include "wifi.h"

#include "gui/pages.h"
#include "gui/main/main.h"




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
  nets->networks = (struct pg_wifi_networkStruct**)malloc(nets->max * sizeof(struct pg_wifi_networkStruct));

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








int pg_wifi_appendNetwork(struct pg_wifi_networkStruct *wifi, struct pg_wifi_networksStruct *wns) {
  if (wns->len < wns->max ) {
    wns->networks[wns->len] = wifi;
    ++wns->len;
    return wns->len - 1;
  }
  return -1;
}




void pg_wifi_addNetSaved(char *buf, size_t sz, size_t cnt) {
  if (cnt == 0) { // has header line (skip for now)
    printf("Got Header Line: --%s--\n", buf);
    return;
  }

  char *t[4];
  t[0] = malloc(sizeof(int));
  t[1] = malloc(sz);
  t[2] = malloc(sz);
  t[3] = malloc(sz);

  parseTabbedData(buf, t, 4);
  printf("--%s--\nN: %s\nSSID: %s\nBSSID: %s\nFLAGS: %s\n\n", buf, t[0], t[1], t[2], t[3]);

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

  free(t[0]);
  free(t[1]);
  free(t[2]);
  free(t[3]);
  pg_wifi_appendNetwork(wifi, pg_wifi_nets_saved);
}


void pg_wifi_addNetAvailable(char *buf, size_t sz, size_t cnt) {
  if (cnt == 0) { // has header line (skip for now)
    printf("Got Header Line: --%s--\n", buf);
    return;
  }

  printf("Got Parsed Line\n%s\n", buf);
  return;
  char *t[4];
  t[0] = malloc(sizeof(int));
  t[1] = malloc(sz);
  t[2] = malloc(sz);
  t[3] = malloc(sz);

  parseTabbedData(buf, t, 4);
  printf("--%s--\nN: %s\nSSID: %s\nBSSID: %s\nFLAGS: %s\n\n", buf, t[0], t[1], t[2], t[3]);

  struct pg_wifi_networkStruct *wifi = PG_WIFI_INIT_NETWORK();
  wifi->network_id = atoi(t[0]);
  strlcpy(wifi->ssid, t[1], strlen(t[1]) + 1);
  strlcpy(wifi->bssid, t[2], strlen(t[2]) + 1);
  strlcpy(wifi->flags, t[3], strlen(t[3]) + 1);
  free(t[0]);
  free(t[1]);
  free(t[2]);
  free(t[3]);
  pg_wifi_appendNetwork(wifi, pg_wifi_nets_available);
}





void pg_wifi_updateStatus(char *buf, size_t sz, size_t cnt) {
  printf("Check Len: %d\nStatus: %s\n", sz, buf);
  char *key = (char *)calloc(128, sizeof(char));
  char *val = (char *)calloc(sz, sizeof(char));

  sscanf(buf, "%127[^=]=%s", key, val);
  printf("Status Key: %s\nVal: %s\n", key, val);

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
  pg_wifi_wpaShownList = E_WIFI_WPA_LIST_NETWORKS;

  pg_wifi_wpaSendCmdBuf("LIST_NETWORKS", buf, &len);
  if (!len) {
    free(buf);
    return;
  }
  // id / ssid / bssid / flags
  sgetlines_withcb(buf, len, &pg_wifi_addNetSaved);
  free(buf);
}

void pg_wifi_updateAvailableNetworks() {

  size_t len = 4096;
  char *buf = (char *)calloc(len, sizeof(char));
  pg_wifi_wpaShownList = E_WIFI_WPA_SCAN_NETWORKS;

  pg_wifi_wpaSendCmdBuf("SCAN_RESULTS", buf, &len);
  if (!len) {
    free(buf);
    return;
  }
  // id / ssid / bssid / flags
  sgetlines_withcb(buf, len, &pg_wifi_addNetAvailable);
  free(buf);
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






void pg_wifi_showStatus(gslc_tsGui *pGui) {
  printf("Setting: %s\n", pg_wifi_status->ip_address);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], pg_wifi_status->ip_address);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], pg_wifi_status->ssid);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], pg_wifi_status->wpa_state);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], pg_wifi_status->freq);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], pg_wifi_status->mode);
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









bool pg_wifi_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  
  touchscreenPageOpen(pGui, E_PG_MAIN);
  pg_wifi_wpaClose();
  return true;
}

bool pg_wifi_cbBtn_savedNetworks(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
  pg_wifi_updateSavedNetworks();

  // wifiCleanShows();
  // m_wifi_show_list = 1;
  // gslc_ElemSetRedraw(pGui, pg_wifiEl[E_WIFI_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

bool pg_wifi_cbBtn_availableNetworks(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_updateAvailableNetworks();
  /*
  wifiCleanShows();
  m_wifi_show_list = 1;
  gslc_ElemSetRedraw(pGui, pg_wifiEl[E_WIFI_EL_BOX], GSLC_REDRAW_FULL);
  */
  return true;
}

bool pg_wifi_cbBtn_reassociate(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_wpaSendCmd("REASSOCIATE");
  return true;
}

bool pg_wifi_cbBtn_disableNetwork(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_disableNetwork(pg_wifi_status->network_id);
  return true;
}

bool pg_wifi_cbBtn_enableNetwork(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_enableNetwork(pg_wifi_status->network_id);
  return true;
}




void pg_wifi_listShow(gslc_tsGui *pGui, int arrStart) {
  /*
  for (size_t i = 0; i < 5; i++) {
    gslc_tsElemRef* pElemRefSSID = gslc_PageFindElemById(pGui, E_PG_WIFI, (9800 + i));
    gslc_tsElemRef* pElemRefSignal = gslc_PageFindElemById(pGui, E_PG_WIFI, (9810 + i));
    int iS = (arrStart + i);
    if (iS < pg_wifi_networks->len) {

      size_t dSize = snprintf(NULL, 0, "%sdBm", pg_wifi_networks->networks[iS]->dBm) + 1;
      char *buf = malloc(dSize);
      sprintf(buf, "%sdBm", wifiNetworks[iS]->dBm);
      gslc_ElemSetTxtStr(pGui, pElemRefSSID, pg_wifi_networks->networks[iS]->ssid);
      gslc_ElemSetTxtStr(pGui, pElemRefSignal, buf);
    } else {
      gslc_ElemSetTxtStr(pGui, pElemRefSSID, " ");
      gslc_ElemSetTxtStr(pGui, pElemRefSignal, " ");
    }

    gslc_ElemSetVisible(pGui, pElemRefSSID, true);
    gslc_ElemSetVisible(pGui, pElemRefSignal, true);
    gslc_ElemDraw(pGui, E_PG_WIFI, (9800 + i));
    gslc_ElemDraw(pGui, E_PG_WIFI, (9810 + i));
  }
*/
}

void pg_wifi_listSetSelected(gslc_tsGui *pGui, int selected) {
  // debug_print("Select: %d\n", selected);
  for (size_t i = 0; i < 5; i++) {
    gslc_tsElemRef* pElemRefSSID = gslc_PageFindElemById(pGui, E_PG_WIFI, (9800 + i));
    gslc_tsElemRef* pElemRefSignal = gslc_PageFindElemById(pGui, E_PG_WIFI, (9810 + i));
    if (selected == i) {
      gslc_ElemSetFillEn(pGui, pElemRefSSID, true);
      gslc_ElemSetFillEn(pGui, pElemRefSignal, true);
    } else {
      gslc_ElemSetFillEn(pGui, pElemRefSSID, false);
      gslc_ElemSetFillEn(pGui, pElemRefSignal, false);
    }
  }
}





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





bool pg_wifi_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  if (draw_wifi_flag) { return false; }
  draw_wifi_flag = 1;

  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  gslc_DrawFillRect(pGui,pRect,pElem->colElemFill);

  // gslc_ElemDraw(pGui, E_PG_WIFI, E_ELEM_WIFI_CLOSE); // Close Menu
  // gslc_ElemDraw(pGui, E_PG_WIFI, E_ELEM_WIFI_LIST_NETWORKS); // List Networks
  // gslc_ElemDraw(pGui, E_PG_WIFI, E_ELEM_WIFI_SCAN_NETWORKS); // Scan Networks
  // gslc_ElemDraw(pGui, E_PG_WIFI, E_ELEM_WIFI_REASSOCIATE); // Reassociate
  // gslc_ElemDraw(pGui, E_PG_WIFI, E_ELEM_WIFI_DISABLE); // Disable
  // gslc_ElemDraw(pGui, E_PG_WIFI, E_ELEM_WIFI_ENABLE); // Enable

  // if (m_wifi_show_status) {
  //   pg_wifi_showStatus(pGui);
  // }
  /*
  if (m_wifi_show_list) {
    pg_wifi_listSetSelected(pGui, m_wifi_list_selected);
    pg_wifi_listShow(pGui, m_wifi_list_start);
  }
  */

  draw_wifi_flag = 0;
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);

  return true;
}









int pg_wifi_guiInit(gslc_tsGui *pGui)
{
  // debug_print("%s\n", "Wifi GUI Init");
  int ePage = E_PG_WIFI;
  gslc_PageAdd(pGui, ePage, pg_wifiElem, MAX_ELEM_PG_WIFI_RAM, pg_wifiElemRef, MAX_ELEM_PG_WIFI);

  // Set Background to a flat color
  gslc_SetBkgndColor(pGui, GSLC_COL_BLACK);

  gslc_tsElemRef* pElemRef = NULL;
  gslc_tsRect rFullScreen = {0,0,480,320};
  int xHei = 20;

/*
  // Main View Box
  pg_wifiEl[E_WIFI_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, rFullScreen);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_wifiEl[E_WIFI_EL_BOX], &pg_wifi_cbDrawBox);
*/

  // Close Key
  pg_wifiEl[E_WIFI_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {((rFullScreen.x + rFullScreen.w) - 100),(rFullScreen.y + 5),100,50},
          "Close", 0, E_FONT_MONO14, &pg_wifi_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], true);

  // Saved Networks
  pg_wifiEl[E_WIFI_EL_LIST_NETWORKS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullScreen.x + 5), ((rFullScreen.y + rFullScreen.h) - 55), 125, 50},
          "Saved Networks", 0, E_FONT_MONO14, &pg_wifi_cbBtn_savedNetworks);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], true);


  // Available Networks
  pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullScreen.x + 150), ((rFullScreen.y + rFullScreen.h) - 55), 125, 50},
          "Scan Networks", 0, E_FONT_MONO14, &pg_wifi_cbBtn_availableNetworks);
  gslc_ElemSetTxtCol(pGui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS], true);



  // Reassociate To AP
  pg_wifiEl[E_WIFI_EL_REASSOCIATE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullScreen.x + 300), ((rFullScreen.y + rFullScreen.h) - 55), 125, 50},
          "Reassociate", 0, E_FONT_MONO14, &pg_wifi_cbBtn_reassociate);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], true);

  // Disable Network
  pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullScreen.x + 5),(rFullScreen.y + 5),125,50},
          "Disable", 0, E_FONT_MONO14, &pg_wifi_cbBtn_disableNetwork);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], true);

  // Enable Network
  pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullScreen.x + 150),(rFullScreen.y + 5),125,50},
          "Enable", 0, E_FONT_MONO14, &pg_wifi_cbBtn_enableNetwork);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], true);


  xHei = 20;
  pg_wifiEl[E_WIFI_EL_STATUS_SSID] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullScreen.x, (rFullScreen.y + 60 + (0 * xHei)),rFullScreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], false);

  pg_wifiEl[E_WIFI_EL_STATUS_IP] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullScreen.x, (rFullScreen.y + 60 + (1 * xHei)),rFullScreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], false);


  pg_wifiEl[E_WIFI_EL_STATUS_STATE] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullScreen.x, (rFullScreen.y + 60 + (2 * xHei)),rFullScreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], false);

  pg_wifiEl[E_WIFI_EL_STATUS_FREQ] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullScreen.x, (rFullScreen.y + 60 + (3 * xHei)),rFullScreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], false);

  pg_wifiEl[E_WIFI_EL_STATUS_MODE] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullScreen.x, (rFullScreen.y + 60 + (4 * xHei)),rFullScreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], false);


/*
  xHei = 40;
  for (size_t i = 0; i < 5; i++) {
    pElemRef = gslc_ElemCreateBtnTxt(pGui, (9800 + i), ePage,
            (gslc_tsRect) {rFullScreen.x, (rFullScreen.y + 60 + (i * xHei)),rFullScreen.w,xHei},
            (char*)" ", 0, E_FONT_MONO18, &CbBtnWifi);
    gslc_ElemSetTxtCol(pGui, pElemRef, GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLUE_LT3,GSLC_COL_BLUE_LT3);
    gslc_ElemSetTxtAlign(pGui,pElemRef,GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetTxtMarginXY(pGui,pElemRef,10,0);
    gslc_ElemSetFillEn(pGui,pElemRef,false);
    gslc_ElemSetFrameEn(pGui,pElemRef,true);
    gslc_ElemSetVisible(pGui,pElemRef,false);

    pElemRef = gslc_ElemCreateTxt(pGui, (9810 + i), ePage,
            (gslc_tsRect) {(rFullScreen.w - 100), (rFullScreen.y + 61 + (i * xHei)), 90, xHei - 2},
            (char*)" ", 0, E_FONT_MONO18);
    gslc_ElemSetTxtCol(pGui, pElemRef, GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLUE_LT3,GSLC_COL_BLUE_LT3);
    gslc_ElemSetTxtAlign(pGui,pElemRef,GSLC_ALIGN_MID_RIGHT);
    gslc_ElemSetTxtMarginXY(pGui,pElemRef,10,0);
    gslc_ElemSetFillEn(pGui,pElemRef,false);
    gslc_ElemSetFrameEn(pGui,pElemRef,false);
    gslc_ElemSetVisible(pGui,pElemRef,false);
  }
*/
  return 1;
}





// GUI Init
void pg_wifi_init(gslc_tsGui *pGui) {

  draw_wifi_flag = 0;
  // maxWifiNetworks = 100;

  m_wifi_list_selected = 0;
  m_wifi_list_page = 0;
  m_wifi_list_start = 0;

  // Create Interface
  pg_wifi_guiInit(pGui);

  cbInit[E_PG_WIFI] = NULL;
}


// GUI Open
void pg_wifi_open(gslc_tsGui *pGui) {
  // Initiaize Status Array
  pg_wifi_status = PG_WIFI_INIT_STATUS();

  // Initialize Network info arrays
  pg_wifi_nets_available = PG_WIFI_INIT_NETWORKS();
  pg_wifi_nets_saved = PG_WIFI_INIT_NETWORKS();

  // Open WPA Interface socket
  // ls -l /var/run/wpa_supplicant for list of interfaces
  pg_wifi_wpaOpen("/var/run/wpa_supplicant/wlan0");

  // Fetch and show current wifi status
  if (pg_wifi_getStatus()) {
    pg_wifi_showStatus(pGui);
  } else {
    // Status Unavailable, Show error
  }
}


// GUI Destroy
void pg_wifi_destroy() {
  PG_WIFI_DESTROY_STATUS(pg_wifi_status);
  PG_WIFI_DESTROY_NETWORKS(pg_wifi_nets_available);
  PG_WIFI_DESTROY_NETWORKS(pg_wifi_nets_saved);
}

void __attribute__ ((constructor)) pg_wifi_setup(void) {
  cbInit[E_PG_WIFI] = &pg_wifi_init;
  cbOpen[E_PG_WIFI] = &pg_wifi_open;
  cbDestroy[E_PG_WIFI] = &pg_wifi_destroy;
}
