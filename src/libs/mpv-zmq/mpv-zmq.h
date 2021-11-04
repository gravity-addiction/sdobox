#ifndef _MPV_ZMQ_H_
#define _MPV_ZMQ_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "./mpv-cache.h"

struct lib_mpv_cache *libmpvCache;

int libmpv_zmq_init();
void libmpv_zmq_destroy();

int libmpv_zmq_timeserver_init(void **qfive, char *url);

int libmpv_zmq_async_init();
int libmpv_zmq_request_init();
int libmpv_zmq_raw_init();

// uint64_t libmpv_zmq_cmd_async(char* question, void *cb);
int libmpv_zmq_cmd_w_reply(char* question, char** response);
int libmpv_zmq_cmd(char* question);
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

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_ZMQ_H_