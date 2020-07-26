#ifndef _AUDIO_H_
#define _AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void audio_closeHandle();

long volume_min;
long volume_max;
long volume_cur;

void volume_setVolume(long ssvolume);
void volume_setVolumeDb(long db);
int volume_getVolume(long * volume, long * dbGain);
void volume_setPercent(long volPercent);
void volume_increase();
void volume_decrease();
void volume_mute();

void volume_range(long *min, long *max, long *cur);
void volume_range_get();


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _AUDIO_H_