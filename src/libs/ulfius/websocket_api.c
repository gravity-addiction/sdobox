#include <stdio.h>
#include <ulfius.h>
#include <wiringPi.h> // Gordons Wiring Pi
#include <jansson.h>

#include "libs/shared.h"
#include "lib_websocket.h"
#include "websocket_api.h"
#include "libs/dbg/dbg.h"


struct libUlfiusSDOBNewVideo * LIBULFIUS_SDOB_NEWVIDEO() {
  struct libUlfiusSDOBNewVideo *eventInfo = (struct libUlfiusSDOBNewVideo*)malloc(sizeof(struct libUlfiusSDOBNewVideo));
  eventInfo->cnt = 0;
  // eventInfo->team = strdup((char*)' ');
  // eventInfo->rnd = strdup((char*)' ');
  // eventInfo->meet = strdup((char*)' ');
  // eventInfo->desc = strdup((char*)' ');
  return eventInfo;
}

int callback_hello_world (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 200, "Hello World!");
  return U_CALLBACK_CONTINUE;
}

int callback_spotify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 200, "Confirmed!");

  printf("State: %s\n", u_map_get(request->map_url, "state"));
  const char* code = u_map_get(request->map_url, "code");
  printf("Code: %s\n", code);

  char *fullpath = malloc(strlen("/opt/sdobox/scripts/spotify/register_device_code.sh ") + strlen(code) + 1);
  sprintf(fullpath, "/opt/sdobox/scripts/spotify/register_device_code.sh %s", code);
  system(fullpath);
  free(fullpath);

  return U_CALLBACK_CONTINUE;
}

int callback_skydiveorbust_newvideo (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 204, NULL);
  json_error_t error;
  json_t * json_nb_sheep = ulfius_get_json_body_request(request, &error);
  if (json_nb_sheep == NULL) {
    printf("Error: %s\n", error.text);
  } else {
    libUlfiusSDOBNewVideoInfo->team = strdup(json_string_value(json_object_get(json_nb_sheep, "team")));
    libUlfiusSDOBNewVideoInfo->rnd = strdup(json_string_value(json_object_get(json_nb_sheep, "rnd")));
    libUlfiusSDOBNewVideoInfo->meet = strdup(json_string_value(json_object_get(json_nb_sheep, "meet")));
    libUlfiusSDOBNewVideoInfo->desc = strdup(json_string_value(json_object_get(json_nb_sheep, "desc")));
  }
  free(json_nb_sheep);

  libUlfiusSDOBNewVideoInfo->cnt++;
  return U_CALLBACK_CONTINUE;
}

// ------------------------
// Websocket API Thread
// ------------------------
PI_THREAD (websocketApiThread)
{
  if (websocketApiThreadRunning) {
    dbgprintf(DBG_ERROR, "Not Starting Websocket API Thread, Already Started\n");
    return NULL;
  }
  websocketApiThreadRunning = 1;

  if (websocketApiThreadKill) {
    dbgprintf(DBG_ERROR, "Not Starting Websocket API Thread, Stop Flag Set\n");
    websocketApiThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_INFO, "Starting Websocket API Thread\n");
  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, WEBSOCKET_API_PORT, NULL, NULL) != U_OK) {
    dbgprintf(DBG_ERROR, "Error ulfius_init_instance, abort\n");
    return NULL;
  }

  // Add Headers
  u_map_put(instance.default_headers, "Access-Control-Allow-Origin", "*");

  ulfius_add_endpoint_by_val(&instance, "GET", "/H3xx92sk", NULL, 0, &callback_spotify, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", "/p/skydiveorbust/newvideo", NULL, 0, &callback_skydiveorbust_newvideo, NULL);
  ulfius_set_default_endpoint(&instance, &websocket_cbDefault, NULL);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    dbgprintf(DBG_INFO, "Start Websocket API on port %d\n", instance.port);
    while(!websocketApiThreadKill) {
      usleep(100000);
    }
  } else {
    dbgprintf(DBG_ERROR, "Error starting Websocket API\n");
  }

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  websocketApiThreadRunning = 0;
  return NULL;
}



int websocket_api_start() {
  dbgprintf(DBG_INFO, "%s\n", "websocketApiThreadStart()");
  if (websocketApiThreadRunning) { return 0; }

  libUlfiusSDOBNewVideoInfo = LIBULFIUS_SDOB_NEWVIDEO();

  websocketApiThreadKill = 0;
  return piThreadCreate(websocketApiThread);
}

void websocket_api_stop() {
  dbgprintf(DBG_INFO, "%s\n", "websocketApiThreadStop()");
  if (websocketApiThreadRunning && !websocketApiThreadKill) {
    websocketApiThreadKill = 1;
    int shutdown_cnt = 0;
    while (websocketApiThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    websocketApiThreadKill = 0;
  }
}
