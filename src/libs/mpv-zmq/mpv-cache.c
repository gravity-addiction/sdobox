#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <jansson.h>

#include "./mpv-cache.h"

struct lib_mpv_player * LIBMPV_PLAYER_INIT() {
  struct lib_mpv_player *player = (struct lib_mpv_player*)malloc(sizeof(struct lib_mpv_player));

  player->cnt = 0;
  player->has_file = 0;
  player->is_loaded = 0;
  player->is_playing = 0;
  player->is_seeking = 0;
  player->position = 0.00;
  player->duration = 0.00;
  player->pbrate = 1.00;
  player->pbrateUser = 0.75;
  player->pbrateStr = (char*)malloc(10 * sizeof(char));
  player->pbrateStr[0] = '\0';
  player->pbrateUserStr = (char*)malloc(10 * sizeof(char));
  player->pbrateUserStr[0] = '\0';
  player->positionStr = (char*)malloc(32 * sizeof(char));
  player->positionStr[0] = '\0';
  player->folder = (char*)malloc(1024 * sizeof(char));
  player->folder[0] = '\0';
  player->file = (char*)malloc(512 * sizeof(char));
  player->file[0] = '\0';

  return player;
}
void LIBMPV_PLAYER_DESTROY(struct lib_mpv_player *player) {
  free(player->positionStr);
  free(player->pbrateStr);
  free(player->pbrateUserStr);
  free(player->folder);
  free(player->file);  
  free(player);
}



struct lib_mpv_cache * LIBMPV_CACHE_INIT() {
  struct lib_mpv_cache *cache = (struct lib_mpv_cache*)malloc(sizeof(struct lib_mpv_cache));
  cache->player = LIBMPV_PLAYER_INIT();

  return cache;
}
void LIBMPV_CACHE_DESTROY(struct lib_mpv_cache *cache) {
  if (cache->player != NULL) {
    LIBMPV_PLAYER_DESTROY(cache->player);
  }
  free(cache);
}
