#ifndef _AUDIO_H_
#define _AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

long volume_cur;
void volume_setVolume(long volume);
long volume_getVolume();

void volume_increase();
void volume_decrease();
void volume_mute();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _AUDIO_H_