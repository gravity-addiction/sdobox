#ifndef _MPV_ZMQ_H_
#define _MPV_ZMQ_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "libs/mpv-wrapper/mpv-cache.h"

int libmpv_zmq_init();
void libmpv_zmq_destroy();

int libmpv_zmq_async_init();

// uint64_t libmpv_zmq_cmd_async(char* question, void *cb);
int libmpv_zmq_cmd_w_reply(char* question, char** response);
int libmpv_zmq_cmd(char* userCmd);
char* libmpv_zmq_fmt_cmd(char* fmt, ...);

char* libmpv_zmq_get_prop_string(char* prop);
int64_t libmpv_zmq_get_prop_int64(char* prop);
double libmpv_zmq_get_prop_double(char* prop);
int libmpv_zmq_get_prop_flag(char* prop);

int libmpv_zmq_set_prop_char(char* prop, char* prop_val);
int libmpv_zmq_set_prop_int(char* prop, int prop_val);
int libmpv_zmq_set_prop_double(char* prop, double prop_val);
int libmpv_zmq_set_prop_flag(char* prop, char* prop_val);
int libmpv_zmq_cmd_prop_val(char* cmd, char* prop, double prop_val);

void *pg_mpvEvents; // ZMQ mpvEventQueue
void pg_mpv_add_event(struct queue_head **head);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_ZMQ_H_
