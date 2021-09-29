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

#include "libs/shared.h"
#include "libs/dbg/dbg.h"
#include "libs/fbbg/fbbg.h"
#include "libs/fbcp/fbcp.h"
#include "libs/zhelpers.h"
#include "mpv2.h"


struct libMpv2PlayerInfo * LIBMPV_EVENTS_INIT_INFO() {
  struct libMpv2PlayerInfo *eventInfo = (struct libMpv2PlayerInfo*)malloc(sizeof(struct libMpv2PlayerInfo));
  eventInfo->cnt = 0;
  eventInfo->has_file = 0;
  eventInfo->is_loaded = 0;
  eventInfo->is_playing = 0;
  eventInfo->is_seeking = 0;
  eventInfo->position = 0.00;
  eventInfo->duration = 0.00;
  eventInfo->pbrate = 1.00;
  eventInfo->pbrateUser = 0.75;
  eventInfo->pbrateStr = (char*)malloc(10 * sizeof(char));
  eventInfo->pbrateStr[0] = '\0';
  eventInfo->pbrateUserStr = (char*)malloc(10 * sizeof(char));
  eventInfo->pbrateUserStr[0] = '\0';
  eventInfo->positionStr = (char*)malloc(32 * sizeof(char));
  eventInfo->positionStr[0] = '\0';
  eventInfo->folder = (char*)malloc(1024 * sizeof(char));
  eventInfo->folder[0] = '\0';
  eventInfo->file = (char*)malloc(512 * sizeof(char));
  eventInfo->file[0] = '\0';
  return eventInfo;
}

void LIBMPV_EVENTS_DESTROY_INFO(struct libMpv2PlayerInfo *eventInfo) {
  free(eventInfo->positionStr);
  free(eventInfo->pbrateStr);
  free(eventInfo->pbrateUserStr);
  free(eventInfo->folder);
  free(eventInfo->file);
  // free(eventInfo);
}

void *rawrep = NULL;
void *reqrep = NULL;


int libmpv2_init() {
  dbgprintf(DBG_MPV_WRITE, "MPV Init\n");
  
  reqId = 1;
  reqTop = 2147483647;

  libMpvVideoInfo = LIBMPV_EVENTS_INIT_INFO();

  return 1;
}

void libmpv2_destroy() {
  LIBMPV_EVENTS_DESTROY_INFO(libMpvVideoInfo);
  free(libMpvVideoInfo);
  if (rawrep != NULL) {
    zmq_close(rawrep);
    rawrep = NULL;
  }
  if (reqrep != NULL) {
    zmq_close(reqrep);
    reqrep = NULL;
  }
}

int libmpv2_request_init() {
  reqrep = zmq_socket (zerocontext, ZMQ_REQ);
  return zmq_connect (reqrep, "tcp://192.168.126.85:5557");
}
int libmpv2_pub_init() {
  rawrep = zmq_socket (zerocontext, ZMQ_PUB);
  return zmq_connect (rawrep, "tcp://192.168.126.85:5558");
}


int mpv_cmd_w_reply(char* question, char** response) {
  int rc;

  if (reqrep == NULL) {
    dbgprintf(DBG_MPV_WRITE, "%s\n", "Connecting Requests");
    rc = libmpv2_request_init();
  }

  rc = s_send (reqrep, question);
  char *string = s_recv (reqrep);

  size_t strLen = snprintf(NULL, 0, "%s", string) + 1;
  *response = (char*)malloc(strLen * sizeof(char));
  strlcpy(*response, string, strLen);
  free (string);

  return rc;
}

int mpv_cmd(char* question) {
  int rc;

  if (rawrep == NULL) {
    dbgprintf(DBG_MPV_WRITE, "%s\n", "Connecting Raw Pub");
    rc = libmpv2_pub_init();
  }

  return zmq_send(rawrep, question, strlen(question), ZMQ_DONTWAIT);
}


/*
 * mpv_fmt_cmd -- like printf.  Takes a format string and variable arguments, formats
 * the message and sends via mpv_cmd.
 */
