#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <netinet/in.h>
#include <wiringPi.h> // Gordons Wiring Pi
#include <unistd.h>

#include "lib_wifi_wpa.h"
#include "shared.h"
#include "dbg/dbg.h"

// pg_wifi_wpaSetEventCallbackFunc ?
void pg_wifi_wpaEventCB(char* event) {
  if (strcmp(event, "CTRL-EVENT-SCAN-STARTED ") == 0) {
    pg_wifi_wpaScanning = 1;
  } else if (strcmp(event, "CTRL-EVENT-SCAN-RESULTS ") == 0) {
    pg_wifi_wpaScanning = 0;
  }
}

// ------------------------
// WPA Event Thread
// ------------------------
PI_THREAD (pg_wifi_wpaEventThread) {

  if (pg_wifi_wpaEventThreadStop) {
    // debug_print("%s\n", "Not Starting WPA Event Thread, Stop Flag Set");
    pg_wifi_wpaEventThreadRunning = 0;
    return NULL;
  }

  int rc;
  const int maxBuf = 4096;
  char buf[maxBuf];
  buf[maxBuf - 1] = '\0';
  size_t len = maxBuf - 1;

  dbgprintf(DBG_INFO, "%s\n", "Started WPA Event Thread");
  while (!pg_wifi_wpaEventThreadStop) {
    rc = wpa_ctrl_recv(pg_wifi_wpa_events, buf, &len);
    buf[len-1] = '\0';

    if (rc == 0 && len > 3) {
      // 60 59 61 bits at the beginning of the event, not sure why yet
      memmove(buf, buf+3, strlen(buf) - 1);
      buf[strlen(buf)] = '\0';
      dbgprintf(DBG_WPA, "WPA Event: %s\n", buf);
      // call functions requesting access to events
      pg_wifi_wpaEventCB(buf);
      if (cbEvent[0]) { cbEvent[0](buf); }

      // Reset maximum buffer length
      len = maxBuf - 1;
    } else {
      usleep(250000);
    }
  }

  // debug_print("%s\n", "Closing WPA Event Thread");
  pg_wifi_wpaEventThreadRunning = 0;
  wpa_ctrl_detach(pg_wifi_wpa_conn);
  wpa_ctrl_close(pg_wifi_wpa_conn);

  return NULL;
}


int pg_wifi_wpaEvents(char* wpa_interface) {
  if (pg_wifi_wpaEventThreadRunning) {
    dbgprintf(DBG_DEBUG, "Not Starting WPA Event Thread, Already Started\n");
    return 0;
  }
  pg_wifi_wpaEventThreadRunning = 1;

  pg_wifi_wpa_events = wpa_ctrl_open(wpa_interface);
  if (pg_wifi_wpa_events == NULL) {
    dbgprintf(DBG_ERROR, "Failed to connect to wpa_supplicant global interface: %s error: %s\n", wpa_interface, strerror(errno));
    return 0;
  } else {
    dbgprintf(DBG_WPA, "WPA Connect Successful\n");
  }


  // Later to Attach Events
  wpa_ctrl_attach(pg_wifi_wpa_events);

  pg_wifi_wpaEventThreadStop = 0;
  if (pg_wifi_wpaTestConnection()) {
    if (piThreadCreate(pg_wifi_wpaEventThread) != 0) {
      dbgprintf(DBG_ERROR, "WPA Event Thread did not start");
      return 0;
    }
    return 1;
  }

  return 0;
}

int pg_wifi_wpaOpen(char* wpa_interface) {
  pg_wifi_wpaScanning = 0;
  pg_wifi_wpa_conn = wpa_ctrl_open(wpa_interface);
  if (pg_wifi_wpa_conn == NULL) {
    dbgprintf(DBG_ERROR, "Failed to open to wpa_supplicant global interface: %s error: %s\n", wpa_interface, strerror(errno));
    return 0;
  } else {
    dbgprintf(DBG_WPA, "WPA Open Successful\n");
  }

  return 1;
}

int pg_wifi_wpaSendCmdBuf(char* cmd, char ** buf) {
  // CLEAR(buf, lenB);
  size_t len = 4096;
  char b[len];
  b[len - 1] = '\0';

  int rc = wpa_ctrl_request(pg_wifi_wpa_conn, cmd, strlen(cmd), b, &len, NULL);
  if (rc == -1) {
    dbgprintf(DBG_ERROR, "wpa_ctrl_request() failed: %s.\n", strerror(errno));
    return 0;
  } else if (rc == -2) {
    dbgprintf(DBG_ERROR, "%s\n", "wpa_ctrl_request() timeout.");
    return 0;
  }
  dbgprintf(DBG_WPA, "WPA Cmd: %s\nLen: %d\nBuffer: %s\n", cmd, len, b);
  *buf = b;
  return len;
}

