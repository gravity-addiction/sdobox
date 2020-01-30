// System Headers
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h> // standard defs
#include <errno.h>
#include <libgen.h>
#include <syslog.h>
#include <limits.h>       //For PATH_MAX

#include <signal.h> // catching ctrl-c signal to quit
#include <pthread.h> // threading
#include <time.h> // system time clocks
#include <math.h> // millis()

#include <curl/curl.h> // curl
#include <wiringPi.h> // Gordons Wiring Pi
#include <libconfig.h> // Config files ~/.config/touchapp/touchapp.conf
#include <sqlite3.h> // SQLite3 Database



// Core Headers
#include "touchapp.h"
#include "shared.h"


// Lib Headers
#include "queue/queue.h"
#include "buttons/buttons.h"
#include "flightgroups/flightgroups.h"

#include "GUIslice-wrapper/GUIslice-wrapper.h"
// #include "gui/wifi/wifi.h"
// #include "gui/wifi/wifi_wpa.h"




// SIGINT handler
void signal_sigint(int sig) { // can be called asynchronously
  // printf("SigInt Read! %d\n", m_bQuit);
  if (m_bQuit > 1) {
    // debug_print("\n%s\n", "fuck it, exit now!");
    exit(1);
  } else if (m_bQuit > 0) {
    // debug_print("\n%s\n", "Alright, We'll fast track this shut down!");
    ++m_bQuit;
  } else {
    // debug_print("\n\n%s\n", "Hold on there fella, we gotta shut some shit down first!");
    m_bSigInt = sig;
    m_bQuit = 1;
  }
}





// ------------------------
// Application Configuration
// ------------------------
// Fetch Settings from config_path set in touchapp.h
// default is ~/.config/touchapp/touchapp.conf
void get_config_settings()
{
  config_t cfg;
  int retInt;

  // const char *retStr;

  config_init(&cfg);

  // Read the file. If there is an error, report it and exit.
  if(access(config_path, F_OK) == -1 || !config_read_file(&cfg, config_path))
  {
    // debug_print("Cannot Find config_path: %s\n", config_path);
    config_destroy(&cfg);
    return;
  }

  // rotary_pin_a (ROTARY_PIN_A)
  if (config_lookup_int(&cfg, "rotary_pin_a", &retInt)) {
    ROTARY_PIN_A = retInt;
  }
  // rotary_pin_b (ROTARY_PIN_B)
  if (config_lookup_int(&cfg, "rotary_pin_b", &retInt)) {
    ROTARY_PIN_B = retInt;
  }
  // rotary_pin_btn (ROTARY_PIN_BTN)
  if (config_lookup_int(&cfg, "rotary_pin_btn", &retInt)) {
    ROTARY_PIN_BTN = retInt;
  }
  // right_pin_btn (RIGHT_PIN_BTN)
  if (config_lookup_int(&cfg, "right_pin_btn", &retInt)) {
    RIGHT_PIN_BTN = retInt;
  }
  // left_pin_btn (LEFT_PIN_BUTTON)
  if (config_lookup_int(&cfg, "left_pin_btn", &retInt)) {
    LEFT_PIN_BTN = retInt;
  }
  // debounce_delay (lib_buttonsBtnDebounceDelay)
  if (config_lookup_int(&cfg, "debounce_delay", &retInt)) {
    lib_buttonsBtnDebounceDelay = retInt;
  } else {
    lib_buttonsBtnDebounceDelay = 175;
  }
  // btn_hold_delay (lib_buttonsBtnHoldDelay)
  if (config_lookup_int(&cfg, "btn_hold_delay", &retInt)) {
    lib_buttonsBtnHoldDelay = retInt;
  } else {
    lib_buttonsBtnHoldDelay = 750;
  }

  config_destroy(&cfg);
}


