#ifndef _MPV_ZMQ_HELPER_H_
#define _MPV_ZMQ_HELPER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "mpv-zmq.h"

int mpv_seek(double distance);
int mpv_seek_arg(double distance, char* flags);

void mpv_check_pause();
int mpv_pause();
int mpv_play();
int mpv_playpause_toggle();
int mpv_fullscreen(int fs);

double mpv_speed(double spd);
double mpv_speed_adjust(double spd);

void mpv_stop();
void mpv_playlist_clear();

static char* quotify(char* original, char** saved_replacement);
int mpv_loadfile(char* folder, char* filename, char* flag, char* opts);
void mpv_quit();
void stop_video();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_ZMQ_HELPER_H_