char* mpv_fmt_cmd(char* fmt, ...) {
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



int mpv_set_prop_char(char* prop, char* prop_val) {
  return mpv_cmd(mpv_fmt_cmd("set %s %s\n", prop, prop_val));
}

int mpv_set_prop_int(char* prop, int prop_val) {
  return mpv_cmd(mpv_fmt_cmd("set %s %d\n", prop, prop_val));
}

int mpv_set_prop_double(char* prop, double prop_val) {
  return mpv_cmd(mpv_fmt_cmd("set %s %f\n", prop, prop_val));
}

int mpv_cmd_prop_val(char* cmd, char* prop, double prop_val) {
  return mpv_cmd(mpv_fmt_cmd("%s %s %f\n", cmd, prop, prop_val));
}

int mpv_seek(double distance) {
  if (!libMpvVideoInfo->is_loaded) { return 0; }
  return mpv_cmd(mpv_fmt_cmd("seek %f\n", distance));
}

int mpv_seek_arg(double distance, char* flags) {
  if (!libMpvVideoInfo->is_loaded) { return 0; }
  return mpv_cmd(mpv_fmt_cmd("seek %f %s\n", distance, flags));
}

void mpv_check_pause() {
  char *paused = NULL;
  int rc = mpv_cmd_w_reply("\"get_property\", \"pause\"", &paused);
  if (paused != NULL) {
    if (strncmp(paused, "false", 5) == 0) {
      libMpvVideoInfo->is_playing = 1;
    } else {
      libMpvVideoInfo->is_playing = 0;
    }
    free(paused);
  }
}
int mpv_pause() {
  libMpvVideoInfo->is_playing = 0;
  return mpv_cmd(mpv_fmt_cmd("{\"command\": [\"set_property\", \"%s\", %s]}\n", "pause", "true"));
}

int mpv_play() {
  libMpvVideoInfo->is_playing = 1;
  // Start Background layer behind mpv images
  // fbbg_start();
  return mpv_cmd(mpv_fmt_cmd("{\"command\": [\"set_property\", \"%s\", %s]}\n", "pause", "false"));
}

int mpv_fullscreen(int fs) {
  if (fs) {
    return mpv_cmd(mpv_fmt_cmd("{\"command\": [\"set_property\", \"%s\", %s]}\n", "fullscreen", "true"));
  } else {
    return mpv_cmd(mpv_fmt_cmd("{\"command\": [\"set_property\", \"%s\", %s]}\n", "fullscreen", "false"));
  }
}

int mpv_volume_mute() {
  return mpv_cmd(mpv_fmt_cmd("{\"command\": [\"set_property\", \"%s\", %d]}\n", "volume", 0));
}
int mpv_volume_on() {
  return mpv_cmd(mpv_fmt_cmd("{\"command\": [\"set_property\", \"%s\", %d]}\n", "volume", 100));
}

int mpv_playpause_toggle() {
  if (!libMpvVideoInfo->is_loaded) {
    return 0;
  }
  if (libMpvVideoInfo->is_playing) {
    return mpv_pause();
  } else {
    return mpv_play();
  }
}


// MPV Player Speed
double mpv_speed(double spd) {
  mpv_set_prop_double("speed", spd);
  libMpvVideoInfo->pbrate = spd;
  return libMpvVideoInfo->pbrate;
}

double mpv_speed_adjust(double spd) {
  char *retSpeed = NULL;
  double new_spd;
  int rc = mpv_cmd_w_reply("\"get_property\", \"speed\"", &retSpeed);
  if (retSpeed != NULL) {
    double number = strtod(retSpeed, NULL);
    free(retSpeed);

    if (number == 0) { return -1; }

    if (number <= 0.1) {
      new_spd = number + (spd / 10);
    } else {
      new_spd = number + spd;
    }

    if (new_spd > 1) {
      new_spd = 1;
    } else if (new_spd < .01) {
      new_spd = .01;
    } else if (new_spd == .11) {
      new_spd = .2;
    }
    if (number != new_spd) {
      return mpv_speed(new_spd);
      // sprintf(m_player_playback_speed, "%.0f%%", (i_mpv_playback_speed * 100));
    }
    return libMpvVideoInfo->pbrate;
  }
  return -1;
}


void mpv_stop() {
  fbbg_stop();
  mpv_cmd(mpv_fmt_cmd("stop"));
}

void mpv_playlist_clear() {
  mpv_cmd(mpv_fmt_cmd("playlist-clear"));
}

//
// Transform folder & filenames that might have double-quotes or
// back-quotes in them so that we can safely wrap the whole path in
// double-quotes.
//
static char* quotify(char* original, char** saved_replacement) {
  if (strchr(original, '"') == NULL && strchr(original, '\\') == NULL) {
    return original;
  }
  else {
    char* dst = *saved_replacement = malloc(strlen(original) * 2 + 1);
    char* src = original;
    while(*src) {
      if (*src == '"' || *src == '\\')
        *dst++ = '\\';

      *dst++ = *src++;
    }
    *dst++ = 0;
    return *saved_replacement;
  }
}

int mpv_loadfile(char* folder, char* filename, char* flag, char* opts) {

  char * qfolder = NULL;
  char * qfilename = NULL;
  folder = quotify(folder,&qfolder);
  filename = quotify(filename,&qfilename);

  libMpvVideoInfo->has_file = 1;
  strlcpy(libMpvVideoInfo->folder, folder, 1024);
  strlcpy(libMpvVideoInfo->file, filename, 512);
  int result = mpv_cmd(mpv_fmt_cmd("loadfile \"%s/%s\" %s %s", folder, filename, flag, opts));
  mpv_pause();

  // Free the possibly-allocated replacement strings -- free(NULL) is a safe no-op.
  free(qfolder);
  free(qfilename);

  return result;
}

void mpv_quit() {
  fbbg_stop();
  mpv_cmd(mpv_fmt_cmd("quit"));
}

void stop_video() {
  // mpv_quit();
  // m_bPosThreadStop = 1;
}




