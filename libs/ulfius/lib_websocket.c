#include <string.h>
#include <ulfius.h>
#include "dbg/dbg.h"

/**
 * decode a u_map into a string
 */
char * print_map(const struct _u_map * map) {
  char * line, * to_return = NULL;
  const char **keys, * value;
  int len, i;
  if (map != NULL) {
    keys = u_map_enum_keys(map);
    for (i=0; keys[i] != NULL; i++) {
      value = u_map_get(map, keys[i]);
      len = snprintf(NULL, 0, "key is %s, value is %s", keys[i], value);
      line = o_malloc((len+1)*sizeof(char));
      snprintf(line, (len+1), "key is %s, value is %s", keys[i], value);
      if (to_return != NULL) {
        len = o_strlen(to_return) + o_strlen(line) + 1;
        to_return = o_realloc(to_return, (len+1)*sizeof(char));
        if (o_strlen(to_return) > 0) {
          strcat(to_return, "\n");
        }
      } else {
        to_return = o_malloc((o_strlen(line) + 1)*sizeof(char));
        to_return[0] = 0;
      }
      strcat(to_return, line);
      o_free(line);
    }
    return to_return;
  } else {
    return NULL;
  }
}
/*
// Readfile to buffer to streaming
static char * read_file(const char * filename) {
  char * buffer = NULL;
  long length;
  FILE * f;
  if (filename != NULL) {
    f = fopen (filename, "rb");
    if (f) {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = o_malloc (length + 1);
      if (buffer) {
        fread (buffer, 1, length, f);
        buffer[length] = '\0';
      }
      fclose (f);
    }
    return buffer;
  } else {
    return NULL;
  }
}
*/



/**
 * Default callback function called if no endpoint has a match
 */
int websocket_cbDefault (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 404, "Page not found, do what you want");
  return U_CALLBACK_CONTINUE;
}





/**
 * websocket_onclose_callback
 * onclose callback function
 * Used to clear data after the websocket connection is closed
 */
void websocket_onclose_callback (const struct _u_request * request,
                                struct _websocket_manager * websocket_manager,
                                void * websocket_onclose_user_data) {
  if (websocket_onclose_user_data != NULL) {
    dbgprintf(DBG_DEBUG, "websocket_onclose_user_data is %s", websocket_onclose_user_data);
    o_free(websocket_onclose_user_data);
  }
}

void websocket_onclose_file_callback (const struct _u_request * request,
                                struct _websocket_manager * websocket_manager,
                                void * websocket_onclose_user_data) {
  dbgprintf(DBG_DEBUG, "websocket_onclose_file_callback");
}

/**
 * websocket_manager_callback
 * send 5 text messages and 1 ping for 11 seconds, then closes the websocket
 */
void websocket_manager_callback(const struct _u_request * request,
                               struct _websocket_manager * websocket_manager,
                               void * websocket_manager_user_data) {
  if (websocket_manager_user_data != NULL) {
    dbgprintf(DBG_DEBUG, "websocket_manager_user_data is %s", websocket_manager_user_data);
  }

  // Send text message without fragmentation
  if (ulfius_websocket_wait_close(websocket_manager, 2000) == U_WEBSOCKET_STATUS_OPEN) {
    if (ulfius_websocket_send_message(websocket_manager, U_WEBSOCKET_OPCODE_TEXT, o_strlen("Message without fragmentation from server"), "Message without fragmentation from server") != U_OK) {
      dbgprintf(DBG_ERROR, "Error send message without fragmentation");
    }
  }

  // Send text message with fragmentation for ulfius clients only, browsers seem to dislike fragmented messages
  if (o_strncmp(u_map_get(request->map_header, "User-Agent"), U_WEBSOCKET_USER_AGENT, o_strlen(U_WEBSOCKET_USER_AGENT)) == 0 &&
      ulfius_websocket_wait_close(websocket_manager, 2000) == U_WEBSOCKET_STATUS_OPEN) {
    if (ulfius_websocket_send_fragmented_message(websocket_manager, U_WEBSOCKET_OPCODE_TEXT, o_strlen("Message with fragmentation from server"), "Message with fragmentation from server", 5) != U_OK) {
      dbgprintf(DBG_ERROR, "Error send message with fragmentation");
    }
  }

  // Send ping message
  if (ulfius_websocket_wait_close(websocket_manager, 2000) == U_WEBSOCKET_STATUS_OPEN) {
    if (ulfius_websocket_send_message(websocket_manager, U_WEBSOCKET_OPCODE_PING, 0, NULL) != U_OK) {
      dbgprintf(DBG_ERROR, "Error send ping message");
    }
  }

  // Send binary message without fragmentation
  if (ulfius_websocket_wait_close(websocket_manager, 2000) == U_WEBSOCKET_STATUS_OPEN) {
    if (ulfius_websocket_send_message(websocket_manager, U_WEBSOCKET_OPCODE_BINARY, o_strlen("Message without fragmentation from server"), "Message without fragmentation from server") != U_OK) {
      dbgprintf(DBG_ERROR, "Error send binary message without fragmentation");
    }
  }

  dbgprintf(DBG_DEBUG, "Closing websocket_manager_callback");
}

void websocket_manager_file_callback(const struct _u_request * request,
                               struct _websocket_manager * websocket_manager,
                               void * websocket_manager_user_data) {
  dbgprintf(DBG_DEBUG, "Opening websocket_manager_file_callback");
  for (;;) {
    sleep(1);
    if (websocket_manager == NULL || !websocket_manager->connected) {
      break;
    }
  }
  dbgprintf(DBG_DEBUG, "Closing websocket_manager_file_callback");
}

