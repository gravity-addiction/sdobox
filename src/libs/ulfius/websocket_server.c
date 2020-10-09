#include <stdio.h>
#include <ulfius.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "libs/shared.h"
#include "static_file_callback.h"
#include "lib_websocket.h"
#include "websocket_server.h"
#include "libs/dbg/dbg.h"




// ------------------------
// Websocket Server Thread
// ------------------------
PI_THREAD (websocketServerThread)
{
  if (websocketServerThreadRunning) {
    dbgprintf(DBG_ERROR, "Not Starting Websocket Server Thread, Already Started\n");
    return NULL;
  }
  websocketServerThreadRunning = 1;

  if (websocketServerThreadKill) {
    dbgprintf(DBG_ERROR, "Not Starting Websocket Server Thread, Stop Flag Set\n");
    websocketServerThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_INFO, "Starting Websocket Server Thread\n");

  int ret;
  struct _u_instance instance;
  struct _static_file_config file_config;
  // char * cert_file = NULL, * key_file = NULL;

  dbgprintf(DBG_DEBUG, "Starting websocket server\n");

  file_config.mime_types = o_malloc(sizeof(struct _u_map));
  u_map_init(file_config.mime_types);
  u_map_put(file_config.mime_types, ".html", "text/html");
  u_map_put(file_config.mime_types, ".css", "text/css");
  u_map_put(file_config.mime_types, ".js", "application/javascript");
  u_map_put(file_config.mime_types, ".png", "image/png");
  u_map_put(file_config.mime_types, ".jpg", "image/jpeg");
  u_map_put(file_config.mime_types, ".jpeg", "image/jpeg");
  u_map_put(file_config.mime_types, ".ttf", "font/ttf");
  u_map_put(file_config.mime_types, ".woff", "font/woff");
  u_map_put(file_config.mime_types, ".woff2", "font/woff2");
  u_map_put(file_config.mime_types, ".map", "application/octet-stream");
  u_map_put(file_config.mime_types, "*", "application/octet-stream");
  file_config.files_path = WEBSOCKET_SERVER_STATIC_PATH;
  file_config.url_prefix = WEBSOCKET_SERVER_STATIC;
  file_config.map_header = o_malloc(sizeof(struct _u_map));
  u_map_init(file_config.map_header);

  if (ulfius_init_instance(&instance, WEBSOCKET_SERVER_PORT, NULL, NULL) != U_OK) {
    dbgprintf(DBG_ERROR, "Error Websocket Server, abort\n");
    return NULL;
  }

  u_map_put(instance.default_headers, "Access-Control-Allow-Origin", "*");

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", WEBSOCKET_SERVER_URL, NULL, 0, &websocket_cb, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", WEBSOCKET_SERVER_URL, "/echo", 0, &websocket_cbEcho, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", WEBSOCKET_SERVER_URL, "/file", 0, &websocket_cbFile, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", WEBSOCKET_SERVER_STATIC, "*", 0, &callback_static_file, &file_config);
/*
  // Start the framework
  if (argc > 3 && 0 == o_strcmp(argv[1], "-https")) {
    key_file = read_file(argv[2]);
    cert_file = read_file(argv[3]);
    if (key_file == NULL || cert_file == NULL) {
      printf("Error reading https certificate files\n");
      ret = U_ERROR_PARAMS;
    } else {
      ret = ulfius_start_secure_framework(&instance, key_file, cert_file);
    }
    o_free(key_file);
    o_free(cert_file);
  } else {
    ret = ulfius_start_framework(&instance);
  }
  */
  ret = ulfius_start_framework(&instance);

  if (ret == U_OK) {
    dbgprintf(DBG_INFO, "Start Websocket Server on port %d\n", instance.port);


    while(!websocketServerThreadKill) {
      usleep(100000);
    }

  } else {
    dbgprintf(DBG_ERROR, "Error starting Websocket Server\n");
  }
  dbgprintf(DBG_INFO, "End Websocket Server\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);
  u_map_clean_full(file_config.mime_types);
  u_map_clean_full(file_config.map_header);

  websocketServerThreadRunning = 0;
  return NULL;
}


int websocket_server_start() {
  dbgprintf(DBG_INFO, "%s\n", "websocketServerThreadStart()");
  if (websocketServerThreadRunning) { return 0; }

  websocketServerThreadKill = 0;
  return piThreadCreate(websocketServerThread);
}

void websocket_server_stop() {
  dbgprintf(DBG_INFO, "%s\n", "websocketServerThreadStop()");
  if (websocketServerThreadRunning && !websocketServerThreadKill) {
    websocketServerThreadKill = 1;
    int shutdown_cnt = 0;
    while (websocketServerThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    websocketServerThreadKill = 0;
  }
}
