#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "shared.h"
#include "lib_wifi_wpa.h"
#include "lib_wifi_networks.h"
#include "wifi.h"

#include "gui/pages.h"
#include "gui/main/main.h"
#include "gui/keyboard/keyboard.h"

/*
commands:
  status [verbose] = get current WPA/EAPOL/EAP status
  ifname = get current interface name
  ping = pings wpa_supplicant
  relog = re-open log-file (allow rolling logs)
  note <text> = add a note to wpa_supplicant debug log
  mib = get MIB variables (dot1x, dot11)
  help [command] = show usage help
  interface [ifname] = show interfaces/select interface
  level <debug level> = change debug level
  license = show full wpa_cli license
  quit = exit wpa_cli
  set = set variables (shows list of variables when run without arguments)
  dump = dump config variables
  get <name> = get information
  driver_flags = list driver flags
  logon = IEEE 802.1X EAPOL state machine logon
  logoff = IEEE 802.1X EAPOL state machine logoff
  pmksa = show PMKSA cache
  pmksa_flush = flush PMKSA cache entries
  pmksa_get <network_id> = fetch all stored PMKSA cache entries
  pmksa_add <network_id> <BSSID> <PMKID> <PMK> <reauth_time in seconds> <expiration in seconds> <akmp> <opportunistic> = store PMKSA cache entry from external storage
  reassociate = force reassociation
  reattach = force reassociation back to the same BSS
  preauthenticate <BSSID> = force preauthentication
  identity <network id> <identity> = configure identity for an SSID
  password <network id> <password> = configure password for an SSID
  new_password <network id> <password> = change password for an SSID
  pin <network id> <pin> = configure pin for an SSID
  otp <network id> <password> = configure one-time-password for an SSID
  psk_passphrase <network id> <PSK/passphrase> = configure PSK/passphrase for an SSID
  passphrase <network id> <passphrase> = configure private key passphrase
    for an SSID
  sim <network id> <pin> = report SIM operation result
  bssid <network id> <BSSID> = set preferred BSSID for an SSID
  blacklist <BSSID> = add a BSSID to the blacklist
  blacklist clear = clear the blacklist
  blacklist = display the blacklist
  log_level <level> [<timestamp>] = update the log level/timestamp
  log_level = display the current log level and log options
  list_networks = list configured networks
  select_network <network id> = select a network (disable others)
  enable_network <network id> = enable a network
  disable_network <network id> = disable a network
  add_network = add a network
  remove_network <network id> = remove a network
  set_network <network id> <variable> <value> = set network variables (shows
    list of variables when run without arguments)
  get_network <network id> <variable> = get network variables
  dup_network <src network id> <dst network id> <variable> = duplicate network variables
  list_creds = list configured credentials
  add_cred = add a credential
  remove_cred <cred id> = remove a credential
  set_cred <cred id> <variable> <value> = set credential variables
  get_cred <cred id> <variable> = get credential variables
  save_config = save the current configuration
  disconnect = disconnect and wait for reassociate/reconnect command before
    connecting
  reconnect = like reassociate, but only takes effect if already disconnected
  scan = request new BSS scan
  scan_results = get latest scan results
  abort_scan = request ongoing scan to be aborted
  bss <<idx> | <bssid>> = get detailed scan result info
  get_capability <eap/pairwise/group/key_mgmt/proto/auth_alg/channels/freq/modes> = get capabilities
  reconfigure = force wpa_supplicant to re-read its configuration file
  terminate = terminate wpa_supplicant
  interface_add <ifname> <confname> <driver> <ctrl_interface> <driver_param>
    <bridge_name> <create> <type> = adds new interface, all parameters but
    <ifname> are optional. Supported types are station ('sta') and AP ('ap')
  interface_remove <ifname> = removes the interface
  interface_list = list available interfaces
  ap_scan <value> = set ap_scan parameter
  scan_interval <value> = set scan_interval parameter (in seconds)
  bss_expire_age <value> = set BSS expiration age parameter
  bss_expire_count <value> = set BSS expiration scan count parameter
  bss_flush <value> = set BSS flush age (0 by default)
  ft_ds <addr> = request over-the-DS FT with <addr>
  wps_pbc [BSSID] = start Wi-Fi Protected Setup: Push Button Configuration
  wps_pin <BSSID> [PIN] = start WPS PIN method (returns PIN, if not hardcoded)
  wps_check_pin <PIN> = verify PIN checksum
  wps_cancel Cancels the pending WPS operation
  wps_nfc [BSSID] = start Wi-Fi Protected Setup: NFC
  wps_nfc_config_token <WPS|NDEF> = build configuration token
  wps_nfc_token <WPS|NDEF> = create password token
  wps_nfc_tag_read <hexdump of payload> = report read NFC tag with WPS data
  nfc_get_handover_req <NDEF> <WPS> = create NFC handover request
  nfc_get_handover_sel <NDEF> <WPS> = create NFC handover select
  nfc_report_handover <role> <type> <hexdump of req> <hexdump of sel> = report completed NFC handover
  wps_reg <BSSID> <AP PIN> = start WPS Registrar to configure an AP
  wps_ap_pin [params..] = enable/disable AP PIN
  wps_er_start [IP address] = start Wi-Fi Protected Setup External Registrar
  wps_er_stop = stop Wi-Fi Protected Setup External Registrar
  wps_er_pin <UUID> <PIN> = add an Enrollee PIN to External Registrar
  wps_er_pbc <UUID> = accept an Enrollee PBC using External Registrar
  wps_er_learn <UUID> <PIN> = learn AP configuration
  wps_er_set_config <UUID> <network id> = set AP configuration for enrolling
  wps_er_config <UUID> <PIN> <SSID> <auth> <encr> <key> = configure AP
  wps_er_nfc_config_token <WPS/NDEF> <UUID> = build NFC configuration token
  ibss_rsn <addr> = request RSN authentication with <addr> in IBSS
  sta <addr> = get information about an associated station (AP)
  all_sta = get information about all associated stations (AP)
  list_sta = list all stations (AP)
  deauthenticate <addr> = deauthenticate a station
  disassociate <addr> = disassociate a station
  chan_switch <cs_count> <freq> [sec_channel_offset=] [center_freq1=] [center_freq2=] [bandwidth=] [blocktx] [ht|vht] = CSA parameters
  suspend = notification of suspend/hibernate
  resume = notification of resume/thaw
  roam <addr> = roam to the specified BSS
  p2p_find [timeout] [type=*] = find P2P Devices for up-to timeout seconds
  p2p_stop_find = stop P2P Devices search
  p2p_asp_provision <addr> adv_id=<adv_id> conncap=<conncap> [info=<infodata>] = provision with a P2P ASP Device
  p2p_asp_provision_resp <addr> adv_id=<adv_id> [role<conncap>] [info=<infodata>] = provision with a P2P ASP Device
  p2p_connect <addr> <"pbc"|PIN> [ht40] = connect to a P2P Device
  p2p_listen [timeout] = listen for P2P Devices for up-to timeout seconds
  p2p_group_remove <ifname> = remove P2P group interface (terminate group if GO)
  p2p_group_add [ht40] = add a new P2P group (local end as GO)
  p2p_group_member <dev_addr> = Get peer interface address on local GO using peer Device Address
  p2p_prov_disc <addr> <method> = request provisioning discovery
  p2p_get_passphrase = get the passphrase for a group (GO only)
  p2p_serv_disc_req <addr> <TLVs> = schedule service discovery request
  p2p_serv_disc_cancel_req <id> = cancel pending service discovery request
  p2p_serv_disc_resp <freq> <addr> <dialog token> <TLVs> = service discovery response
  p2p_service_update = indicate change in local services
  p2p_serv_disc_external <external> = set external processing of service discovery
  p2p_service_flush = remove all stored service entries
  p2p_service_add <bonjour|upnp|asp> <query|version> <response|service> = add a local service
  p2p_service_rep asp <auto> <adv_id> <svc_state> <svc_string> [<svc_info>] = replace local ASP service
  p2p_service_del <bonjour|upnp> <query|version> [|service] = remove a local service
  p2p_reject <addr> = reject connection attempts from a specific peer
  p2p_invite <cmd> [peer=addr] = invite peer
  p2p_peers [discovered] = list known (optionally, only fully discovered) P2P peers
  p2p_peer <address> = show information about known P2P peer
  p2p_set <field> <value> = set a P2P parameter
  p2p_flush = flush P2P state
  p2p_cancel = cancel P2P group formation
  p2p_unauthorize <address> = unauthorize a peer
  p2p_presence_req [<duration> <interval>] [<duration> <interval>] = request GO presence
  p2p_ext_listen [<period> <interval>] = set extended listen timing
  p2p_remove_client <address|iface=address> = remove a peer from all groups
  vendor_elem_add <frame id> <hexdump of elem(s)> = add vendor specific IEs to frame(s)
    0: Probe Req (P2P), 1: Probe Resp (P2P) , 2: Probe Resp (GO), 3: Beacon (GO), 4: PD Req, 5: PD Resp, 6: GO Neg Req, 7: GO Neg Resp, 8: GO Neg Conf, 9: Inv Req, 10: Inv Resp, 11: Assoc Req (P2P), 12: Assoc Resp (P2P)
  vendor_elem_get <frame id> = get vendor specific IE(s) to frame(s)
    0: Probe Req (P2P), 1: Probe Resp (P2P) , 2: Probe Resp (GO), 3: Beacon (GO), 4: PD Req, 5: PD Resp, 6: GO Neg Req, 7: GO Neg Resp, 8: GO Neg Conf, 9: Inv Req, 10: Inv Resp, 11: Assoc Req (P2P), 12: Assoc Resp (P2P)
  vendor_elem_remove <frame id> <hexdump of elem(s)> = remove vendor specific IE(s) in frame(s)
    0: Probe Req (P2P), 1: Probe Resp (P2P) , 2: Probe Resp (GO), 3: Beacon (GO), 4: PD Req, 5: PD Resp, 6: GO Neg Req, 7: GO Neg Resp, 8: GO Neg Conf, 9: Inv Req, 10: Inv Resp, 11: Assoc Req (P2P), 12: Assoc Resp (P2P)
  wfd_subelem_set <subelem> [contents] = set Wi-Fi Display subelement
  wfd_subelem_get <subelem> = get Wi-Fi Display subelement
  fetch_anqp = fetch ANQP information for all APs
  stop_fetch_anqp = stop fetch_anqp operation
  interworking_select [auto] = perform Interworking network selection
  interworking_connect <BSSID> = connect using Interworking credentials
  interworking_add_network <BSSID> = connect using Interworking credentials
  anqp_get <addr> <info id>[,<info id>]... = request ANQP information
  gas_request <addr> <AdvProtoID> [QueryReq] = GAS request
  gas_response_get <addr> <dialog token> [start,len] = Fetch last GAS response
  hs20_anqp_get <addr> <subtype>[,<subtype>]... = request HS 2.0 ANQP information
  nai_home_realm_list <addr> <home realm> = get HS20 nai home realm list
  hs20_icon_request <addr> <icon name> = get Hotspot 2.0 OSU icon
  fetch_osu = fetch OSU provider information from all APs
  cancel_fetch_osu = cancel fetch_osu command
  sta_autoconnect <0/1> = disable/enable automatic reconnection
  tdls_discover <addr> = request TDLS discovery with <addr>
  tdls_setup <addr> = request TDLS setup with <addr>
  tdls_teardown <addr> = tear down TDLS with <addr>
  tdls_link_status <addr> = TDLS link status with <addr>
  wmm_ac_addts <uplink/downlink/bidi> <tsid=0..7> <up=0..7> [nominal_msdu_size=#] [mean_data_rate=#] [min_phy_rate=#] [sba=#] [fixed_nominal_msdu] = add WMM-AC traffic stream
  wmm_ac_delts <tsid> = delete WMM-AC traffic stream
  wmm_ac_status = show status for Wireless Multi-Media Admission-Control
  tdls_chan_switch <addr> <oper class> <freq> [sec_channel_offset=] [center_freq1=] [center_freq2=] [bandwidth=] [ht|vht] = enable channel switching with TDLS peer
  tdls_cancel_chan_switch <addr> = disable channel switching with TDLS peer <addr>
  signal_poll = get signal parameters
  signal_monitor = set signal monitor parameters
  pktcnt_poll = get TX/RX packet counters
  reauthenticate = trigger IEEE 802.1X/EAPOL reauthentication
  autoscan [params] = Set or unset (if none) autoscan parameters
  raw <params..> = Sent unprocessed command
  flush = flush wpa_supplicant state
  radio_work = radio_work <show/add/done>
  vendor <vendor id> <command id> [<hex formatted command argument>] = Send vendor command
  neighbor_rep_request [ssid=<SSID>] [lci] [civic] = Trigger request to AP for neighboring AP report (with optional given SSID in hex or enclosed in double quotes, default: current SSID; with optional LCI and location civic request)
  erp_flush = flush ERP keys
  mac_rand_scan <scan|sched|pno|all> enable=<0/1> [addr=mac-address mask=mac-address-mask] = scan MAC randomization
  get_pref_freq_list <interface type> = retrieve preferred freq list for the specified interface type
  p2p_lo_start <freq> <period> <interval> <count> = start P2P listen offload
  p2p_lo_stop = stop P2P listen offload
  dpp_qr_code report a scanned DPP URI from a QR Code
  dpp_bootstrap_gen type=<qrcode> [chan=..] [mac=..] [info=..] [curve=..] [key=..] = generate DPP bootstrap information
  dpp_bootstrap_remove *|<id> = remove DPP bootstrap information
  dpp_bootstrap_get_uri <id> = get DPP bootstrap URI
  dpp_bootstrap_info <id> = show DPP bootstrap information
  dpp_auth_init peer=<id> [own=<id>] = initiate DPP bootstrapping
  dpp_listen <freq in MHz> = start DPP listen
  dpp_stop_listen = stop DPP listen
  dpp_configurator_add [curve=..] [key=..] = add DPP configurator
  dpp_configurator_remove *|<id> = remove DPP configurator
  dpp_configurator_get_key <id> = Get DPP configurator's private key
  dpp_configurator_sign conf=<role> configurator=<id> = generate self DPP configuration
  dpp_pkex_add add PKEX code
  dpp_pkex_remove *|<id> = remove DPP pkex information
*/

