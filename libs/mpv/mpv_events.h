#ifndef _LIB_MPV_EVENTS_H_
#define _LIB_MPV_EVENTS_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Each callback function
struct libMpvEventThreadCb {
  int id;
  void (*cb)(char*);
};

// collection of callback functions
struct libMpvEventThreadCbData {
  int max;
  int len;
  struct libMpvEventThreadCb **cbs;
};
struct libMpvEventThreadCbData *libMpvEventThreads;

// information of the callback sent to the thread.
struct libMpvEventThreadData {
  char* event;
  void (*cb)(char*);
};

int libMpvSocketThreadKill;
int libMpvSocketThreadRunning;


struct libMpvEventThreadCbData * LIBMPV_EVENTS_INIT_DATA();
void LIBMPV_EVENTS_DESTROY_DATA(struct libMpvEventThreadCbData *threads);
void libMpvCallbackClean(struct libMpvEventThreadCbData *threads);
void* libMpvCallbackFunc(void* targ);
int libMpvCallbackAppend(void (*function)(char*));

int libMpvSocketThreadStart();
void libMpvSocketThreadStop();

#endif // _LIB_MPV_EVENTS_H_