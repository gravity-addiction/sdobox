
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <math.h>

#include "libs/dbg/dbg.h"
#include "libs/shared.h"
#include "audio.h"


char *audio_card = "default";
char *audio_selem_name = "Headphone";


void audio_closeHandle(snd_mixer_t **audio_handle) {
  snd_mixer_close(*audio_handle);
}

void audio_openHandle(char *card, snd_mixer_t **audio_handle) {
  snd_mixer_open(audio_handle, 0);
  snd_mixer_attach(*audio_handle, card);
  snd_mixer_selem_register(*audio_handle, NULL, NULL);
  snd_mixer_load(*audio_handle);
}

snd_mixer_elem_t* audio_findSelem(char *selem_name, snd_mixer_t **audio_handle, snd_mixer_selem_id_t **audio_sid) {
  snd_mixer_selem_id_alloca(audio_sid);
  snd_mixer_selem_id_set_index(*audio_sid, 0);
  snd_mixer_selem_id_set_name(*audio_sid, selem_name);
  return snd_mixer_find_selem(*audio_handle, *audio_sid);
}


// Maybe initialize with a timeout for multiple volume changes in short time
void volume_setVolume(long volume) {
  long min, max, cur;
  float new, newc;
  snd_mixer_t *audio_handle;
  snd_mixer_selem_id_t *audio_sid;

  audio_openHandle(audio_card, &audio_handle);

  snd_mixer_elem_t* elem = audio_findSelem(audio_selem_name, &audio_handle, &audio_sid);
  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &cur);
  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

  // using logarithmic calucations for volume slider 0 - 1.6 amplitude
  newc = 4550 * log(volume) / 230.2;
  new = min + (((max - min) * newc) / 100);

  // Negative Min, Positive Max
  if (new < min) {
    volume_cur = min;
  } else if (new > max) {
    volume_cur = max;
  } else {
    volume_cur = new;
  }

  snd_mixer_selem_set_playback_volume_all(elem, volume_cur);

  audio_closeHandle(&audio_handle);
}

// Maybe initialize with a timeout for multiple volume changes in short time
int volume_getVolume(long * volume) {
  long volume_new;
  snd_mixer_t *audio_handle;
  snd_mixer_selem_id_t *audio_sid;
  audio_openHandle(audio_card, &audio_handle);

  snd_mixer_elem_t* elem = audio_findSelem(audio_selem_name, &audio_handle, &audio_sid);
  if (elem == NULL) {
    dbgprintf(DBG_ERROR, "Not able to find audio device: %s\n", audio_selem_name);
    return 0;
  }

  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume_new);

  audio_closeHandle(&audio_handle);

  if (volume_new == volume_cur) {
    *volume = volume_cur;
    return 0;
  } else {
    volume_cur = volume_new;
    *volume = volume_cur;
    return 1;
  }
}

void volume_incrase() {
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

double volume_logLinear(double x) {
 // printf("R: %f, P: %f\n", x, ((pow(10, ((x * .01) / 20)) * 1000) / 10));

//  return ((pow(10, (x / 4500)) * 10) / 16);
  return ((pow(10, ((x * .01) / 20)) * 1000) / 10);
}
