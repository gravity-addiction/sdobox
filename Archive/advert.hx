#ifndef _ADVENTAPP_H_
#define _ADVENTAPP_H_


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ------------------------------------------------
int SLIDESHOW_DELAY = 10; // Delay between photos in seconds


// Program Globals
// ------------------------------------------------
int      m_bQuit = 0;
int      m_bSigInt = 0;

char*    m_clean_dotfiles = "find /home/pi/shared -type f -name \".*\" -exec rm \"{}\" \\;";

unsigned int r_a = 0;
unsigned int r_b = 0;
unsigned int r_seq_a;
unsigned int r_seq_b;
unsigned int r_l = 0;
unsigned int r_l_hold = 0;
unsigned int r_r = 0;
unsigned int r_r_hold = 0;

pthread_t pid_tmp_inotify;
pthread_t pid_slideshow;
int slideshow_interval = 10;
pid_t pid_post_image = -1;

char **files;
size_t file_count;
int files_i = 0;
int files_r = 0;
int c_rotary = 0;
int ss_delay = 0;

int i_pause = 0;

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _ADVENTAPP_H_