void pg_wifi_cbBtn_setSSID(gslc_tsGui *pGui, char* str) {
  //
  size_t strSz = snprintf(NULL, 0, "%s", str) + 1;
  if (strSz > pg_wifi_addInput->ssidMax) {
    pg_wifi_addInput->ssidMax += 128;
    char *newPtr = (char *)realloc(pg_wifi_addInput->ssidPtr, pg_wifi_addInput->ssidMax * sizeof(char));
    pg_wifi_addInput->ssidPtr = newPtr;
  }
  snprintf(pg_wifi_addInput->ssidPtr, strSz, "%s", str);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN], pg_wifi_addInput->ssidPtr);
}

void pg_wifi_cbBtn_setPass(gslc_tsGui *pGui, char* str) {
  //
  size_t strSz = snprintf(NULL, 0, "%s", str) + 1;
  if (strSz > pg_wifi_addInput->passMax) {
    pg_wifi_addInput->passMax += 128;
    char *newPtr = (char *)realloc(pg_wifi_addInput->passPtr, pg_wifi_addInput->passMax * sizeof(char));
    pg_wifi_addInput->passPtr = newPtr;
  }
  snprintf(pg_wifi_addInput->passPtr, strSz, "%s", str);
  if (strSz > 1) {
    gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], (char*)"*** Set ***");
  } else {
    gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], (char*)" ");
  }
}

