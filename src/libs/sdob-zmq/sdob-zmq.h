#ifndef _SDOB_ZMQ_H_
#define _SDOB_ZMQ_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct libsdob_question {
  void* server;
  char* question;
};
struct libsdob_question * LIBSDOB_QUESTION_INIT(char* question);
void LIBSDOB_QUESTION_DESTROY(struct libsdob_question *sdobQuestion);

int libsdob_eventThreadKill;
int libsdob_eventThreadRunning;

int libsdob_zmq_init();
void libsdob_zmq_destroy();

int libsdob_zmq_scoring_send(char* scorecard);

int libsdob_eventThreadStart();
void libsdob_eventThreadStop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _SDOB_ZMQ_H_
