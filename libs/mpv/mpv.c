#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>           /* millis() */

// #include <mpv/client.h>
#include "shared.h"
#include "mpv.h"

#include "dbg/dbg.h"

#include "queue/queue.h"




int mpv_socket_conn() {
  int fd;
  struct sockaddr_un addr;
  // Wait for socket to arrive
  if (access(mpv_socket_path, R_OK) == -1) {
    dbgprintf(DBG_ERROR, "No Socket Available for Singlet %s\n", mpv_socket_path);
    return -1;
  }

  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    dbgprintf(DBG_ERROR, "%s\n", "MPV Socket Error");
    return -1;
  }

  // Set Socket Non-Blocking
  setnonblock(fd);

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  if (*mpv_socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path+1, mpv_socket_path+1, sizeof(addr.sun_path)-2);
  } else {
    strncpy(addr.sun_path, mpv_socket_path, sizeof(addr.sun_path)-1);
  }

  if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
    dbgprintf(DBG_ERROR, "%s\n%s\n", "MPV Singlet Connect Error", strerror(errno));
    return -1;
  }
  return fd;
}

void mpv_socket_close(int fd) {
  if (fd) { close(fd); }
}



int mpv_init() {
  dbgprintf(DBG_MPV_WRITE, "MPV Init\n");
  mpv_socket_path = "/tmp/mpv.socket";
  mpv_fifo_path = "/tmp/mpv.fifo";

  // if (!mpv_socket_fd) {
  //  mpv_socket_fd = mpv_socket_conn();
  //}

  mpv_socket_fdSelect = -1;

  mpv_stop();
  mpv_playlist_clear();

  m_is_video_playing = 0;
  mi_video_fps_frame = 0;

  mpv_video_duration = 0.00;
  mpv_video_pos = 0.00;


  mi_video_fps = 0;
  m_video_lock = 0;

  mpv_video_rate = 1.0;
  m_nPosPlayer = 0;

  i_chapter_cnt = 0; // placeholder for chapter count

  return 1;
}





/*
static inline void check_error(int status) {
  if (status < 0) {
    // printf("mpv API error: %s\n", mpv_error_string(status));
    exit(1);
  }
}
*/
int mpv_create_player(char* filePath) {
  /*
  mpv_handle *ctx = mpv_create();
  if (!ctx) {
    // printf("failed creating context\n");
    return 0;
  }

  // Enable default key bindings, so the user can actually interact with
  // the player (and e.g. close the window).
  // /usr/bin/mpv
  // --video-sync=display-resample
  // --hr-seek-framedrop=yes
  // --input-ipc-server=/tmp/mpv.socket

  // --no-osc
  // --no-osd-bar
  // --osd-on-seek=no
  // --rpi-display=0
  // --rpi-layer=1
  // --rpi-background=yes
  // --rpi-osd=no
  // --reset-on-next-file=all
  // --keep-open=always
  // --idle

  // debug_print("%s\n", "video-sync");
  check_error(mpv_set_option_string(ctx, "video-sync", "display-resample"));
  // debug_print("%s\n", "hr-seek-framedrop");
  check_error(mpv_set_option_string(ctx, "hr-seek-framedrop", "yes"));
  // debug_print("%s\n", "osd-on-seek");
  check_error(mpv_set_option_string(ctx, "osd-on-seek", "no"));

  // debug_print("%s\n", "reset-on-next-file");
  check_error(mpv_set_option_string(ctx, "reset-on-next-file", "all"));
  // debug_print("%s\n", "keep-open");
  check_error(mpv_set_option_string(ctx, "keep-open", "always"));

  // debug_print("%s\n", "vo");
  check_error(mpv_set_option_string(ctx, "vo", "rpi"));
  // debug_print("%s\n", "hwdec");
  check_error(mpv_set_option_string(ctx, "hwdec", "mmal"));
  // Done setting up options.
  check_error(mpv_initialize(ctx));

  usleep(1000000);
  // Play this file.
  const char *cmd[] = {"loadfile", filePath, NULL};
  check_error(mpv_command(ctx, cmd));

  // Let it play, and wait until the user quits.
  while (1) {
    mpv_event *event = mpv_wait_event(ctx, 10000);
    // printf("event: %s\n", mpv_event_name(event->event_id));
    if (event->event_id == MPV_EVENT_SHUTDOWN)
      break;
  }

  // printf("mpv terminating\n");
  mpv_terminate_destroy(ctx);
  */
  return 1;
}