void pg_wifi_cbBtn_resetSSID(gslc_tsGui *pGui) {
  pg_wifi_cbBtn_setSSID(pGui, "");
  pg_wifi_cbBtn_setPass(pGui, "");
}

//////////////////////////////////
//
// GUI CALLBACK BUTTONS
//
bool pg_wifi_cbBtn_SSID(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_keyboard_show(pGui, 16, pg_wifi_addInput->ssidPtr, &pg_wifi_cbBtn_setSSID);
  return true;
}

bool pg_wifi_cbBtn_SSIDPass(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_keyboard_show(pGui, 16, pg_wifi_addInput->passPtr, &pg_wifi_cbBtn_setPass);
  return true;
}

bool pg_wifi_cbBtn_findnet(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_WIFI_LIST);
  return true;
}

bool pg_wifi_cbBtn_savednet(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_WIFI_LIST_SAVED);
  return true;
}

bool pg_wifi_cbBtn_connect(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Add Network get id

  // set_network 1 ssid "TEST1"
  // set_network 1 psk "TESTPASS!@#"
  // password 1 "TEST!PASDF#"
  // enable_network
  // save_config

  size_t len = 32;
  char buf[len + 1];
  CLEAR(buf, len + 1);
  buf[len] = '\0';

  pg_wifi_wpaSendCmdBuf("ADD_NETWORK", buf, &len);
  char *pEnd;
  int network_id = strtol(buf, &pEnd, 10);
  if (pEnd == buf
      || errno == ERANGE
  ) {
    printf("Unable to add Network: %s\nNetid: %d\n", buf, network_id);
    return false;
  }

  size_t strSsidSz = snprintf(NULL, 0, "SET_NETWORK %d ssid \"%s\"", network_id, pg_wifi_addInput->ssidPtr) + 1;
  char *strSsidCmd = (char *)malloc(strSsidSz * sizeof(char));
  snprintf(strSsidCmd, strSsidSz, "SET_NETWORK %d ssid \"%s\"", network_id, pg_wifi_addInput->ssidPtr);

  size_t strSsidRetSz = 32;
  char strSsidRet[strSsidRetSz + 1];
  CLEAR(strSsidRet, strSsidRetSz + 1);
  strSsidRet[strSsidRetSz] = '\0';

  pg_wifi_wpaSendCmdBuf(strSsidCmd, strSsidRet, &strSsidRetSz);
  if (strcmp(strSsidRet, "FAIL") == 0) {
    // Failed adding SSID, unwind;
    pg_wifi_showErrorMsg(pGui, "SSID value failed");
    return true;
  }
  free(strSsidCmd);

  size_t strPassSz = snprintf(NULL, 0, "SET_NETWORK %d psk \"%s\"", network_id, pg_wifi_addInput->passPtr) + 1;
  char *strPassCmd = (char *)malloc(strPassSz * sizeof(char));
  snprintf(strPassCmd, strPassSz, "SET_NETWORK %d psk \"%s\"", network_id, pg_wifi_addInput->passPtr);

  size_t strPassRetSz = 32;
  char strPassRet[strPassRetSz + 1];
  CLEAR(strPassRet, strPassRetSz + 1);
  strPassRet[strPassRetSz] = '\0';

  pg_wifi_wpaSendCmdBuf(strPassCmd, strPassRet, &strPassRetSz);
  if (strcmp(strPassRet, "FAIL") == 0) {
    // Failed adding SSID, unwind;
    pg_wifi_showErrorMsg(pGui, "Password value failed");
    return true;
  }
  free(strPassCmd);

  pg_wifi_enableNetwork(network_id);
  pg_wifi_wpaSendCmd("SAVE_CONFIG");

  pg_wifi_cbBtn_resetSSID(pGui);

  return true;
}


