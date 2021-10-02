#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <jansson.h>

#include "libs/shared.h"
#include "libs/dbg/dbg.h"
#include "libs/fbbg/fbbg.h"

#include "./mpv-zmq-helper.h"
#include "./mpv-zmq.h"

int mpv_seek(double distance) {
  if (!libmpvCache->player->is_loaded) { return 0; }
  return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("seek;%f;", distance));
}

int mpv_seek_arg(double distance, char* flags) {
  if (!libmpvCache->player->is_loaded) { return 0; }
  return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("seek;%f;%s;", distance, flags));
}

void mpv_check_pause() {
  char *paused = NULL;
  int rc = libmpv_zmq_cmd_w_reply(strdup("get_prop_flag;pause"), &paused);
  if (paused != NULL) {
    if (strncmp(paused, "false", 5) == 0) {
      libmpvCache->player->is_playing = 1;
    } else {
      libmpvCache->player->is_playing = 0;
    }
    free(paused);
  }
}
int mpv_pause() {
  libmpvCache->player->is_playing = 0;
  return libmpv_zmq_cmd(strdup("set_prop_flag;pause;true"));
}

int mpv_play() {
  libmpvCache->player->is_playing = 1;
  // Start Background layer behind mpv images
  // fbbg_start();
  return libmpv_zmq_cmd(strdup("set_prop_flag;pause;false"));
}

int mpv_fullscreen(int fs) {
  if (fs) {
    return libmpv_zmq_cmd(strdup("set_prop_flag;fullscreen;true"));
  } else {
    return libmpv_zmq_cmd(strdup("set_prop_flag;fullscreen;false"));
  }
}

int mpv_volume_mute() {
  return libmpv_zmq_set_prop_double("volume", 0.0);
}
int mpv_volume_on() {
  return libmpv_zmq_set_prop_double("volume", 100);
}

int mpv_playpause_toggle() {
  if (!libmpvCache->player->is_loaded) {
    printf("Video Not Loaded\n");
    return 0;
  }
  if (libmpvCache->player->is_playing) {
    return mpv_pause();
  } else {
    return mpv_play();
  }
}


// MPV Player Speed
double mpv_speed(double spd) {
  libmpv_zmq_set_prop_double("speed", spd);
  libmpvCache->player->pbrate = spd;
  return libmpvCache->player->pbrate;
}

double mpv_speed_adjust(double spd) {
  double new_spd;
  double cur_spd = libmpv_zmq_get_prop_double("speed");
  if (cur_spd == 0) { return -1; }

  if (cur_spd <= 0.1) {
    new_spd = cur_spd + (spd / 10);
  } else {
    new_spd = cur_spd + spd;
  }

  if (new_spd > 1) {
    new_spd = 1;
  } else if (new_spd < .01) {
    new_spd = .01;
  } else if (new_spd == .11) {
    new_spd = .2;
  }
  if (cur_spd != new_spd) {
    return mpv_speed(new_spd);
    // sprintf(m_player_playback_speed, "%.0f%%", (i_mpv_playback_speed * 100));
  }
  return libmpvCache->player->pbrate;
}


double mpv_time_pos() {
  return libmpv_zmq_get_prop_double("time-pos");
}


void mpv_stop() {
  fbbg_stop();
  libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("stop"));
}

void mpv_playlist_clear() {
  libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("playlist-clear"));
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

  libmpvCache->player->has_file = 1;
  strlcpy(libmpvCache->player->folder, folder, 1024);
  strlcpy(libmpvCache->player->file, filename, 512);
  int result = libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("loadfile;%s/%s;%s;%s;", folder, filename, flag, opts));
  mpv_pause();

  // Free the possibly-allocated replacement strings -- free(NULL) is a safe no-op.
  free(qfolder);
  free(qfilename);

  return result;
}

void mpv_quit() {
  fbbg_stop();
  libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("quit"));
}

void stop_video() {
  // mpv_quit();
  // m_bPosThreadStop = 1;
}




