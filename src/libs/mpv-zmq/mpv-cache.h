#ifndef _MPV_CACHE_H_
#define _MPV_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct lib_mpv_player {
  unsigned long int cnt;
  int has_file;
  int is_loaded;
  int is_playing;
  int is_seeking;
  int has_seeked;
  int64_t position_update;
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
  char *url;
};

struct lib_mpv_server {
  char* videoserver;
  char* cmdserver;
  char* reqserver;
};

struct lib_mpv_cache {
  struct lib_mpv_player *player;
  struct lib_mpv_server *server;
};
struct lib_mpv_cache *libMpvCache;

struct lib_mpv_player * LIBMPV_PLAYER_INIT();
void LIBMPV_PLAYER_DESTROY(struct lib_mpv_player *player);

struct lib_mpv_server * LIBMPV_SERVER_INIT();
void LIBMPV_SERVER_DESTROY(struct lib_mpv_server *server);

struct lib_mpv_cache * LIBMPV_CACHE_INIT();
void LIBMPV_CACHE_DESTROY(struct lib_mpv_cache *cache);

// void libmpz_cache_create_videoJson();
int libmpz_cache_parse_videoJson(char *json_data);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_CACHE_H_