bool pg_wifi_cbBtn_close(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageOpen(pGui, E_PG_MAIN);
  return true;
}


bool pg_wifi_cbBtn_reassociate(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_wpaSendCmd("REASSOCIATE");
  return true;
}


bool pg_wifi_cbBtn_refresh(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_getStatus();
  pg_wifi_showStatus(pGui);

  return true;
}


bool pg_wifi_cbBtn_disableNetwork(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_disableNetwork(pg_wifi_status->id);
  return true;
}

bool pg_wifi_cbBtn_enableNetwork(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_enableNetwork(pg_wifi_status->id);
  return true;
}

bool pg_wifi_cbBtn_errorMsgOk(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_wifi_closeErrorMsg(pGui);

  return true;
}











/////////////////////////////
//
// GUI DISPLAY FUNCTIONS
void pg_wifi_showStatus(gslc_tsGui *pGui) {
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], pg_wifi_status->ip_address);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], pg_wifi_status->ssid);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], pg_wifi_status->wpa_state);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], pg_wifi_status->freq);
  gslc_ElemSetTxtStr(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], pg_wifi_status->mode);
}


void pg_wifi_showErrorMsg(gslc_tsGui *pGui, char* str) {
  // Hide Menu Items
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BTN_FINDNET], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BTN_SAVEDNET], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BTN_CONNECT], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_REFRESH], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], false);

  // Populate Textbox
  gslc_ElemXTextboxReset(pGui, pg_wifiEl[E_WIFI_EL_ERROR_MSG]);
  gslc_ElemXTextboxAdd(pGui, pg_wifiEl[E_WIFI_EL_ERROR_MSG], str);

  // Show Error Message
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_ERROR_HEADER], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_ERROR_MSG], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_ERROR_OK], true);
}



