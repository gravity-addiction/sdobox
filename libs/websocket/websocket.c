#include <stdio.h>
#include <ulfius.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "shared.h"
#include "websocket.h"
#include "dbg/dbg.h"

int callback_hello_world (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 200, "Hello World!");
  return U_CALLBACK_CONTINUE;
}


void setEndpoints(struct _u_instance instance) {}
  ulfius_add_endpoint_by_val(&instance, "GET", "/helloworld", NULL, 0, &callback_hello_world, NULL);
}


// ------------------------
// SQLite Wrapper Thread
// ------------------------
PI_THREAD (websocketThread)
{
  if (websocketThreadRunning) {
    dbgprintf(DBG_ERROR, "Not Starting sqlite3 Thread, Already Started\n");
    return NULL;
  }
  websocketThreadRunning = 1;

  if (websocketThreadKill) {
    dbgprintf(DBG_ERROR, "Not Starting sqlite3 Thread, Stop Flag Set\n");
    websocketThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_INFO, "Starting SQLite Wrapper Thread\n");
  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, WEBSOCKET_PORT, NULL, NULL) != U_OK) {
    dbgprintf(DBG_ERROR, "Error ulfius_init_instance, abort\n");
    return NULL;
  }

  // Endpoint list declaration
  setEndpoints(instance);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    dbgprintf(DBG_INFO, "Start websocket on port %d\n", instance.port);
    while(websocketThreadRunning) {
      usleep(100000);
    }
  } else {
    dbgprintf(DBG_ERROR, "Error starting websocket\n");
  }

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  websocketThreadRunning = 0;
  return NULL;
}

int websocket_start() {
  dbgprintf(DBG_INFO, "%s\n", "websocketThreadStart()");
  if (websocketThreadRunning) { return 0; }

  websocketThreadKill = 0;
  return piThreadCreate(websocketThread);
}

void websocket_stop() {
  dbgprintf(DBG_INFO, "%s\n", "websocketThreadStop()");
  if (websocketThreadRunning && !websocketThreadKill) {
    websocketThreadKill = 1;
    int shutdown_cnt = 0;
    while (websocketThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    websocketThreadKill = 0;
  }
}
