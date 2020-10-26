#include <stdio.h>
#include <ulfius.h>
#include <wiringPi.h> // Gordons Wiring Pi
#include <jansson.h>

#include "libs/shared.h"
#include "lib_websocket.h"
#include "websocket_api.h"
#include "libs/dbg/dbg.h"
#include "libs/queue/queue.h"

#include "gui/skydiveorbust/skydiveorbust.h"


struct libUlfiusSDOBNewVideo * LIBULFIUS_SDOB_NEWVIDEO() {
  struct libUlfiusSDOBNewVideo *eventInfo = (struct libUlfiusSDOBNewVideo*)malloc(sizeof(struct libUlfiusSDOBNewVideo));
  eventInfo->cnt = 0;

  eventInfo->host = (char*)malloc(2 * sizeof(char));

  eventInfo->local_folder = (char*)malloc(2 * sizeof(char));
  eventInfo->video_file = (char*)malloc(2 * sizeof(char));
  eventInfo->url = (char*)malloc(2 * sizeof(char));

  eventInfo->team = (char*)malloc(2 * sizeof(char));
  eventInfo->rnd = (char*)malloc(2 * sizeof(char));
  eventInfo->videoId = (char*)malloc(2 * sizeof(char));
  eventInfo->eventId = (char*)malloc(2 * sizeof(char));
  eventInfo->eventStr = (char*)malloc(2 * sizeof(char));
  eventInfo->compId = (char*)malloc(2 * sizeof(char));
  eventInfo->compStr = (char*)malloc(2 * sizeof(char));
  eventInfo->es = (char*)malloc(2 * sizeof(char));

  return eventInfo;
}

void LIBULFIUS_SDOB_NEWVIDEO_DESTROY(struct libUlfiusSDOBNewVideo *eventInfo) {
  free(eventInfo->host);

  free(eventInfo->local_folder);
  free(eventInfo->video_file);
  free(eventInfo->url);

  free(eventInfo->team);
  free(eventInfo->rnd);
  free(eventInfo->videoId);
  free(eventInfo->eventId);
  free(eventInfo->eventStr);
  free(eventInfo->compId);
  free(eventInfo->compStr);
  free(eventInfo->es);
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
    // LIBULFIUS_SDOB_NEWVIDEO_DESTROY(libUlfiusSDOBNewVideoInfo);
    libUlfiusSDOBNewVideoInfo->host = strdup(json_string_value(json_object_get(json_nb_sheep, "host")));

    libUlfiusSDOBNewVideoInfo->local_folder = strdup(json_string_value(json_object_get(json_nb_sheep, "folder")));
    libUlfiusSDOBNewVideoInfo->video_file = strdup(json_string_value(json_object_get(json_nb_sheep, "file")));
    libUlfiusSDOBNewVideoInfo->url = strdup(json_string_value(json_object_get(json_nb_sheep, "url")));

    libUlfiusSDOBNewVideoInfo->team = strdup(json_string_value(json_object_get(json_nb_sheep, "team")));
    libUlfiusSDOBNewVideoInfo->rnd = strdup(json_string_value(json_object_get(json_nb_sheep, "rnd")));
    libUlfiusSDOBNewVideoInfo->videoId = strdup(json_string_value(json_object_get(json_nb_sheep, "videoId")));
    libUlfiusSDOBNewVideoInfo->eventId = strdup(json_string_value(json_object_get(json_nb_sheep, "eventId")));
    libUlfiusSDOBNewVideoInfo->eventStr = strdup(json_string_value(json_object_get(json_nb_sheep, "slug")));
    libUlfiusSDOBNewVideoInfo->compId = strdup(json_string_value(json_object_get(json_nb_sheep, "compId")));
    libUlfiusSDOBNewVideoInfo->compStr = strdup(json_string_value(json_object_get(json_nb_sheep, "comp")));
    libUlfiusSDOBNewVideoInfo->es = strdup(json_string_value(json_object_get(json_nb_sheep, "es")));
  }
  free(json_nb_sheep);

  libUlfiusSDOBNewVideoInfo->cnt++;
  return U_CALLBACK_CONTINUE;
}

int callback_skydiveorbust_prestart (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 204, NULL);

  json_error_t error;
  json_t * json_nb_sheep = ulfius_get_json_body_request(request, &error);
  if (json_nb_sheep == NULL) {
    printf("Error: %s\n", error.text);
  } else {
    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->mark = E_SCORES_SOPST;
    item->time = json_real_value(json_object_get(json_nb_sheep, "sopst"));
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    // pg_sdob_scorecard_score_sopst(&m_gui, json_real_value(json_object_get(json_nb_sheep, "sopst")), json_real_value(json_object_get(json_nb_sheep, "pst")));
  }
  free(json_nb_sheep);

  return U_CALLBACK_CONTINUE;
}

int callback_skydiveorbust_workingtime (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 204, NULL);

  json_error_t error;
  json_t * json_nb_sheep = ulfius_get_json_body_request(request, &error);
  if (json_nb_sheep == NULL) {
    printf("Error: %s\n", error.text);
  } else {
    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->mark = E_SCORES_SOWT;
    item->time = json_real_value(json_object_get(json_nb_sheep, "sopst"));
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    // pg_sdob_scorecard_score_sowt(&m_gui, json_real_value(json_object_get(json_nb_sheep, "sowt")), json_real_value(json_object_get(json_nb_sheep, "wt")));
  }
  free(json_nb_sheep);
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
  ulfius_add_endpoint_by_val(&instance, "PUT", "/p/skydiveorbust/prestart", NULL, 0, &callback_skydiveorbust_prestart, NULL);
  ulfius_add_endpoint_by_val(&instance, "PUT", "/p/skydiveorbust/workingtime", NULL, 0, &callback_skydiveorbust_workingtime, NULL);
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

  // LIBULFIUS_SDOB_NEWVIDEO_DESTROY(libUlfiusSDOBNewVideoInfo);
  // free(libUlfiusSDOBNewVideoInfo);
}
