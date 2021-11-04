#ifndef _SDOB_ZMQ_H_
#define _SDOB_ZMQ_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


int libsdob_zmq_init();
void libsdob_zmq_destroy();
int libsdob_zmq_scoring_init();
void * libsdob_zmq_events_init();
int libsdob_zmq_scoring_send(char* question, char** response);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _SDOB_ZMQ_H_
