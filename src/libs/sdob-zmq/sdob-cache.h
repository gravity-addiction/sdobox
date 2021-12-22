#ifndef _SDOB_CACHE_H_
#define _SDOB_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct libsdob_server {
  char* scoringserver;
  char* eventserver;
  char* roomid;
};

struct libsdob_cache {
  struct libsdob_server *server;
};
struct libsdob_cache *libSdobCache;

struct libsdob_server * LIBSDOB_SERVER_INIT();
void LIBSDOB_SERVER_DESTROY(struct libsdob_server *server);

struct libsdob_cache * LIBSDOB_CACHE_INIT();
void LIBSDOB_CACHE_DESTROY(struct libsdob_cache *cache);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _SDOB_CACHE_H_