int main( int argc, char* args[] )
{
  // directory of application for a start path
  // char* strPath = dirname(args[0]);

  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  // Main Quitter flag, 1 quits, 2 hurrys up quitting, and 3 quits rtfn
  m_bQuit = 0;

  // Touchapp Environment
  get_config_settings();
  // Syslog Mask
  setlogmask (LOG_UPTO (LOG_NOTICE));

  // Register Signals
  signal(SIGINT, signal_sigint);
  // signal(SIGTERM, signal_sigint);


  // ------------------------------------------------
  // Initialize workers
  // ------------------------------------------------
  // libs/buttons
  lib_buttons_init(&m_gui);

  // Start Buttons
  // lib_buttonsThreadStart();


  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  guislice_wrapper_init(&m_gui);

  // InitGUI_Keyboard(&m_gui, strPath);
  // InitGUI_Wifi(&m_gui, strPath);


  ///// TEST STUFF /////
  // wifi_scan("wlan0");

  // if (piThreadCreate(pg_wifi_wpaEventThread) != 0) {
  //   printf("WPA Event Thread Did Not Start\n");
  // }

  // pg_wifi_wpaSendCmd("SCAN");
  // struct list_head *results;
  // wifi_scan(pg_wifi_wpa_conn, 0);
  // // debug_print("Results %s\n", results);
  // wpaScan();
  // wifi_init("wlan0");
  // Load Video
  // mpv_loadfile("/home/pi/Videos/tunnel_20191204_730pm_40358_1.mp4", "replace", "fullscreen=yes");
  // mpv_loadfile("/home/pi/Videos/Group1-12_2.mp4", "replace", "fullscreen=yes");
  // mpv_create_player("/home/pi/Videos/Group5-34_2.mp4");
  // int videoid = db_video_info_insert(1, "22221", "vfs,problems", 60, "About this Time last week");
  // // debug_print("Video: %d\n", videoid);

  // db_video_info_search();
  // json_test();
  // fileinfo_create("/home/pi/Videos/18US_CF4S_R3_3203.mp4");

  // openlog ("touchapp", LOG_NDELAY | LOG_PID, LOG_LOCAL1);
  // syslog (LOG_NOTICE, "SUBMISSION/MEET2019 4143,1,JR,0,01/26/2019 12:49:01,Group14-12_2.mp4,403.600000,148.866667,/,/,/,/,O,/,/,/,/,/,O,/,/,/,/,O,/,/,/,/,/,/,/,/");
  // closelog();

  /*
  size_t fileLen = strlen("/home/pi/Videos/SVNH2019/flightgroups.json") + 1;
  char *filename = malloc(fileLen);
  strlcpy(filename, "/home/pi/Videos/SVNH2019/flightgroups.json", fileLen);
  xml_flightgroups_parseFile(filename);
  */
  ///// TEST STUFF /////



  // mainButtonSetFuncs();
  // InitGUI_AdvertGui(strPath);
  touchscreenPageOpen(&m_gui, E_PG_MAIN);

  // ------------------------------------------------
  // Main event loop
  // ------------------------------------------------
  int m_bSleep = 1;
  while (!m_bQuit) {
    if (cbThread[m_page_current] &&
        cbThread[m_page_current](&m_gui)
    ) {
      m_bSleep = 0;
    }

    if (lib_buttons_thread()) {
       m_bSleep = 0;
    }

    if (m_bSleep) {
      usleep(1000);
    } else {
      m_bSleep = 1; // reset flag to next loop
    }

  gslc_Update(&m_gui);
  } // bQuit

  // debug_print("%s\n", "Shutting Down!");


  // Quit GUIslice
  guislice_wrapper_quit(&m_gui);

  // Close all Pages
  touchScreenPageDestroyAll(&m_gui);

  // Shutdown Buttons Thread
  // lib_buttonsThreadStop();

  // Kill any outstanding fbcp instances
  // fbcp_stop();

  printf("%s\n", "Controls are yours.");
  usleep(250000);
  return m_bSigInt;
}


