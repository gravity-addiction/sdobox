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

// #include <mpv/client.h>
#include "shared.h"
#include "mpv.h"

#include "queue/queue.h"




int mpv_socket_conn() {
  int fd;
  struct sockaddr_un addr;
  // Wait for socket to arrive
  if (access(mpv_socket_path, R_OK) == -1) {
    printf("No Socket Available for Singlet %s\n", mpv_socket_path);
    return -1;
  }

  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    printf("%s\n", "MPV Socket Error");
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
    printf("%s\n%s\n", "MPV Singlett Connect Error", strerror(errno));
    return -1;
  }
  return fd;
}

void mpv_socket_close(int fd) {
  if (fd) { close(fd); }
}



int mpv_init(gslc_tsGui *pGui) {
  printf("%s\n", "MPV Init");
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
  // Reset socket if more than 10 seconds since last reconnection
  if (millis() - mpv_socket_lastConn > 10000) {
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
      printf("error getting socket error code: %s\n", strerror(retval));
      mpv_socket_close(fd);
      return 0;
    }

    if (error != 0) {
      /* socket has a non zero error status */
      printf("socket error: %s\n", strerror(error));
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
  printf("Write: %s\n", data);
  int writeSz = write(mpv_socket_fdSelect, data, strlen(data));
  printf("Write Size: %d\n", writeSz);
  if (writeSz != strlen(data)) {
    printf("Bad MPV Write\n");
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
    // printf("MPV Singlet: %s\n", prop);

    char* data_tmp = "{ \"command\": [\"get_property\", \"%s\"] }\n";
    size_t len = snprintf(NULL, 0, data_tmp, prop) + 1;
    char *data = (char*)malloc(len * sizeof(char));
    if (data == NULL) {
        printf("%s\n%s\n", "Error!, No Memory", strerror(errno));
        return -1;
    }
    snprintf(data, len, data_tmp, prop);

    int result = -1;

    pthread_mutex_lock(&mpvSocketAccessLock);
  
    if (!mpv_fd_write(data)) {
        goto cleanup;
    }
    free(data);
  
    while (1) {
        /* select returns 0 if timeout, 1 if input available, -1 if error. */
        int selT = select(FD_SETSIZE, &mpv_socket_set, NULL, NULL, &mpv_socket_timeout);
        if (selT == -1) {
            printf("%s\n%s\n","Error! Closing MPV Socket, SELECT -1", strerror(errno));
            mpv_socket_close(mpv_socket_fdSelect);
            mpv_socket_fdSelect = -1;
            goto cleanup;
        } else if (selT == 1) {
            char* mpv_rpc_ret = NULL;
            int rc = sgetline(mpv_socket_fdSelect, &mpv_rpc_ret);
            if (rc > 0 && mpv_rpc_ret != NULL) {
                // error response
                if (strncmp(mpv_rpc_ret, "{\"error\":", 9) == 0) {
                    printf("%s\n%s\n%s\n","Error!", mpv_rpc_ret, strerror(errno));
                    free(mpv_rpc_ret);
                    goto cleanup;
                    // data response
                } else if (strncmp(mpv_rpc_ret, "{\"data\":", 8) == 0) {
                    result = ta_json_parse(mpv_rpc_ret, "data", json_prop);
                    free(mpv_rpc_ret);
                    goto cleanup;
                    // every other response
                } else {
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




int mpv_set_prop_char(char* prop, char* prop_val) {
  char* data_tmp = "set %s %s\n";
  size_t dataSz = snprintf(NULL, 0, data_tmp, prop, prop_val) + 1;
  char *data = (char*)calloc(dataSz, sizeof(char));
  if (data == NULL) { return -1; }
  snprintf(data, dataSz, data_tmp, prop, prop_val);
  return mpv_cmd(data);
}

int mpv_set_prop_int(char* prop, int prop_val) {
  char* data_tmp = "set %s %d\n";
  size_t dataSz = snprintf(NULL, 0, data_tmp, prop, prop_val) + 1;
  char *data = (char*)calloc(dataSz, sizeof(char));
  if (data == NULL) { return -1; }
  snprintf(data, dataSz, data_tmp, prop, prop_val);
  return mpv_cmd(data);
}

int mpv_set_prop_double(char* prop, double prop_val) {
  char* data_tmp = "set %s %f\n";
  size_t dataSz = snprintf(NULL, 0, data_tmp, prop, prop_val) + 1;
  char *data = (char*)calloc(dataSz, sizeof(char));
  if (data == NULL) { return -1; }
  snprintf(data, dataSz, data_tmp, prop, prop_val);
  return mpv_cmd(data);
}

int mpv_cmd_prop_val(char* cmd, char* prop, double prop_val) {
  char* data_tmp = "%s %s %f\n";
  size_t dataSz = snprintf(NULL, 0, data_tmp, cmd, prop, prop_val) + 1;
  char *data = (char*)calloc(dataSz, sizeof(char));
  if (data == NULL) { return -1; }
  snprintf(data, dataSz, data_tmp, cmd, prop, prop_val);
  return mpv_cmd(data);
}



int mpv_seek(double distance) {
  char* data_tmp = "seek %f\n";
  size_t dataSz = snprintf(NULL, 0, data_tmp, distance) + 1;
  char *data = (char*)calloc(dataSz, sizeof(char));
  if (data == NULL) { return -1; }
  snprintf(data, dataSz, data_tmp, distance);
  return mpv_cmd(data);
}

int mpv_seek_arg(double distance, char* flags) {
  char* data_tmp = "seek %f %s\n";
  size_t dataSz = snprintf(NULL, 0, data_tmp, distance, flags) + 1;
  char *data = (char*)calloc(dataSz, sizeof(char));
  if (data == NULL) { return -1; }
  snprintf(data, dataSz, data_tmp, distance, flags);
  return mpv_cmd(data);
}



int mpv_pause() {
  size_t cmdSz = strlen("set pause yes\n") + 1;
  char *cmd = (char*)malloc(cmdSz * sizeof(char));
  strlcpy(cmd, "set pause yes\n", cmdSz);
  return mpv_cmd(cmd);
}

int mpv_play() {
  size_t cmdSz = strlen("set pause no\n") + 1;
  char *cmd = (char*)malloc(cmdSz * sizeof(char));
  strlcpy(cmd, "set pause no\n", cmdSz);
  return mpv_cmd(cmd);
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
  int cmdStopSz = strlen("stop\n") + 1;
  char *cmdStop = malloc(cmdStopSz);
  strlcpy(cmdStop, "stop\n", cmdStopSz);
  mpv_cmd(cmdStop);
}

void mpv_playlist_clear() {
  int cmdClearSz = strlen("playlist-clear\n") + 1;
  char *cmdClear = malloc(cmdClearSz);
  strlcpy(cmdClear, "playlist-clear\n", cmdClearSz);
  mpv_cmd(cmdClear);
}

int mpv_loadfile(char* app, char* folder, char* filename, char* flag, char* opts) {
  char* data_tmp = "loadfile /home/pi/shared/%s/%s/%s %s %s\n";
  size_t dataSz = snprintf(NULL, 0, data_tmp, app, folder, filename, flag, opts) + 1;
  char *data = (char*)calloc(dataSz, sizeof(char));
  if (data == NULL) { return -1; }
  snprintf(data, dataSz, data_tmp, app, folder, filename, flag);
  return mpv_cmd(data);
}

void mpv_quit() {
  size_t dataSz = strlen("quit\n") + 1;
  char *data = (char*)calloc(dataSz, sizeof(char));
  strlcpy(data, "quit\n", dataSz);
  mpv_cmd(data);
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