int mpv_fd_check(int fd) {

  // 16-Feb-2020 - disabling this -- after the synchonization fix
  // applied last week, this really serves no purpose.
  // Reset socket if more than 10 seconds since last reconnection
  if (0 && millis() - mpv_socket_lastConn > 10000) {
    dbgprintf(DBG_MPV_WRITE|DBG_MPV_READ, "Proactively closing MPV socket\n");
    mpv_socket_close(fd);
    return 0;
  }

  // Check Socket is in good standing
  if (fd > -1) {
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt (fd, SOL_SOCKET, SO_ERROR, &error, &len);

    if (retval != 0) {
      /* there was a problem getting the error code */
      dbgprintf(DBG_ERROR, "error getting socket error code: %s\n", strerror(retval));
      mpv_socket_close(fd);
      return 0;
    }

    if (error != 0) {
      /* socket has a non zero error status */
      dbgprintf(DBG_ERROR, "socket error: %s\n", strerror(error));
      mpv_socket_close(mpv_socket_fdSelect);
      return 0;
    }
    return 1;
  }
  return 0;
}

int mpv_fd_write(char *data) {
  if (mpv_fd_check(mpv_socket_fdSelect) == 0) {
    mpv_socket_lastConn = millis();
    mpv_socket_fdSelect = mpv_socket_conn();
    if (mpv_socket_fdSelect == -1) {
      return 0;
    }

    /* Initialize the file descriptor set. */
    FD_ZERO (&mpv_socket_set);
    FD_SET (mpv_socket_fdSelect, &mpv_socket_set);

    /* Initialize the timeout data structure. */
    mpv_socket_timeout.tv_sec = 2;
    mpv_socket_timeout.tv_usec = 0;
  }


  dbgprintf(DBG_MPV_WRITE,
            (data[strlen(data)-1] == '\n' ? "mpvwrite: %s" : "mpvwrite: %s\n"),
            data);
  int writeSz = write(mpv_socket_fdSelect, data, strlen(data));
  if (writeSz != strlen(data)) {
    dbgprintf(DBG_ERROR, "Bad MPV Write\n");
    // mpv_socket_fdSelect = -1;
    // mpv_fd_write(char *data)
    return 0;
  }
  return 1;
}

static pthread_mutex_t mpvSocketAccessLock = PTHREAD_MUTEX_INITIALIZER;

int mpv_cmd(char *cmd_string) {

    pthread_mutex_lock(&mpvSocketAccessLock);
    int fdWrite = mpv_fd_write(cmd_string);
    pthread_mutex_unlock(&mpvSocketAccessLock);

    free(cmd_string);
    return fdWrite;
}

