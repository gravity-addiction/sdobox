#ifndef _AUDIO_H_
#define _AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void audio_closeHandle();

long volume_cur;
void volume_setVolume(long ssvolume);
int volume_getVolume(long * volume);

void volume_increase();
void volume_decrease();
void volume_mute();

double volume_logLinear(double x);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _AUDIO_H_