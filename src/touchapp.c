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

#include <curl/curl.h> // curl
#include <wiringPi.h> // Gordons Wiring Pi, millis()
#include <libconfig.h> // Config files /home/pi/.config/sdobox/sdobox.conf




// Core Headers
#include "libs/shared.h"


// Lib Headers
#include "libs/queue/queue.h"
#include "libs/backlight/backlight.h"
#include "libs/buttons/buttons.h"
#include "libs/fbcp/fbcp.h"
#include "libs/mpv-zmq/mpv-zmq.h"
#include "libs/sdob-zmq/sdob-zmq.h"
// #include "libs/sqlite3-wrapper/sqlite3-wrapper.h"
// #include "libs/ulfius/websocket_api.h"
// #include "libs/ulfius/websocket_server.h"
// #include "libs/ulfius/websocket_client.h"
#include "libs/dbg/dbg.h"
#include "libs/GUIslice-wrapper/GUIslice-wrapper.h"
#include "libs/fbbg/fbbg.h"
#include "libs/avahi/avahi.h"
#include <mpv/client.h>

#include "libs/mpv-zmq/mpv_zeromq.h"
#include "libs/mpv-wrapper/mpv-wrapper.h"
// #include "libs/sdob-socket/sdob-socket.h"
// #include "libs/json/json.h" // JSON Parsing
// #include "libs/usb-drives/usb-drives-thread.h"
#include "libs/zhelpers/zhelpers-conn.h"
#include "libs/zhelpers/zhelpers-tx.h"

#include "gui/skydiveorbust/skydiveorbust.h"
// #include "gui/wifi/wifi.h"
// #include "gui/wifi/wifi_wpa.h"

int      m_bSigInt = 0; // placeholder for returning from app same sigint value that the app received
int      m_startPage = 0; // Page Int to start, -1 for no page just buttons
int      m_touchscreenInit = 0; // guislice init successful

void signal_sigint(int sig);
void get_config_settings();
int main( int argc, char* args[] );


// SIGINT handler
void signal_sigint(int sig) { // can be called asynchronously
  dbgprintf(DBG_DEBUG, "SIGINT: %d current m_bQuit value = %d\n", sig, m_bQuit);
  if (m_bQuit > 1) {
    dbgprintf(DBG_DEBUG, "Forcibly exiting now!\n");
    _exit(1);                   /* do NOT call atexit functions */
  } else if (m_bQuit > 0) {
    dbgprintf(DBG_DEBUG, "Turning up the shutdown heat...\n");
    ++m_bQuit;
  } else {
    dbgprintf(DBG_DEBUG, "Requesting shut-down...\n");
    m_bSigInt = sig;
    m_bQuit = 1;
  }
}





