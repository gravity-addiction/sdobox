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

  eventInfo->host = (char*)malloc(128 * sizeof(char));
  eventInfo->host[0] = '\0';

  eventInfo->local_folder = (char*)malloc(256 * sizeof(char));
  eventInfo->local_folder[0] = '\0';
  eventInfo->video_file = (char*)malloc(256 * sizeof(char));
  eventInfo->video_file[0] = '\0';
  eventInfo->url = (char*)malloc(512 * sizeof(char));
  eventInfo->url[0] = '\0';

  eventInfo->team = (char*)malloc(64 * sizeof(char));
  eventInfo->team[0] = '\0';
  eventInfo->rnd = (char*)malloc(64 * sizeof(char));
  eventInfo->rnd[0] = '\0';
  eventInfo->videoId = (char*)malloc(64 * sizeof(char));
  eventInfo->videoId[0] = '\0';
  eventInfo->eventId = (char*)malloc(64 * sizeof(char));
  eventInfo->eventId[0] = '\0';
  eventInfo->eventStr = (char*)malloc(128 * sizeof(char));
  eventInfo->eventStr[0] = '\0';
  eventInfo->compId = (char*)malloc(64 * sizeof(char));
  eventInfo->compId[0] = '\0';
  eventInfo->compStr = (char*)malloc(128 * sizeof(char));
  eventInfo->compStr[0] = '\0';
  eventInfo->es = (char*)malloc(128 * sizeof(char));
  eventInfo->es[0] = '\0';

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
  json_t *json_nb_sheep = ulfius_get_json_body_request(request, &error);
  if (json_is_object(json_nb_sheep) == 0) {
     printf("Error: %s\n", error.text);
  } else {

    // LIBULFIUS_SDOB_NEWVIDEO_DESTROY(libUlfiusSDOBNewVideoInfo);
    void *iter = json_object_iter(json_nb_sheep);
    while (iter) {
      
      char *key = (char*)json_object_iter_key(iter);
      json_t *value = json_object_iter_value(iter);
      char *val = (char*)json_string_value(value);

      if (strcmp(key, "host") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->host, val, 128); }
      else if (strcmp(key, "folder") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->local_folder, val, 256); }
      else if (strcmp(key, "file") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->video_file, val, 256); }
      else if (strcmp(key, "url") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->url, val, 512); }

      else if (strcmp(key, "team") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->team, val, 64); }
      else if (strcmp(key, "rnd") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->rnd, val, 64); }
      else if (strcmp(key, "videoId") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->videoId, val, 64); }
      else if (strcmp(key, "eventId") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->eventId, val, 64); }
      else if (strcmp(key, "slug") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->eventStr, val, 128); }
      else if (strcmp(key, "compId") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->compId, val, 64); }
      else if (strcmp(key, "comp") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->compStr, val, 128); }
      else if (strcmp(key, "es") == 0) { strlcpy(libUlfiusSDOBNewVideoInfo->es, val, 128); }

      json_decref(value);
      iter = json_object_iter_next(json_nb_sheep, iter);
    }

    libUlfiusSDOBNewVideoInfo->cnt++;
    json_decref(json_nb_sheep);
  }

  return U_CALLBACK_CONTINUE;
}

int callback_skydiveorbust_prestart (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 204, NULL);
  
  json_error_t error;
  json_t *json_prestart = ulfius_get_json_body_request(request, &error);
  if (json_is_object(json_prestart) == 0) {
    printf("Not Object Error: %s\n", error.text);
  } else {
    json_t *json_sopst = json_object_get(json_prestart, "sopst");
    json_t *json_pst = json_object_get(json_prestart, "pst");
    double jSopst = (double)json_number_value(json_sopst);
    double jPst = (double)json_number_value(json_pst);
    json_decref(json_sopst);
    json_decref(json_pst);

    pg_sdob_scorecard_score_sopst(&m_gui, jSopst, jPst);

    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->mark = E_SCORES_SOPST;
    item->time = jSopst;
    item->amt = jPst;
    
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    json_decref(json_prestart);
  }
  
  return U_CALLBACK_CONTINUE;
}

int callback_skydiveorbust_workingtime (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 204, NULL);
  
  json_error_t error;
  json_t *json_nb_sheep = ulfius_get_json_body_request(request, &error);
  if (json_nb_sheep == NULL || json_is_object(json_nb_sheep) != 0) {
    printf("Error: %s\n", error.text);
  } else {
    json_t *json_sowt = json_object_get(json_nb_sheep, "sowt");
    json_t *json_wt = json_object_get(json_nb_sheep, "wt");
    double jSowt = (double)json_number_value(json_sowt);
    double jWt = (double)json_number_value(json_wt);
    json_decref(json_sowt);
    json_decref(json_wt);

    pg_sdob_scorecard_score_sowt(&m_gui, jSowt, jWt);

    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->mark = E_SCORES_SOWT;
    item->time = jSowt;
    item->amt = jWt;
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    json_decref(json_nb_sheep);
  }

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
  printf("%s\n", "websocketApiThreadStart()");
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

  LIBULFIUS_SDOB_NEWVIDEO_DESTROY(libUlfiusSDOBNewVideoInfo);
  free(libUlfiusSDOBNewVideoInfo);
}
