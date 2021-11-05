#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

#include "libs/zhelpers/zhelpers-conn.h"
#include "libs/zhelpers/zhelpers-tx.h"
#include "sdob-cache.h"

struct libsdob_server * LIBSDOB_SERVER_INIT() {
  struct libsdob_server *server = (struct libsdob_server*)malloc(sizeof(struct libsdob_server));
  return server;
}
void LIBSDOB_SERVER_DESTROY(struct libsdob_server *server) {
  free(server);
}

struct libsdob_cache * LIBSDOB_CACHE_INIT() {
  struct libsdob_cache *cache = (struct libsdob_cache*)malloc(sizeof(struct libsdob_cache));
  cache->server = LIBSDOB_SERVER_INIT();

  return cache;
}
void LIBSDOB_CACHE_DESTROY(struct libsdob_cache *cache) {
  if (cache->server != NULL) {
    LIBSDOB_SERVER_DESTROY(cache->server);
  }  
  free(cache);
}