void pg_wifi_closeErrorMsg(gslc_tsGui *pGui) {
  // Hide Error Msg box
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_ERROR_HEADER], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_ERROR_MSG], false);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_ERROR_OK], false);

  // Show Menu Items
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BTN_FINDNET], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BTN_SAVEDNET], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_BTN_CONNECT], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_REFRESH], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], true);
  gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], true);

  // Clean Textbox
  // gslc_ElemXTextboxReset(pGui, pg_wifiEl[E_WIFI_EL_ERROR_MSG]);
}




bool pg_wifi_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{

  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  gslc_DrawFillRect(pGui,pRect,pElem->colElemFill);


  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);

  return true;
}









int pg_wifi_guiInit(gslc_tsGui *pGui)
{
  // debug_print("%s\n", "Wifi GUI Init");
  int ePage = E_PG_WIFI;
  gslc_PageAdd(pGui, ePage, pg_wifiElem, MAX_ELEM_PG_WIFI_RAM, pg_wifiElemRef, MAX_ELEM_PG_WIFI);

  int xHei = 20;

  // Main View Box
  pg_wifiEl[E_WIFI_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, (gslc_tsRect){0,170,420,120});
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pGui, pg_wifiEl[E_WIFI_EL_BOX], &pg_wifi_cbDrawBox);



  pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0,165,480,50}, (char*)"SSID:", 0, E_FONT_MONO18, &pg_wifi_cbBtn_SSID);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT], GSLC_COL_WHITE, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT], 8, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT], true);
  gslc_ElemSetGlowEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_TXT], false);

  pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0,220,480,50}, (char*)"Pass:", 0, E_FONT_MONO18, &pg_wifi_cbBtn_SSIDPass);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT], GSLC_COL_WHITE, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT], 8, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT], true);
  gslc_ElemSetGlowEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_TXT], false);


  pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {70,165,405,50}, pg_wifi_addInput->ssidPtr, 0, E_FONT_MONO24, &pg_wifi_cbBtn_SSID);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN], GSLC_COL_BLACK, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN], false);
  gslc_ElemSetGlowEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_SSID_BTN], false);



  pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {70,220,405,50}, (char*)" ", 0, E_FONT_MONO18, &pg_wifi_cbBtn_SSIDPass);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], GSLC_COL_BLACK, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], false);
  gslc_ElemSetGlowEn(pGui, pg_wifiEl[E_WIFI_EL_BOX_PASS_BTN], false);

  pg_wifiEl[E_WIFI_EL_BTN_FINDNET] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {0,275,152,45}, (char*)"Search WIFI", 0, E_FONT_MONO18, &pg_wifi_cbBtn_findnet);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BTN_FINDNET], GSLC_COL_BLACK, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_BTN_FINDNET], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_FINDNET], true);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_FINDNET], true);
  gslc_ElemSetGlowEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_FINDNET], false);

  pg_wifiEl[E_WIFI_EL_BTN_SAVEDNET] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {164,275,152,45}, (char*)"Saved", 0, E_FONT_MONO18, &pg_wifi_cbBtn_savednet);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BTN_SAVEDNET], GSLC_COL_BLACK, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_BTN_SAVEDNET], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_SAVEDNET], true);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_SAVEDNET], true);
  gslc_ElemSetGlowEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_SAVEDNET], false);

  pg_wifiEl[E_WIFI_EL_BTN_CONNECT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {328,275,152,45}, (char*)"Connect", 0, E_FONT_MONO18, &pg_wifi_cbBtn_connect);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_BTN_CONNECT], GSLC_COL_BLACK, GSLC_COL_GREEN_DK2, GSLC_COL_BLACK);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_BTN_CONNECT], GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_BTN_CONNECT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_CONNECT], true);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_CONNECT], true);
  gslc_ElemSetGlowEn(pGui, pg_wifiEl[E_WIFI_EL_BTN_CONNECT], false);



  // Close Key
  pg_wifiEl[E_WIFI_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 100),(rFullscreen.y + 5),100,50},
          "Close", 0, E_FONT_MONO14, &pg_wifi_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_CLOSE], true);

