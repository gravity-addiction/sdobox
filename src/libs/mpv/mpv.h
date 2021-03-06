#ifndef _MPV_H_
#define _MPV_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct mpv_cmd_status {
  int resultReqId;
  int resultError;
  int resultData;
  char *jsonReqId;
  char *jsonError;
  char *jsonData;
};
struct mpv_cmd_status mpvSocketCmdStatus;

typedef union {
  void* ptr;
  int hasPtr;
  int integer;
  unsigned uinteger;
  double floating;
} mpv_any_u;

char* mpv_socket_path;
char* mpv_fifo_path;
int mpv_socket_fd;

int mpv_socket_fdSelect;
fd_set mpv_socket_set;
struct timeval mpv_socket_timeout;
unsigned int mpv_socket_lastConn;

// ------------------------------
// mpv.c
mpv_any_u * MPV_ANY_U();
void MPV_ANY_U_FREE(mpv_any_u *mpvu);

int mpv_socket_conn();
void mpv_socket_close(int fd);
int mpv_init();
void mpv_destroy();

int mpv_create_player(char* filePath);
int mpv_fd_write(char *data);
int mpvSocketSinglet(char* prop, mpv_any_u ** json_prop);

// void mpv_setCallbackDouble(int evt, void (*function)(double));
// void mpv_runCallbackDouble(int evt, double spd);

int mpv_cmd(char *cmd_string);  /* sends and frees argument */

int mpv_fmt_cmd(char* fmt, ...); /* formats and sends, nothing passed in is freed */

int mpv_set_prop_char(char* prop, char* prop_val);
int mpv_set_prop_int(char* prop, int prop_val);
int mpv_set_prop_double(char* prop, double prop_val);
int mpv_cmd_prop_val(char* cmd, char* prop, double prop_val);

int mpv_seek(double distance);
int mpv_seek_arg(double distance, char* flags);

void mpv_check_pause();
int mpv_pause();
int mpv_play();
int mpv_playpause_toggle();
int mpv_fullscreen(int fs);

double mpv_speed(double spd);
double mpv_speed_adjust(double spd);

void mpv_stop();
void mpv_playlist_clear();
int mpv_loadfile(char* folder, char* filename, char* flag, char* opts);
void mpv_quit();
void stop_video();

// https://mpv.io/manual/stable/#terminal-status-line
// The audio/video sync as A-V:  0.000. This is the difference between audio
// and video time. Normally it should be 0 or close to 0. If it's growing, it
// might indicate a playback problem. (avsync property.)
// Total A/V sync change, e.g. ct: -0.417. Normally invisible. Can show up if
// there is audio "missing", or not enough frames can be dropped. Usually this
// will indicate a problem. (total-avsync-change property.)
int check_avsync();
int video_display_fps();
int video_estimated_display_fps();

int mpv_volume_mute();
int mpv_volume_on();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_H_
