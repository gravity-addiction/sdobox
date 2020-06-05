
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <math.h>

#include "libs/shared.h"
#include "audio.h"

// Maybe initialize with a timeout for multiple volume changes in short time
void volume_setVolume(long volume) {
  long min, max, cur;
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  const char *card = "default";
  const char *selem_name = "PCM";

  snd_mixer_open(&handle, 0);
  snd_mixer_attach(handle, card);
  snd_mixer_selem_register(handle, NULL, NULL);
  snd_mixer_load(handle);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, selem_name);
  snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
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

  snd_mixer_close(handle);
}

// Maybe initialize with a timeout for multiple volume changes in short time
long volume_getVolume() {
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  const char *card = "default";
  const char *selem_name = "PCM";

  snd_mixer_open(&handle, 0);
  snd_mixer_attach(handle, card);
  snd_mixer_selem_register(handle, NULL, NULL);
  snd_mixer_load(handle);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, selem_name);
  snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume_cur);

  snd_mixer_close(handle);
  return volume_cur;
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