/*
  // Refresh Networks
  pg_wifiEl[E_WIFI_EL_REFRESH_NETWORKS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {425, 180, 55, 130},
          "Refresh", 0, E_FONT_MONO14, &pg_wifi_cbBtn_refreshNetworks);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_REFRESH_NETWORKS], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_REFRESH_NETWORKS], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_REFRESH_NETWORKS], GSLC_ALIGNV_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_REFRESH_NETWORKS], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_REFRESH_NETWORKS], true);

  // Saved Networks
  pg_wifiEl[E_WIFI_EL_LIST_NETWORKS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {425, 180, 55, 30},
          "S", 0, E_FONT_MONO14, &pg_wifi_cbBtn_savedNetworks);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_LIST_NETWORKS], true);


  // Available Networks
  pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {425, 220, 55, 30},
          "A", 0, E_FONT_MONO14, &pg_wifi_cbBtn_availableNetworks);
  gslc_ElemSetTxtCol(pGui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_SCAN_NETWORKS], true);
*/



  // Disable Network
  pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x),(rFullscreen.y + 5),105,50},
          "Disable", 0, E_FONT_MONO14, &pg_wifi_cbBtn_disableNetwork);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_DISABLE_NETWORK], true);

  // Enable Network
  pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x + 130),(rFullscreen.y + 5),105,50},
          "Enable", 0, E_FONT_MONO14, &pg_wifi_cbBtn_enableNetwork);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_ENABLE_NETWORK], true);

  // Reassociate To AP
  pg_wifiEl[E_WIFI_EL_REASSOCIATE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {(rFullscreen.x + 255),(rFullscreen.y + 5),105,50},
          "Reset", 0, E_FONT_MONO14, &pg_wifi_cbBtn_reassociate);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_REASSOCIATE], true);

  // Refresh Status
  pg_wifiEl[E_WIFI_EL_REFRESH] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {((rFullscreen.x + rFullscreen.w) - 100),(rFullscreen.y + 100),100,50},
          "Refresh", 0, E_FONT_MONO14, &pg_wifi_cbBtn_refresh);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_REFRESH], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_REFRESH], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_REFRESH], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_REFRESH], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_REFRESH], true);

  xHei = 20;
  pg_wifiEl[E_WIFI_EL_STATUS_SSID] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + 60 + (0 * xHei)),rFullscreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_SSID], false);

  pg_wifiEl[E_WIFI_EL_STATUS_IP] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + 60 + (1 * xHei)),rFullscreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_IP], false);


  pg_wifiEl[E_WIFI_EL_STATUS_STATE] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + 60 + (2 * xHei)),rFullscreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_STATE], false);

  pg_wifiEl[E_WIFI_EL_STATUS_FREQ] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + 60 + (3 * xHei)),rFullscreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_FREQ], false);

  pg_wifiEl[E_WIFI_EL_STATUS_MODE] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + 60 + (4 * xHei)),rFullscreen.w, xHei},
          (char*)"", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtMarginXY(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], 10, 0);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], false);
  // gslc_ElemSetVisible(pGui, pg_wifiEl[E_WIFI_EL_STATUS_MODE], false);


  // Create textbox
  pg_wifiEl[E_WIFI_EL_ERROR_MSG] = gslc_ElemXTextboxCreate(pGui, GSLC_ID_AUTO, ePage,
          &pg_wifi_errorMsgTextbox,
          (gslc_tsRect) {rFullscreen.x, (rFullscreen.y + 60), rFullscreen.w, (rFullscreen.h - 60)},
          E_FONT_MONO18, (char*)&pg_wifi_errorMsgBuf, pg_wifi_errorMsgRows, pg_wifi_errorMsgCols);
  gslc_ElemXTextboxWrapSet(pGui, pg_wifiEl[E_WIFI_EL_ERROR_MSG], true);
  gslc_ElemSetCol(pGui, pg_wifiEl[E_WIFI_EL_ERROR_MSG], GSLC_COL_BLUE_LT2, GSLC_COL_BLACK, GSLC_COL_GRAY_DK3);

  pg_wifiEl[E_WIFI_EL_ERROR_HEADER] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x, rFullscreen.y, rFullscreen.w, 60},
          (char*)"ERROR MESSAGE", 0, E_FONT_MONO18);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_ERROR_HEADER], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_ERROR_HEADER], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_ERROR_HEADER], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_ERROR_HEADER], false);

  pg_wifiEl[E_WIFI_EL_ERROR_OK] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {rFullscreen.x + rFullscreen.w - 80, rFullscreen.y, 80, 60},
          (char*)"OK", 0, E_FONT_MONO18, &pg_wifi_cbBtn_errorMsgOk);
  gslc_ElemSetTxtCol(pGui, pg_wifiEl[E_WIFI_EL_ERROR_OK], GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pGui, pg_wifiEl[E_WIFI_EL_ERROR_OK], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_wifiEl[E_WIFI_EL_ERROR_OK], false);
  gslc_ElemSetFrameEn(pGui, pg_wifiEl[E_WIFI_EL_ERROR_OK], false);

  return 1;
}



