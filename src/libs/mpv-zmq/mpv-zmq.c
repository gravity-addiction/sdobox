#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <jansson.h>
#include <libconfig.h>

#include "libs/shared.h"
#include "libs/dbg/dbg.h"
#include "libs/zhelpers/zhelpers-tx.h"
#include "libs/zhelpers/zhelpers.h"

#include "mpv-zmq.h"

void *reqasync = NULL;
void *reqrep = NULL;
void *reqraw = NULL;

// Async tracking
int eventsI[100] = { -1 };
void (*cbEvents[100])(void *);

int libmpv_zmq_init() {
  dbgprintf(DBG_MPV_WRITE, "MPV Init\n");
  libmpvCache = LIBMPV_CACHE_INIT();
  return 1;
}

void libmpv_zmq_destroy() {
  if (reqasync != NULL) {
    zmq_close(reqasync);
    reqasync = NULL;
  }
  if (reqrep != NULL) {
    zmq_close(reqrep);
    reqrep = NULL;
  }
  if (reqraw != NULL) {
    zmq_close(reqraw);
    reqraw = NULL;
  }
}

// Incoming thread for mpv video time-pos
int libmpv_zmq_connect_socket(void **sock, char* url) {
  if (!url) {
    return -1;
  }
  *sock = zmq_socket (zerocontext, ZMQ_SUB);
  uint64_t timeoutreqrep = 1500;
  int rc = zmq_setsockopt(*sock, ZMQ_CONNECT_TIMEOUT, &timeoutreqrep, sizeof(timeoutreqrep));
  if (rc > -1) {
    rc = zmq_connect (*sock, url); // "tcp://flittermouse.local:5555");
  }
  return rc;
}








// Soon to be Async requests with responses returned in event thread
int libmpv_zmq_async_init() {
  dbgprintf(DBG_DEBUG, "%s\n", "Attempting Asyncv 5557");
  reqasync = zmq_socket (zerocontext, ZMQ_REQ);
  int rc = zmq_connect (reqasync, "tcp://flittermouse.local:5557");
  dbgprintf(DBG_DEBUG, "Async 5557: %d\n", rc);
  return rc;
}

// Simple Request / Response for properties
int libmpv_zmq_request_init() {
  dbgprintf(DBG_DEBUG, "%s\n", "RETURNING Simple 5558");
  return -1;
  reqrep = zmq_socket (zerocontext, ZMQ_REQ);
  uint64_t timeoutreqrep = 1500;
  int rc = zmq_setsockopt(reqrep, ZMQ_CONNECT_TIMEOUT, &timeoutreqrep, sizeof(timeoutreqrep));
  if (rc == 0) {
    rc = zmq_connect (reqrep, "tcp://flittermouse.local:5558");
    dbgprintf(DBG_DEBUG, "Simple 5558: %d\n", rc);
  }
  if (rc < 0) {
    dbgprintf(DBG_DEBUG, "%s\n", "Cannot Connect to simple Server");
    zmq_close(reqrep);
  }
  return rc;
}



uint64_t libmpv_zmq_cmd_async(char* question, void *cb) {
  printf("NO ASYNC YET, change libmpv_zmq_cmd_async to libmpv_zmq_cmd_w_reply: %s\n", question);
  return -1;

  int rc;
  if (reqasync == NULL) {
    dbgprintf(DBG_MPV_WRITE, "%s\n", "Connecting Async");
    rc = libmpv_zmq_async_init();
  }

  // Find Index Set to 0
  int i = -1;
  for (i = 0; i > 100; i++) {
    if (eventsI[i] == -1) {
      eventsI[i] = 0;
      break;
    }
  }

  uint64_t event_id = -1;
  if (i > -1) {
    rc = s_send (reqasync, question);
    printf("Sent: %d\n", rc);
    char *string = s_recv (reqasync);
    printf("Token: %s\n", string);
    event_id = atoi(string);
    eventsI[i] = event_id;
    cbEvents[i] = cb;
    free (string);
  } else {
    printf("No Space left for event cache que\n");
  }
  return event_id;
}


