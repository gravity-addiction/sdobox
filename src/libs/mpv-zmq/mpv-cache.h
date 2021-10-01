#ifndef _MPV_PLAYER_H_
#define _MPV_PLAYER_H_

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

struct lib_mpv_cache {
  struct lib_mpv_player *player;
};
struct lib_mpv_cache *libMpvCache;

struct lib_mpv_player * LIBMPV_PLAYER_INIT();
void LIBMPV_PLAYER_DESTROY();

struct lib_mpv_cache * LIBMPV_CACHE_INIT();
void LIBMPV_CACHE_DESTROY();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_PLAYER_H_