// pg_wifi_wpaSetEventCallbackFunc ?
void pg_wifi_wpaEvent(char* event) {
  if (strcmp(event, "CTRL-EVENT-SCAN-RESULTS ") == 0) {
    pg_wifi_updateAvailableNetworks();
  }

  if (pg_wifi_getStatus()) {
    pg_wifi_showStatus(&m_gui);
  }
  /*
  char *newTxt = malloc(strlen(str) + 1);
  strlcpy(newTxt, str, strlen(str) + 1);
  scorecard->judge = newTxt;
  gslc_ElemSetTxtStr(pGui, m_pElemScorecardJudge, str);
  */
}


// GUI Init
void pg_wifi_init(gslc_tsGui *pGui) {

  // Initialize Wifi Input
  pg_wifi_addInput = PG_WIFI_INIT_INPUT();

  // Initiaize Status Array
  pg_wifi_status = PG_WIFI_INIT_STATUS();

  // Initialize Network info arrays
  pg_wifi_nets_available = PG_WIFI_INIT_NETWORKS();
  pg_wifi_nets_saved = PG_WIFI_INIT_NETWORKS();
  pg_wifi_net_selected = NULL;

  pg_wifi_errorMsgBuf = calloc(pg_wifi_errorMsgRows * pg_wifi_errorMsgCols, sizeof(char));
  // Create Interface
  pg_wifi_guiInit(pGui);

  // pg_wifi_setInterface("wlan0");
  pg_wifi_wpaScanning = 0;
  pg_wifi_wpaOpen("/var/run/wpa_supplicant/wlan0");
  pg_wifi_wpaEvents("/var/run/wpa_supplicant/wlan0");

  // pg_wifi_wpaSendCmd("SCAN");

  cbInit[E_PG_WIFI] = NULL;
}


