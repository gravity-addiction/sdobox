#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <jansson.h>
#include <libconfig.h>

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


struct lib_mpv_server * LIBMPV_SERVER_INIT() {
  struct lib_mpv_server *server = (struct lib_mpv_server*)malloc(sizeof(struct lib_mpv_server));

  return server;
}
void LIBMPV_SERVER_DESTROY(struct lib_mpv_server *server) {
  free(server);
}


struct lib_mpv_cache * LIBMPV_CACHE_INIT() {
  struct lib_mpv_cache *cache = (struct lib_mpv_cache*)malloc(sizeof(struct lib_mpv_cache));
  cache->player = LIBMPV_PLAYER_INIT();
  cache->server = LIBMPV_SERVER_INIT();

  return cache;
}
void LIBMPV_CACHE_DESTROY(struct lib_mpv_cache *cache) {
  if (cache->player != NULL) {
    LIBMPV_PLAYER_DESTROY(cache->player);
  }
  if (cache->server != NULL) {
    LIBMPV_SERVER_DESTROY(cache->server);
  }  
  free(cache);
}
/*
int libmpz_cache_create_videoJson(char *json_data) {
  json_t *root = json_object();
  json_t *json_arr = json_array();
  
  json_object_set_new( root, "destID", json_integer( 1 ) );
  json_object_set_new( root, "command", json_string("enable") );
  json_object_set_new( root, "respond", json_integer( 0 ));
  json_object_set_new( root, "data", json_arr );
  
  json_array_append( json_arr, json_integer(11) );
  json_array_append( json_arr, json_integer(12) );
  json_array_append( json_arr, json_integer(14) );
  json_array_append( json_arr, json_integer(9) );
  
  char *s = json_dumps(root, 0);
  
  puts(s);
  json_decref(root);
  return 1;
}


int libmpz_cache_parse_videoJson(char *json_data) {
  json_error_t error;
  // parse text into JSON structure
  json_t *root = load_json(json_data);

  if (root) {
    // print and release the JSON structure
    print_json(root);
    json_decref(root);
    return 0;
  }

  return 1;
}
*/