int mpvSocketSinglet(char* prop, char ** json_prop) {
    // dbgprintf(DBG_MPV_WRITE|DBG_MPV_READ, "mpvsinglet: %s\n", prop);

    char* data_tmp = "{ \"command\": [\"get_property\", \"%s\"] }\n";
    size_t len = snprintf(NULL, 0, data_tmp, prop) + 1;
    char *data = (char*)malloc(len * sizeof(char));
    if (data == NULL) {
        dbgprintf(DBG_ERROR, "%s\n%s\n", "Error!, No Memory", strerror(errno));
        return -1;
    }
    snprintf(data, len, data_tmp, prop);

    int result = -1;

    pthread_mutex_lock(&mpvSocketAccessLock);

    /* set the timeout data structure. */
    mpv_socket_timeout.tv_sec = 2;
    mpv_socket_timeout.tv_usec = 0;

    if (!mpv_fd_write(data)) {
        goto cleanup;
    }
    free(data);

    while (1) {
        /* select returns 0 if timeout, 1 if input available, -1 if error. */
        int selT = select(FD_SETSIZE, &mpv_socket_set, NULL, NULL, &mpv_socket_timeout);
        if (selT == -1) {
            dbgprintf(DBG_ERROR, "%s\n%s\n","Error! Closing MPV Socket, SELECT -1", strerror(errno));
            mpv_socket_close(mpv_socket_fdSelect);
            mpv_socket_fdSelect = -1;
            goto cleanup;
        } else if (selT == 1) {
            char* mpv_rpc_ret = NULL;
            int rc = sgetline(mpv_socket_fdSelect, &mpv_rpc_ret);
            if (rc > 0 && mpv_rpc_ret != NULL) {
                // error response
                if (strncmp(mpv_rpc_ret, "{\"error\":\"success\"}", 19) == 0) {
                    dbgprintf(DBG_MPV_READ,
                              "Successful Response after request property %s, %s\n",
                              prop, mpv_rpc_ret);
                    *json_prop = strdup("true");
                    free(mpv_rpc_ret);
                    goto cleanup;
                    // data response
                } else if (strncmp(mpv_rpc_ret, "{\"error\":", 9) == 0) {
                    dbgprintf(DBG_MPV_READ|DBG_ERROR,
                              "Error after requesting property %s, %s\n",
                              prop, mpv_rpc_ret);
                    free(mpv_rpc_ret);
                    goto cleanup;
                    // data response
                } else if (strncmp(mpv_rpc_ret, "{\"data\":", 8) == 0) {
                    dbgprintf(DBG_MPV_READ, "mpvread: '%s'\n", mpv_rpc_ret);
                    result = ta_json_parse(mpv_rpc_ret, "data", json_prop);
                    free(mpv_rpc_ret);
                    goto cleanup;
                    // every other response
                } else {
                    dbgprintf(DBG_MPV_READ, "mpvread - ignoring: '%s'\n", mpv_rpc_ret);
                    free(mpv_rpc_ret);
                    // ...and try again
                }
            } else {
                if (mpv_rpc_ret != NULL) { free(mpv_rpc_ret); }
                goto cleanup;
            }
        }
    }

 cleanup:
    pthread_mutex_unlock(&mpvSocketAccessLock);
    return result;
}


/*
 * mpv_fmt_cmd -- like printf.  Takes a format string and variable arguments, formats
 * the message and sends via mpv_cmd.
 */
int mpv_fmt_cmd(char* fmt, ...) {
  va_list ap;
  char* p = NULL;
  int size = 0;

  va_start(ap, fmt);
  size = vsnprintf(p, size, fmt, ap);
  va_end(ap);

  if (size < 0)
    return -1;

  ++size;                       /* for '\0' */
  p = malloc(size);
  if (!p)
    return -1;

  va_start(ap, fmt);
  size = vsnprintf(p,size,fmt,ap);
  va_end(ap);

  if (size < 0) {
    free(p);
    return -1;
  }

  return mpv_cmd(p);
}

int mpv_set_prop_char(char* prop, char* prop_val) {
  return mpv_fmt_cmd("set %s %s\n", prop, prop_val);
}

int mpv_set_prop_int(char* prop, int prop_val) {
  return mpv_fmt_cmd("set %s %d\n", prop, prop_val);
}

int mpv_set_prop_double(char* prop, double prop_val) {
  return mpv_fmt_cmd("set %s %f\n", prop, prop_val);
}

int mpv_cmd_prop_val(char* cmd, char* prop, double prop_val) {
  return mpv_fmt_cmd("%s %s %f\n", cmd, prop, prop_val);
}