/**
 * websocket_incoming_message_callback
 * Read incoming message and prints it on the console
 */
void websocket_incoming_message_callback (const struct _u_request * request,
                                         struct _websocket_manager * websocket_manager,
                                         const struct _websocket_message * last_message,
                                         void * websocket_incoming_message_user_data) {
  if (websocket_incoming_message_user_data != NULL) {
    dbgprintf(DBG_DEBUG, "websocket_incoming_message_user_data is %s", websocket_incoming_message_user_data);
  }
  dbgprintf(DBG_DEBUG, "Incoming message, opcode: 0x%02x, mask: %d, len: %zu", last_message->opcode, last_message->has_mask, last_message->data_len);
  if (last_message->opcode == U_WEBSOCKET_OPCODE_TEXT) {
    dbgprintf(DBG_DEBUG, "text payload '%.*s'", last_message->data_len, last_message->data);
  } else if (last_message->opcode == U_WEBSOCKET_OPCODE_BINARY) {
    dbgprintf(DBG_DEBUG, "binary payload");
  }
}

void websocket_echo_message_callback (const struct _u_request * request,
                                         struct _websocket_manager * websocket_manager,
                                         const struct _websocket_message * last_message,
                                         void * websocket_incoming_message_user_data) {
  dbgprintf(DBG_DEBUG, "Incoming message, opcode: 0x%02x, mask: %d, len: %zu, text payload '%.*s'", last_message->opcode, last_message->has_mask, last_message->data_len, last_message->data_len, last_message->data);
  if (ulfius_websocket_send_message(websocket_manager, last_message->opcode, last_message->data_len, last_message->data) != U_OK) {
    dbgprintf(DBG_ERROR, "Error ulfius_websocket_send_message");
  }
}

void websocket_incoming_file_callback (const struct _u_request * request,
                                         struct _websocket_manager * websocket_manager,
                                         const struct _websocket_message * last_message,
                                         void * websocket_incoming_message_user_data) {
  char * my_message = msprintf("Incoming file %p, opcode: 0x%02x, mask: %d, len: %zu", last_message, last_message->opcode, last_message->has_mask, last_message->data_len);
  dbgprintf(DBG_DEBUG, my_message);
  ulfius_websocket_send_message(websocket_manager, U_WEBSOCKET_OPCODE_TEXT, o_strlen(my_message), my_message);
  o_free(my_message);
}






/**
 * Ulfius main callback function that simply calls the websocket manager and closes
 */
int callback_websocket (const struct _u_request * request, struct _u_response * response, void * user_data) {
  char * websocket_user_data = o_strdup("my_user_data");
  int ret;

  if ((ret = ulfius_set_websocket_response(response, NULL, NULL, &websocket_manager_callback, websocket_user_data, &websocket_incoming_message_callback, websocket_user_data, &websocket_onclose_callback, websocket_user_data)) == U_OK) {
    return U_CALLBACK_CONTINUE;
  } else {
    return U_CALLBACK_ERROR;
  }
}



int callback_websocket_echo (const struct _u_request * request, struct _u_response * response, void * user_data) {
  char * websocket_user_data = o_strdup("my_user_data");
  int ret;

  dbgprintf(DBG_DEBUG, "Client connected to echo websocket");
  if ((ret = ulfius_set_websocket_response(response, NULL, NULL, NULL, NULL, &websocket_echo_message_callback, websocket_user_data, &websocket_onclose_callback, websocket_user_data)) == U_OK) {
    return U_CALLBACK_CONTINUE;
  } else {
    return U_CALLBACK_ERROR;
  }
}

int callback_websocket_file (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int ret;

  if ((ret = ulfius_set_websocket_response(response, NULL, NULL, &websocket_manager_file_callback, NULL, &websocket_incoming_file_callback, NULL, &websocket_onclose_file_callback, NULL)) == U_OK) {
    return U_CALLBACK_CONTINUE;
  } else {
    return U_CALLBACK_ERROR;
  }
}








/**
 * Ulfius main callback function that simply calls the websocket manager and closes
 */
int websocket_cb(const struct _u_request * request, struct _u_response * response, void * user_data) {
  char * websocket_user_data = o_strdup("my_user_data");
  int ret;

  if ((ret = ulfius_set_websocket_response(response, NULL, NULL, &websocket_manager_callback, websocket_user_data, &websocket_incoming_message_callback, websocket_user_data, &websocket_onclose_callback, websocket_user_data)) == U_OK) {
    return U_CALLBACK_CONTINUE;
  } else {
    return U_CALLBACK_ERROR;
  }
}

int websocket_cbEcho(const struct _u_request * request, struct _u_response * response, void * user_data) {
  char * websocket_user_data = o_strdup("my_user_data");
  int ret;

  dbgprintf(DBG_DEBUG, "Client connected to echo websocket");
  if ((ret = ulfius_set_websocket_response(response, NULL, NULL, NULL, NULL, &websocket_echo_message_callback, websocket_user_data, &websocket_onclose_callback, websocket_user_data)) == U_OK) {
    return U_CALLBACK_CONTINUE;
  } else {
    return U_CALLBACK_ERROR;
  }
}

int websocket_cbFile(const struct _u_request * request, struct _u_response * response, void * user_data) {
  int ret;

  if ((ret = ulfius_set_websocket_response(response, NULL, NULL, &websocket_manager_file_callback, NULL, &websocket_incoming_file_callback, NULL, &websocket_onclose_file_callback, NULL)) == U_OK) {
    return U_CALLBACK_CONTINUE;
  } else {
    return U_CALLBACK_ERROR;
  }
}
