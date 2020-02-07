#ifndef _MPV_H_
#define _MPV_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "GUIslice-wrapper/GUIslice-wrapper.h"

char* mpv_socket_path;
char* mpv_fifo_path;
int mpv_socket_fd;

double mpv_video_duration; // = 0.00;
double mpv_video_pos; // = 0.00;
double mpv_video_rate; // = 1.0;

int mpv_singlet_lock;

long long int mi_video_fps_frame; // = 0;



int       i_video_pos;
double    mi_video_fps; // = 0;
int       m_video_lock; // = 0;
int       m_is_video_playing; // = 0;



uint16_t  m_nPosPlayer; // = 0;

int       i_chapter_cnt; // placeholder for chapter count

double    m_video_percent_new;
double    m_video_percent_tmp;

int mpv_socket_fdSelect;
fd_set mpv_socket_set;
struct timeval mpv_socket_timeout;

// ------------------------------
// mpv.c

int mpv_socket_conn();
void mpv_socket_close(int fd);
int mpv_init(gslc_tsGui *pGui);

int mpv_create_player(char* filePath);
int mpv_fd_write(char *data);
int mpvSocketSinglet(char* prop, char ** json_prop);

// void mpv_setCallbackDouble(int evt, void (*function)(double));
// void mpv_runCallbackDouble(int evt, double spd);

int mpv_cmd(char *cmd_string);
int mpv_cmd_str(char* cmd_string);
int mpv_set_prop_char(char* prop, char* prop_val);
int mpv_set_prop_int(char* prop, int prop_val);
int mpv_set_prop_double(char* prop, double prop_val);
int mpv_cmd_prop_val(char* cmd, char* prop, double prop_val);

int mpv_seek(double distance);
int mpv_seek_arg(double distance, char* flags);

int mpv_pause();
int mpv_play();

double mpv_speed(double spd);
double mpv_speed_adjust(double spd);

int mpv_loadfile(char* folder, char* filename, char* flag, char* opts);
void mpv_quit();
void stop_video();




// https://mpv.io/manual/stable/#terminal-status-line
// The audio/video sync as A-V:  0.000. This is the difference between audio
// and video time. Normally it should be 0 or close to 0. If it's growing, it
// might indicate a playback problem. (avsync property.)
// Total A/V sync change, e.g. ct: -0.417. Normally invisible. Can show up if
// there is audio "missing", or not enough frames can be dropped. Usually this
// will indicate a problem. (total-avsync-change property.)
int check_avsync();
int video_display_fps();
int video_estimated_display_fps();


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_H_