// GUI Open
void pg_wifi_open(gslc_tsGui *pGui) {

  // Open WPA Interface socket
  // ls -l /var/run/wpa_supplicant for list of interfaces
  pg_wifi_wpaSetEventCallbackFunc(&pg_wifi_wpaEvent);

  // Close any stale error message
  pg_wifi_closeErrorMsg(pGui);

  // Fetch and show current wifi status
  if (pg_wifi_getStatus()) {
    pg_wifi_showStatus(pGui);
  } else {
    // Status Unavailable, Show error
  }

  // Show Selected In Box
  if (pg_wifi_net_selected) {
    pg_wifi_cbBtn_setSSID(pGui, pg_wifi_net_selected->ssid);
    // pg_wifi_cbBtn_setPass(pGui, "");
    // pg_wifi_net_selected = NULL;
  }
}

void pg_wifi_close(gslc_tsGui *pGui) {
  pg_wifi_wpaRemoveEventCallbackFunc(0);
}

// GUI Destroy
void pg_wifi_destroy() {
  PG_WIFI_DESTROY_INPUT(pg_wifi_addInput);
  PG_WIFI_DESTROY_STATUS(pg_wifi_status);

  pg_wifi_clearNetworks(&pg_wifi_nets_available);
  pg_wifi_clearNetworks(&pg_wifi_nets_saved);

  PG_WIFI_DESTROY_NETWORKS(pg_wifi_nets_available);
  PG_WIFI_DESTROY_NETWORKS(pg_wifi_nets_saved);

  free(pg_wifi_errorMsgBuf);

  pg_wifi_wpaClose();
}

void __attribute__ ((constructor)) pg_wifi_setup(void) {
  cbInit[E_PG_WIFI] = &pg_wifi_init;
  cbOpen[E_PG_WIFI] = &pg_wifi_open;
  cbClose[E_PG_WIFI] = &pg_wifi_close;
  cbDestroy[E_PG_WIFI] = &pg_wifi_destroy;
}