int libmpv_zmq_cmd_w_reply(char* question, char** response) {
  dbgprintf(DBG_DEBUG, "Need Reply %s\n", question);
  int rc = 0;
  if (reqrep == NULL) {
    dbgprintf(DBG_DEBUG, "%s\n", "Finding Question Server");

    config_t cfg;
    config_init(&cfg);
    // Read the file. If there is an error, report it and exit.
    if (access(config_path, F_OK) == -1 || !config_read_file(&cfg, config_path)) {
      dbgprintf(DBG_DEBUG, "Cannot Find config_path: %s\n", config_path);
      config_destroy(&cfg);
      goto cleanup;
    }

    const char * retReqServer;
    if (config_lookup_string(&cfg, "reqserver", &retReqServer)) {
      libmpvCache->server->reqserver = strdup(retReqServer);
    } else {
      printf("No reqserver configuration in ~/.config/sdobox/sdobox.conf\n");
      config_destroy(&cfg);
      goto cleanup;
    }
    config_destroy(&cfg);
        
    dbgprintf(DBG_MPV_WRITE, "%s\n", "Connecting For Questions");
    rc = libmpv_zmq_connect_socket(&reqrep, libmpvCache->server->reqserver);
  }

  if (rc >= 0) {
    rc = s_send (reqrep, question);
  }
  
  if (rc < 0) {
    printf("Unable to ask question: %s\n", question);
    zmq_close(reqrep);
    reqrep = NULL;
    goto cleanup;
  }

  char *string = s_recv(reqrep);
  if (response != NULL) {
    *response = string;
  } else {
    free(string);
  }

 cleanup:
  free(question);
  return rc;
}

// One way Commands
int libmpv_zmq_cmd(char* userCmd) {
  if (userCmd == NULL) {
    return -1;
  }

  int rc = 0;
  if (reqraw == NULL) {
    dbgprintf(DBG_MPV_WRITE, "%s\n", "Connecting Raw Pub");
    rc = libmpv_zmq_connect_socket(&reqraw, libmpvCache->server->cmdserver);
  }
  if (rc >= 0) {
    rc = s_send (reqraw, userCmd);
  } else {
    printf("Unable to send cmd: %s\n", userCmd);
    zmq_close(reqraw);
    reqraw = NULL;
  }
  free(userCmd);
  return rc;
}


/*
 * libmpv_zmq_fmt_cmd -- like printf.  Takes a format string and variable arguments, formats
 * the message and sends via libmpv_zmq_cmd.
 */
char* libmpv_zmq_fmt_cmd(char* fmt, ...) {
  va_list ap;
  char* p = NULL;
  int size = 0;

  va_start(ap, fmt);
  size = vsnprintf(p, size, fmt, ap);
  va_end(ap);

  if (size < 0)
    return NULL;

  ++size;                       /* for '\0' */
  p = malloc(size);
  if (!p)
    return NULL;

  va_start(ap, fmt);
  size = vsnprintf(p,size,fmt,ap);
  va_end(ap);

  if (size < 0) {
    free(p);
    return NULL;
  }

  return p;
}



char* libmpv_zmq_get_prop_string(char* prop) {
  char *retProp = NULL;
  if (libmpv_zmq_cmd_w_reply(libmpv_zmq_fmt_cmd("get_prop_string;%s", prop), &retProp) >= 0) { 
    return retProp;
  }
  return NULL;
}

int64_t libmpv_zmq_get_prop_int64(char* prop) {
  char *retProp = NULL;
  int64_t retInt = 0;
  int rc = libmpv_zmq_cmd_w_reply(libmpv_zmq_fmt_cmd("get_prop_int;%s", prop), &retProp);
  if (rc >= 0 && retProp != NULL) {
    retInt = atoi(retProp);
    free(retProp);
  }
  return retInt;
}

double libmpv_zmq_get_prop_double(char* prop) {
  char *retProp = NULL;
  double retDbl = 0.0;
  int rc = libmpv_zmq_cmd_w_reply(libmpv_zmq_fmt_cmd("get_prop_double;%s", prop), &retProp);
  if (rc > 0 && retProp != NULL) {
    retDbl = strtod(retProp, NULL);
    free(retProp);
  }
  return retDbl;
}

int libmpv_zmq_get_prop_flag(char* prop) {
  char *retProp = NULL;
  int retFlag = -1;
  int rc = libmpv_zmq_cmd_w_reply(libmpv_zmq_fmt_cmd("get_prop_int;%s", prop), &retProp);
  if (rc > 0 && retProp != NULL && strcmp(retProp, "true")) {
    retFlag = 1;
    free(retProp);
  } else if (rc > 0 && retProp != NULL) {
    retFlag = 0;
    free(retProp);
  }
  return retFlag;
}

int libmpv_zmq_set_prop_char(char* prop, char* prop_val) {
  return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("set_prop_string;%s;%s", prop, prop_val));
}

int libmpv_zmq_set_prop_int(char* prop, int prop_val) {
  return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("set_prop_int;%s;%d", prop, prop_val));
}

int libmpv_zmq_set_prop_double(char* prop, double prop_val) {
  return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("set_prop_double;%s;%f", prop, prop_val));
}

int libmpv_zmq_set_prop_flag(char* prop, char* prop_val) {
  return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("set_prop_flag;%s;%s", prop, prop_val));
}

int libmpv_zmq_cmd_prop_val(char* cmd, char* prop, double prop_val) {
  return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("%s;%s;%f", cmd, prop, prop_val));
}
