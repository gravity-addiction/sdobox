
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <math.h>

#include "libs/shared.h"
#include "audio.h"


snd_mixer_t *audio_handle;
snd_mixer_selem_id_t *audio_sid;
char *audio_card = "default";
char *audio_selem_name = "PCM";
int audio_handled = 0;


void audio_closeHandle() {
  if (audio_handled == 0) { return; }
  snd_mixer_close(audio_handle);
  audio_handled = 0;
}

void audio_openHandle(char *card) {
  if (audio_handled == 1) { return; }
  snd_mixer_open(&audio_handle, 0);
  snd_mixer_attach(audio_handle, card);
  snd_mixer_selem_register(audio_handle, NULL, NULL);
  snd_mixer_load(audio_handle);
  audio_handled = 1;
}

snd_mixer_elem_t* audio_findSelem(char *selem_name) {
  snd_mixer_selem_id_alloca(&audio_sid);
  snd_mixer_selem_id_set_index(audio_sid, 0);
  snd_mixer_selem_id_set_name(audio_sid, selem_name);
  return snd_mixer_find_selem(audio_handle, audio_sid);
}


// Maybe initialize with a timeout for multiple volume changes in short time
void volume_setVolume(long volume) {
  long min, max, cur;

  audio_openHandle(audio_card);
  snd_mixer_elem_t* elem = audio_findSelem(audio_selem_name);

  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &cur);
  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
  // Negative Min, Positive Max
  if (volume < min) {
    volume_cur = min;
  } else if (volume > max) {
    volume_cur = max;
  } else {
    volume_cur = volume;
  }
  snd_mixer_selem_set_playback_dB_all(elem, volume_cur, (volume_cur > 0) ? 1 : 0);

  audio_closeHandle();
}

// Maybe initialize with a timeout for multiple volume changes in short time
int volume_getVolume(long * volume) {
  long volume_new;

  audio_openHandle(audio_card);
  snd_mixer_elem_t* elem = audio_findSelem(audio_selem_name);

  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume_new);
  audio_closeHandle();

  if (volume_new == volume_cur) {
    *volume = volume_cur;
    return 0;
  } else {
    volume_cur = volume_new;
    *volume = volume_cur;
    return 1;
  }
}

void volume_increase() {
  size_t cmdSz = strlen("amixer -c 0 set PCM 2db+") + 1;
  char *fullpath = malloc(cmdSz);
  snprintf(fullpath, cmdSz, "%s", "amixer -c 0 set PCM 2db+");
  // run_system_cmd(fullpath);
  free(fullpath);
}

void volume_decrease() {
  size_t cmdSz = strlen("amixer -c 0 set PCM 2db-") + 1;
  char *fullpath = malloc(cmdSz);
  snprintf(fullpath, cmdSz, "%s", "amixer -c 0 set PCM 2db-");
  // run_system_cmd(fullpath);
  free(fullpath);
}