// ------------------------
// Application Configuration
// ------------------------
// Fetch Settings from config_path set in touchapp.h
// default is /home/pi/.config/sdobox/sdobox.conf
void get_config_settings()
{
  config_t cfg;
  int retInt;

  // const char *retStr;

  config_init(&cfg);

  // Read the file. If there is an error, report it and exit.
  if (access(config_path, F_OK) == -1 || !config_read_file(&cfg, config_path)) {
    dbgprintf(DBG_DEBUG, "Cannot Find config_path: %s\n", config_path);
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
/*
  // sqlite3 (SQLITE3_PATH)
  const char * retSQL3Path;
  if (config_lookup_string(&cfg, "sqlite3", &retSQL3Path)) {
    SQLITE3_PATH = strdup(retSQL3Path);
  }
*/
  // start page
  if (config_lookup_int(&cfg, "start_page", &retInt)) {
    m_startPage = retInt;
  }
  // Sanity check page int
  if (m_startPage < -1 || m_startPage >= MAX_PAGES) {
    m_startPage = 0;
  }
/*
  // websocket_api_port (WEBSOCKET_API_PORT)
  if (config_lookup_int(&cfg, "websocket_api_port", &retInt)) {
    WEBSOCKET_API_PORT = retInt;
  } else {
    WEBSOCKET_API_PORT = 4004;
  }

  if (config_lookup_int(&cfg, "websocket_server_port", &retInt)) {
    WEBSOCKET_SERVER_PORT = retInt;
  }
  const char * retWebsocketUrl;
  if (config_lookup_string(&cfg, "websocket_server_url", &retWebsocketUrl)) {
    WEBSOCKET_SERVER_URL = strdup(retWebsocketUrl);
  }
  const char * retWebsocketStaticPath;
  if (config_lookup_string(&cfg, "websocket_server_static_path", &retWebsocketStaticPath)) {
    WEBSOCKET_SERVER_STATIC_PATH = strdup(retWebsocketStaticPath);
  }
  const char * retWebsocketStatic;
  if (config_lookup_string(&cfg, "websocket_server_static", &retWebsocketStatic)) {
    WEBSOCKET_SERVER_STATIC = strdup(retWebsocketStatic);
  }
  const char * retWebsocketHost;
  if (config_lookup_string(&cfg, "websocket_client_host", &retWebsocketHost)) {
    WEBSOCKET_CLIENT_HOST = strdup(retWebsocketHost);
  }
*/
  const char * retVideosPath;
  if (config_lookup_string(&cfg, "videos_path", &retVideosPath)) {
    VIDEOS_BASEPATH = strdup(retVideosPath);
  } else if (config_lookup_string(&cfg, "video_path", &retVideosPath)) {
    VIDEOS_BASEPATH = strdup(retVideosPath);
  } else {
    VIDEOS_BASEPATH = strdup("/home/pi/Videos");
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

  // Debug printing support
  init_dbg();

  // Syslog Mask
  setlogmask (LOG_UPTO (LOG_NOTICE));

  // Register Signals
  signal(SIGINT, signal_sigint);
  signal(SIGTERM, signal_sigint);

  // For running as a daemon
  if (sigignore(SIGHUP)) {
    dbgprintf(DBG_ERROR, "attempt to ignore SIGHUP failed: %s\n", strerror(errno));
    abort();
  }

  config_path = "/home/pi/.config/sdobox/sdobox.conf";

  // ------------------------------------------------
  // Check For Touchscreen Calibration
  // ------------------------------------------------
  // Tslib pointercal
  if (access("/etc/pointercal", R_OK) == -1 && access("/usr/local/etc/pointercal", R_OK) == -1) {
    dbgprintf(DBG_ERROR, "No Touchscreen Calibration!\n");
    system("/opt/sdobox/scripts/calibrate");
  }


  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  m_touchscreenInit = guislice_wrapper_init(&m_gui);


  // ------------------------------------------------
  // Initialize workers
  // ------------------------------------------------

  // Touchapp config file
  if (access(config_path, R_OK) == -1) {
    dbgprintf(DBG_ERROR, "No Config File!: %s\n", config_path);

    touchscreenPageOpen(&m_gui, E_PG_CONFIGURE);
/*
    if (lib_buttons_configure(config_path) == 0) {
      gslc_Quit(&m_gui);
      return 0;
    }
*/
  }

  // Touchapp Environment
  get_config_settings();

  // libs/buttons
  lib_buttons_init();

  // Start Buttons
  // lib_buttonsThreadStart();

  // if (SQLITE3_PATH != NULL) {
  //   sqlite3_wrapper_init();
  //   sqlite3_wrapper_start();
  // }

  // if (WEBSOCKET_API_PORT) {
  //   websocket_api_start();
  // }
  // if (WEBSOCKET_SERVER_PORT && WEBSOCKET_SERVER_URL) {
  //   websocket_server_start();
  // }

  // Initialize ZeroMQ Context
  libzhelpers_conn_init();

  // Initialize MPV library
  libsdob_zmq_init();

  libmpv_wrapper_init();
  // libmpv_zmq_init();
  mpv_socket_init();
  
  libMpvSocketThreadStart();
  libMpvQueueThreadStart();

  // libSdobSocketThreadStart();

  // libUsbDrivesThreadStart();
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

  // Start FBBG Display 0, Layer 0
  // fbbg_start();
  // clock_setLocaltime();
  // xdotool_wrapper_wakeUp();
  ///// TEST STUFF /////
 

  // mainButtonSetFuncs();
  // InitGUI_AdvertGui(strPath);
  if (m_touchscreenInit) {
    // Setup TSLib Calibration
    system("DISPLAY=:0.0 /opt/sdobox/scripts/xinput/set &");
    gslc_SetTouchDisabled(&m_gui, false);
    gslc_SetScreenDisableRedraw(&m_gui, false);
    gslc_PageRedrawSet(&m_gui, true);
    // system("DISPLAY=:0.0 xinput set-prop 'ADS7846 Touchscreen' 'Device Enabled' 0 &");
  }

  // Start Page, -1 for no page, buttons only
  if (m_startPage > -1) {
    system("/opt/sdobox/scripts/xinput/disable &");
    touchscreenPageOpen(&m_gui, m_startPage);
  } else {
    system("/opt/sdobox/scripts/xinput/enable &");
    fbcp_start();
  }

  // Register The Device
  // system("/opt/sdobox/scripts/register_device.sh &");

  // ------------------------------------------------
  // Main event loop
  // ------------------------------------------------
  int m_bSleep = 1; // true to usleep when main loop has nothing left to do
  int m_tPageCur = touchscreenPageStackCur();




/*
  // ZeroMQ Context for entire process, including threads
  void *context = zmq_ctx_new ();
  pthread_t mpv_tid, timer_tid, writer_tid;
  int rc;

  // Open :5555 for publishing
  quadfive = zmq_socket(context, ZMQ_PUB);
  rc = zmq_bind(quadfive, "inproc://videoserver"); // "tcp://*:5555");
  assert (rc == 0);

  // Open :5559 for one-way requests
  void *command_raw = zmq_socket (context, ZMQ_PULL);
  rc = zmq_bind (command_raw, "inproc://cmdserver"); // "tcp://*:5559");
  assert (rc == 0);


  // Create MPV Player Handle
  mpv_handle *mpvHandle = mpv_handle_init();

  // Assign pointers for multi-threads
  struct threadArgs threadpassArgs;
  threadpassArgs.mpvHandle = mpvHandle;
  threadpassArgs.quadfive = quadfive;
  threadpassArgs.quadfiveLock = quadfiveLock;
  threadpassArgs.context = context;
  threadpassArgs.bCancel = &m_bQuit;

  // Start Multithreads
  pthread_create(&mpv_tid, NULL, mpvZeroMQThread, (void *)&threadpassArgs);
  pthread_create(&timer_tid, NULL, mpvTimerThread, (void *)&threadpassArgs);
  pthread_create(&writer_tid, NULL, mpvWriterThread, (void *)&threadpassArgs);
*/
  // avahi_main();
  gslc_Update(&m_gui);
  while (!m_bQuit) {
    if (cbThread[m_tPageCur] &&
        cbThread[m_tPageCur](&m_gui) == 0
    ) {
      m_bSleep = 0;
    }
/*
    char msg[4096];
    int size = zmq_recv(command_raw, msg, 4096 - 1, ZMQ_DONTWAIT);
    if (size > 0) {
      msg[size < 4096 ? size : 4096 - 1] = '\0';
      char* retData = NULL;
      int rcCmd = libmpv2_parse_msg(mpvHandle, msg, 1, &retData);
      printf("Raw Send: %d - %s\n%s\n", rcCmd, msg, retData);
    }
*/
    // if (lib_buttons_thread()) {
    //    m_bSleep = 0;
    // }

    if (m_bSleep) {
      usleep(50000);
    } else {
      m_bSleep = 1; // reset flag to next loop
    }

    if (m_touchscreenInit) {
      gslc_Update(&m_gui);
    }

    m_tPageCur = touchscreenPageStackCur();
  } // bQuit

  dbgprintf(DBG_DEBUG, "%s\n", "Shutting Down!");
/*
  zmq_close (command_raw);
  zmq_close (quadfive);
  mpv_handle_destroy(mpvHandle);

  pthread_join(mpv_tid, NULL); 
  pthread_join(timer_tid, NULL);
  pthread_join(writer_tid, NULL);

  zmq_ctx_destroy (context);
*/
  if (m_touchscreenInit) {
    // Quit GUIslice
    guislice_wrapper_quit(&m_gui);

    // Close all Pages
    touchScreenPageDestroyAll(&m_gui);
  }

  // if (WEBSOCKET_SERVER_PORT && WEBSOCKET_SERVER_URL) {
  //   websocket_server_stop();
  // }

  // if (WEBSOCKET_API_PORT) {
  //   websocket_api_stop();
  // }

  // if (SQLITE3_PATH != NULL) {
  //   sqlite3_wrapper_stop();
  // }

  // Kill MPV
  // mpv_stop();
  // libMpvSocketThreadStop();

  // Kill SDOB Socket
  // libSdobSocketThreadStop();

  // Kill USB Drives Thread
  // libUsbDrivesThreadStop();
  printf("%s\n", "Cleanup Threads");
  
  // Shutdown Buttons Thread
  // lib_buttonsThreadStop();
  libmpv_zmq_destroy();
  libsdob_zmq_destroy();
  
  printf("%s\n", "Relax Mirroring");
  // Kill any outstanding fbcp instances
  fbcp_stop();
  // Kill any outstanding fbbg instances
  fbbg_stop();
  
  // printf("%s\n", "Disconnecting ZMQ.");
  // pthread_exit(0);
  // libzhelpers_conn_destroy();

  printf("%s\n", "Controls are yours.");
  gslc_Quit(&m_gui);
  usleep(250000);
  return m_bSigInt;
}