int mpv_seek(double distance) {
  return mpv_fmt_cmd("seek %f\n", distance);
}

int mpv_seek_arg(double distance, char* flags) {
  return mpv_fmt_cmd("seek %f %s\n", distance, flags);
}

int mpv_pause() {
  m_is_video_playing = 0;
  return mpv_fmt_cmd("{\"command\": [\"set_property\", \"%s\", %s]}\n", "pause", "true");
}

int mpv_play() {
  m_is_video_playing = 1;
  return mpv_fmt_cmd("{\"command\": [\"set_property\", \"%s\", %s]}\n", "pause", "false");
}

int mpv_playpause_toggle() {
  /*char* retPlay;
  dbgprintf(DBG_INFO, "A");
  if ((mpvSocketSinglet("pause", &retPlay)) != -1) {
    dbgprintf(DBG_INFO, "B");
    if (strncmp(retPlay, "false", 5) == 0) {
      dbgprintf(DBG_INFO, "C\n");
      return mpv_play();
    } else {
      dbgprintf(DBG_INFO, "D\n");
      return mpv_pause();
    }
  } else */
  if (m_is_video_playing) {
    dbgprintf(DBG_INFO, "E\n");
    return mpv_pause();
  } else {
    dbgprintf(DBG_INFO, "F\n");
    return mpv_play();
  }
}


// MPV Player Speed
double mpv_speed(double spd) {
  mpv_set_prop_double("speed", spd);
  mpv_video_rate = spd;
  return mpv_video_rate;
}

double mpv_speed_adjust(double spd) {
  char* retSpeed;
  double new_spd;
  if (mpvSocketSinglet("speed", &retSpeed)) {
    double number = atof(retSpeed);
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
      mpv_speed(new_spd);
      // sprintf(m_player_playback_speed, "%.0f%%", (i_mpv_playback_speed * 100));
    }
    return mpv_video_rate;
  }
  return -1;
}


void mpv_stop() {
  m_is_video_playing = 0;
  mpv_fmt_cmd("stop\n");
}

void mpv_playlist_clear() {
  mpv_fmt_cmd("playlist-clear\n");
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

  int result = mpv_fmt_cmd("loadfile \"%s/%s\" %s %s\n", folder, filename, flag, opts);

  // Free the possibly-allocated replacement strings -- free(NULL) is a safe no-op.
  free(qfolder);
  free(qfilename);

  return result;
}

void mpv_quit() {
  mpv_fmt_cmd("quit\n");
  mpv_socket_close(mpv_socket_fd);
}

void stop_video() {
  // mpv_quit();
  // m_bPosThreadStop = 1;
}

/*

// https://mpv.io/manual/stable/#terminal-status-line
// The audio/video sync as A-V:  0.000. This is the difference between audio
// and video time. Normally it should be 0 or close to 0. If it's growing, it
// might indicate a playback problem. (avsync property.)
// Total A/V sync change, e.g. ct: -0.417. Normally invisible. Can show up if
// there is audio "missing", or not enough frames can be dropped. Usually this
// will indicate a problem. (total-avsync-change property.)
int check_avsync() {
  char* retPosition;
  int ret;
  ret = mpvSocketSinglet("avsync", &retPosition);
  // debug_print("Video Async: %s\n", retPosition);
  return ret;
}

int video_display_fps() {
  char* retFps;
  int ret;
  ret = mpvSocketSinglet("display-fps", &retFps);
  // debug_print("Video Display FPS: %s\n", retFps);
  char* pEnd;
  mi_video_fps = strtod(retFps, &pEnd);
  return ret;
}

int video_estimated_display_fps() {
  char* retFps;
  int ret;
  ret = mpvSocketSinglet("estimated-display-fps", &retFps);
  // debug_print("Video Estimated Display FPS: %s\n", retFps);
  return ret;
}
*/
