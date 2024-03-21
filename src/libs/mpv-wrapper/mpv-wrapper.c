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
#include "./mpv-cache.h"
#include "./mpv-wrapper.h"
#include "libs/mpv-socket/mpv-socket.h"
#include "libs/mpv-zmq/mpv-zmq.h"

#include "libs/zhelpers/zhelpers-conn.h"
#include "libs/zhelpers/zhelpers-tx.h"
#include "libs/zhelpers/zhelpers.h"

int libmpv_wrapper_init() {
  dbgprintf(DBG_MPV_WRITE, "MPV Wrapper Init\n");

  // libmpvCache = LIBMPV_CACHE_INIT();
  pg_mpvEvents = zmq_socket(libzhelpers_context(), ZMQ_PUB);
  int rc = zmq_connect(pg_mpvEvents, "inproc://videoserver");
  if (rc != 0) {
    printf("Cannot Start Mpv Events Proc: %d\n", rc);
  }

  return 1;
}

double mpv_calc_marktime(struct lib_mpv_player *player) {
  double markTime = player->position;
  // double nDiff = (((double)(s_clock() - player->position_update)) * player->pbrate) / 1000;

  if (player->is_playing == 1) {
    // printf("Calc %d -- %f .. %f .. %f .. %lld\n", libmpvCache->player->is_playing, nDiff, markTime, player->position, player->position_update);
    // if (nDiff < libmpvCache->player->duration) {
    //   markTime = markTime + nDiff;
    // }

    // printf("Is Playing At: %f\n", player->pbrate);
    markTime = markTime - (0.25 * player->pbrate);
  }

  // Check valid markTime
  if (markTime < 0) {
    markTime = 0;
  } else if (markTime > libmpvCache->player->duration) {
    markTime = libmpvCache->player->duration;
  }
  return markTime;
}

int mpv_seek(double distance) {
  // if (!libmpvCache->player->is_loaded) { return 0; }
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    return mpv_socket_seek(distance);
  break;
  case E_MPV_PLAYER_ZMQ:
    return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("seek;%f", distance));
  break;
  }
  return 1;
}

int mpv_seek_arg(double distance, char* flags) {
  // if (!libmpvCache->player->is_loaded) { return 0; }
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    return mpv_socket_seek_arg(distance, flags);
  break;
  case E_MPV_PLAYER_ZMQ:
    return libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("seek;%f;%s", distance, flags));
  break;
  }
  return 1;
}

void mpv_check_pause() {
  dbgprintf(DBG_DEBUG, "Check Pausing\n");
  char *paused = NULL;
  int rc;
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    return mpv_check_pause(strdup("get_prop_flag;pause"), &paused);
  break;
  case E_MPV_PLAYER_ZMQ:
    rc = libmpv_zmq_cmd_w_reply(strdup("get_prop_flag;pause"), &paused);
    if (rc > 0 && paused != NULL) {
      if (strncmp(paused, "false", 5) == 0) {
        libmpvCache->player->is_playing = 1;
      } else {
        libmpvCache->player->is_playing = 0;
      }
      free(paused);
    }
  break;
  }
}

int mpv_pause() {
  // libmpvCache->player->is_playing = 0;
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    return mpv_socket_pause();
  break;
  case E_MPV_PLAYER_ZMQ:
    return libmpv_zmq_cmd(strdup("set_prop_flag;pause;true"));
  break;
  }
  return 1;
}

int mpv_play() {
  // libmpvCache->player->is_playing = 1;
  // Start Background layer behind mpv images
  // fbbg_start();
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    return mpv_socket_play();
  break;
  case E_MPV_PLAYER_ZMQ:
    return libmpv_zmq_cmd(strdup("set_prop_flag;pause;false"));
  break;
  }
  return 1;
}

int mpv_fullscreen(int fs) {
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    return mpv_socket_fullscreen(fs);
  break;
  case E_MPV_PLAYER_ZMQ:
    if (fs) {
      return libmpv_zmq_cmd(strdup("set_prop_flag;fullscreen;true"));
    } else {
      return libmpv_zmq_cmd(strdup("set_prop_flag;fullscreen;false"));
    }
  break;
  }
  return 1;
}

int mpv_volume_mute() {
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    return mpv_socket_volume_mute();
  break;
  case E_MPV_PLAYER_ZMQ:
    return libmpv_zmq_set_prop_double("volume", 0.0);
  break;
  }
  return 1;
}
int mpv_volume_on() {
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    return mpv_socket_volume_on();
  break;
  case E_MPV_PLAYER_ZMQ:
    return libmpv_zmq_set_prop_double("volume", 100);
  break;
  }
  return 1;
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
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    mpv_socket_speed(spd);
  break;
  case E_MPV_PLAYER_ZMQ:
    libmpv_zmq_set_prop_double("speed", spd);
  break;
  }
  libmpvCache->player->pbrate = spd;
  return libmpvCache->player->pbrate;
}

double mpv_speed_adjust(double spd) {
  double new_spd;
  double cur_spd;
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    cur_spd = mpv_socket_speed_adjust(spd);
  break;
  case E_MPV_PLAYER_ZMQ:
    cur_spd = libmpv_zmq_get_prop_double("speed");

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
  break;
  }

  return libmpvCache->player->pbrate;
}


double mpv_time_pos() {
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:

  break;
  case E_MPV_PLAYER_ZMQ:
    return libmpv_zmq_get_prop_double("time-pos");
  break;
  }
  return 0.0;
}


void mpv_stop() {
  fbbg_stop();
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    mpv_socket_stop();
  break;
  case E_MPV_PLAYER_ZMQ:
    libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("stop"));
  break;
  }

}

void mpv_playlist_clear() {
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    mpv_socket_playlist_clear();
  break;
  case E_MPV_PLAYER_ZMQ:
    libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("playlist-clear"));
  break;
  }
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
  int result;
  folder = quotify(folder,&qfolder);
  filename = quotify(filename,&qfilename);

  libmpvCache->player->has_file = 1;
  strlcpy(libmpvCache->player->folder, folder, 1024);
  strlcpy(libmpvCache->player->file, filename, 512);

  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    result = mpv_socket_loadfile(folder, filename, flag, opts);
  break;
  case E_MPV_PLAYER_ZMQ:
    result = libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("loadfile;%s/%s;%s;%s;", folder, filename, flag, opts));
    mpv_pause();
  break;
  }

  // Free the possibly-allocated replacement strings -- free(NULL) is a safe no-op.
  free(qfolder);
  free(qfilename);

  return result;
}

int mpv_loadurl(char* url, char* flag, char* opts) {
  int result;

  // libmpvCache->player->has_file = 1;
  libmpvCache->player->url = strdup(url);
  // strlcpy(libmpvCache->player->url, url, 512);
  dbgprintf(DBG_DEBUG, "Incoming: %s\n", url);
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    result = mpv_socket_loadurl(libmpvCache->player->url, flag, opts);
    // mpv_pause();
  break;
  case E_MPV_PLAYER_ZMQ:
    result = libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("loadfile;%s;%s;%s", url, flag, opts));
    mpv_pause();
  break;
  }
  dbgprintf(DBG_DEBUG, "Returing\n");
  return result;
}

void mpv_quit() {
  fbbg_stop();
  switch (libmpvCache->player_out) {
  case E_MPV_PLAYER_SOCKET:
    mpv_socket_quit();
  break;
  case E_MPV_PLAYER_ZMQ:
    libmpv_zmq_cmd(libmpv_zmq_fmt_cmd("quit"));
  break;
  }

}

void stop_video() {
  // mpv_quit();
  // m_bPosThreadStop = 1;
}




