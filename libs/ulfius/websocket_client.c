#include <stdio.h>
#include <ulfius.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "libs/shared.h"
#include "lib_websocket.h"
#include "websocket_client.h"
#include "libs/dbg/dbg.h"


// ------------------------
// Websocket Client Thread
// ------------------------
PI_THREAD (websocketClientThread)
{
  if (websocketClientThreadRunning) {
    dbgprintf(DBG_ERROR, "Not Starting Websocket Client Thread, Already Started\n");
    return NULL;
  }
  websocketClientThreadRunning = 1;

  if (websocketClientThreadKill) {
    dbgprintf(DBG_ERROR, "Not Starting Websocket Client Thread, Stop Flag Set\n");
    websocketClientThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_INFO, "Starting Websocket Client Thread\n");
    struct _u_request request;
  struct _u_response response;
  struct _websocket_client_handler websocket_client_handler;
  char * websocket_user_data = o_strdup("my user data");
//  char * url = (argc>1&&0==o_strcmp("-https", argv[1]))?"wss://localhost:" PORT PREFIX_WEBSOCKET:"ws://localhost:" PORT PREFIX_WEBSOCKET;

  ulfius_init_request(&request);
  ulfius_init_response(&response);
  if (ulfius_set_websocket_request(&request, WEBSOCKET_CLIENT_HOST, "protocol", "extension") == U_OK) {
    request.check_server_certificate = 0;
    if (ulfius_open_websocket_client_connection(&request, &websocket_manager_callback, websocket_user_data, &websocket_incoming_message_callback, websocket_user_data, &websocket_onclose_callback, websocket_user_data, &websocket_client_handler, &response) == U_OK) {

      while(!websocketClientThreadKill) {
        usleep(100000);
      }

      ulfius_websocket_client_connection_close(&websocket_client_handler);
      dbgprintf(DBG_DEBUG, "Websocket Client closed");
    } else {
      dbgprintf(DBG_ERROR, "Error ulfius_open_websocket_client_connection");
      o_free(websocket_user_data);
    }
  } else {
    dbgprintf(DBG_ERROR, "Error ulfius_set_websocket_request");
    o_free(websocket_user_data);
  }

  ulfius_clean_request(&request);
  ulfius_clean_response(&response);

  websocketClientThreadRunning = 0;
  return NULL;
}



int websocket_client_start() {
  dbgprintf(DBG_INFO, "%s\n", "websocketClientThreadStart()");
  if (websocketClientThreadRunning) { return 0; }

  websocketClientThreadKill = 0;
  return piThreadCreate(websocketClientThread);
}

void websocket_client_stop() {
  dbgprintf(DBG_INFO, "%s\n", "websocketClientThreadStop()");
  if (websocketClientThreadRunning && !websocketClientThreadKill) {
    websocketClientThreadKill = 1;
    int shutdown_cnt = 0;
    while (websocketClientThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    websocketClientThreadKill = 0;
  }
}
