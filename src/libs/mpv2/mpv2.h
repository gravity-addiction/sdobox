#ifndef _MPV2_H_
#define _MPV2_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

unsigned int reqId;
unsigned int reqTop;

struct mpv_conn {
  char* socket_path;
  int fdSelect; // = -1;
  fd_set fdSet;
  struct timeval timeout; // = (struct timeval){0};
  unsigned int lastConn;
  int connected;
  pthread_mutex_t cmdLock;
  int reqId;
  int reqQueCnt;
  int reqQueI[100];
  char reqQue[100][32];
  void (*reqQueCb[100])(struct mpv_conn*, void*);
};

struct libMpv2PlayerInfo {
  unsigned long int cnt;
  int has_file;
  int is_loaded;
  int is_playing;
  int is_seeking;
  int has_seeked;
  double position;
  double duration;
  double fps;
  double pbrate;
  double pbrateUser;
  char *positionStr;
  char *pbrateStr;
  char *pbrateUserStr;
  char *folder;
  char *file;
};
struct libMpv2PlayerInfo *libMpvVideoInfo;

struct libMpv2PlayerInfo * LIBMPV2_PLAYER_INFO_INIT();
void LIBMPV2_PLAYER_INFO_DESTROY(struct libMpv2PlayerInfo *eventInfo);

int libmpv2_init();
void libmpv2_destroy();


int mpv_cmd_w_reply(char* question, char** response);
int mpv_cmd(char* question);
char* mpv_fmt_cmd(char* fmt, ...);

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

static char* quotify(char* original, char** saved_replacement);
int mpv_loadfile(char* folder, char* filename, char* flag, char* opts);
void mpv_quit();
void stop_video();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV2_H_