int pg_wifi_wpaTestConnection() {
  char* buf;
  int len = pg_wifi_wpaSendCmdBuf("PING", &buf);
  printf("Got Back: --%s--\n", buf);
  if (len >= 4 && strncmp(buf, "PONG", 4) == 0) {
    return 1;
  } else { return 0; }
}

int pg_wifi_wpaSendCmd(char* cmd) {
  char* buf;
  int len = pg_wifi_wpaSendCmdBuf(cmd, &buf);
  if (!len) {
    return 0;
  }
  // free(buf);
  return 1;
}

void pg_wifi_wpaClose() {
  // Shutdown WPA Event Thread
  if (pg_wifi_wpaEventThreadRunning) {
    pg_wifi_wpaEventThreadStop = 1;
    int shutdown_cnt = 0;
    while (pg_wifi_wpaEventThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    // debug_print("WPA Event Thread Shutdown %d\n", shutdown_cnt);
    wpa_ctrl_close(pg_wifi_wpa_conn);
  }
}


int pg_wifi_wpaSetEventCallbackFunc(void (*function)(char *)) {
  cbEvent[0] = function;
  return 0;
}

void pg_wifi_wpaRemoveEventCallbackFunc(int funcId) {
  cbEvent[funcId] = NULL;
  return;
}



/* // OLD iwlib method
#include <iwlib.h>
// Must install iwlib-dev
// `sudo apt install iwlib-dev`

// wifi_scan("wlan0");
int wifi_scanner(char* interface) {
  wireless_scan_head head;
  wireless_scan *result;
  iwrange range;
  int sock;

  // Open socket to kernel
  sock = iw_sockets_open();

  // Get some metadata to use for scanning
  if (iw_get_range_info(sock, interface, &range) < 0) {
    printf("Error during iw_get_range_info. Aborting.\n");
    return 0;
  }

  // Perform the scan
  if (iw_scan(sock, interface, range.we_version_compiled, &head) < 0) {
    printf("Error during iw_scan. Aborting.\n");
    return 0;
  }

  // Traverse the results
  result = head.result;
  while (NULL != result) {

    // ESSID
    printf("SSID: ");
    if (result->b.has_essid) {
      printf("%s", result->b.essid);
    } else {
      printf("(HIDDEN)");
    }

    if (result->b.essid_on) {
      printf(" (Active)");
    }

    if (result->b.has_mode) {
      printf(" Mode: %d", result->b.mode);
    }

    if (result->b.has_nwid) {
      printf(" NetworkID: %d", result->b.nwid.value);
    }


    // MAC Address
    if (result->has_ap_addr) {
      char address[128];
      snprintf(address, 128, "%02X:%02X:%02X:%02X:%02X:%02X",
        (unsigned char)result->ap_addr.sa_data[0],
        (unsigned char)result->ap_addr.sa_data[1],
        (unsigned char)result->ap_addr.sa_data[2],
        (unsigned char)result->ap_addr.sa_data[3],
        (unsigned char)result->ap_addr.sa_data[4],
        (unsigned char)result->ap_addr.sa_data[5]
      );
      printf(" MAC: %s", address);
    }

    // dBm Quality Value
    if (result->has_stats) {
      double dBm = 0;
      if (result->stats.qual.updated & IW_QUAL_DBM) {
        dBm = result->stats.qual.level;
        if (result->stats.qual.level >= 64) {
          dBm -= 0x100;
        }
      } else if (result->stats.qual.updated & IW_QUAL_RCPI) {
        dBm = (result->stats.qual.level / 2.0) - 110.0;
      }
      if (dBm) { printf(" Signal Strength: %0.f dBm", dBm); }
    }

    // Maxtransfer Bitrate
    if (result->has_maxbitrate) {
      printf(" Max Bitrate: %s", calculateSize(result->maxbitrate.value));
    }

    // Frequency
    if (result->b.has_freq) {
      int freq = (int)(result->b.freq / 1000000);
      int freq_chan = ((freq - 2412) / 5) + 1;
      printf(" Freq: %d (%d)", freq, freq_chan);
    }
    if (result->b.has_key) {
      printf(" Key: %s", result->b.key);
    }

    printf("\n");

    result = result->next;
  }

  return 1;
}
*/
