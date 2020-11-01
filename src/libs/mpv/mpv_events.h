#ifndef _LIB_MPV_EVENTS_H_
#define _LIB_MPV_EVENTS_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// Each callback function
struct libMpvEventsCb {
  unsigned long int id;
  void (*cb)(char*);
};

// collection of callback functions
struct libMpvEventsCbData {
  int max;
  int len;
  unsigned long int idC;
  struct libMpvEventsCb **cbs;
};
struct libMpvEventsCbData *libMpvEventsCbList;

// information of the callback sent to the thread.
struct libMpvEventsData {
  char* event;
  void (*cb)(char*);
};



int libMpvQueueThreadKill;
int libMpvQueueThreadRunning;

int libMpvSocketThreadKill;
int libMpvSocketThreadRunning;

struct queue_root *libMpvEvents_Queue;
size_t libMpvEvents_QueueLen;


struct libMpvEventsCbData * LIBMPV_EVENTS_INIT_DATA();
void LIBMPV_EVENTS_DESTROY_DATA(struct libMpvEventsCbData *cbList);
void libMpvCallbackClean(struct libMpvEventsCbData *cbList);
unsigned long int libMpvCallbackAppend(void (*function)(char*));
void libMpvCallbackRemove(unsigned long int cbId);
// void* libMpvCallbackFunc(void* targ);



int libMpvQueueThreadStart();
void libMpvQueueThreadStop();

int libMpvSocketThreadStart();
void libMpvSocketThreadStop();

void libmpv_setduration();

#endif // _LIB_MPV_EVENTS_H_