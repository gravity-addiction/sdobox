#ifndef _MPV_WRAPPER_H_
#define _MPV_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <jansson.h>

#include "libs/shared.h"
#include "libs/dbg/dbg.h"
#include "libs/fbbg/fbbg.h"
#include "libs/queue/queue.h"
#include "libs/zhelpers/zhelpers.h"

#include "libs/mpv-zmq/mpv-zmq.h"
#include "libs/mpv-socket/mpv-socket.h"
#include "./mpv-wrapper.h"
#include "./mpv-cache.h"

int libmpv_wrapper_init();
double mpv_calc_marktime(struct lib_mpv_player *player);
int mpv_seek(double distance);
int mpv_seek_arg(double distance, char* flags);
void mpv_check_pause();
int mpv_pause();
int mpv_play();
int mpv_fullscreen(int fs);
int mpv_volume_mute();
int mpv_volume_on();
int mpv_playpause_toggle();
// MPV Player Speed
double mpv_speed(double spd);
double mpv_speed_adjust(double spd);
void mpv_stop();
void mpv_playlist_clear();
static char* quotify(char* original, char** saved_replacement);
int mpv_loadfile(char* folder, char* filename, char* flag, char* opts);
int mpv_loadurl(char* url, char* flag, char* opts);
void mpv_quit();
void stop_video();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_WRAPPER_H_
