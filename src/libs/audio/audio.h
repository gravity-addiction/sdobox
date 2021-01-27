#ifndef _AUDIO_H_
#define _AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

char *audio_card;
char *audio_selem_name;
long volume_min;
long volume_max;
long volume_cur;
long volume_new;

int volume_move_debounce;
int volume_move_delay;

void volume_debounceCheck();

// Convert volume value from log db to scale percentage
void volume_dbToPercent(long volValue, long volMin, long volMax, long * volPercent);

// Set playback volume control for persistant handle
void volume_setVolume(long volume);
void volume_setPercent(long volPercent);

int volume_getVolume(char* card, char* device, long * dbGain);
int volume_getVolumeRange(char* card, char* device, long * dbMin, long *dbMax);

void volume_incrase();
void volume_decrease();




#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _AUDIO_H_