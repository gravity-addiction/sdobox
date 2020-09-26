#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <math.h>

#include "libs/dbg/dbg.h"
#include "libs/shared.h"
#include "audio.h"


// Default audio server (default / Master when pulseaudio is available, hw:0 / PCM for db readings)
char *audio_card = "hw:0";
char *audio_selem_name = "PCM";
snd_mixer_t *volume_audio_handle;
snd_mixer_selem_id_t *volume_audio_sid;
snd_mixer_elem_t* volume_elem;


int volume_no_device = 0; // Lock audio handle when incorrect device is detected


///////////////////////
// Audio core creation and destruction of audio alsa handles
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
//
////////////////////////


////////////////////////
// Volume persistant handle creation
int volume_handleOpen = 0;
void volume_closeAudio() {
  audio_closeHandle(&volume_audio_handle);
  volume_handleOpen = 0;
}

int volume_initAudio(int runTest) {
  if (volume_elem && runTest == 1) { // Test Handle
    if (!snd_mixer_selem_is_active(volume_elem)) {
      volume_closeAudio();
    }
  }

  if (volume_handleOpen == 0) {
    /*
  int val;

  printf("ALSA library version: %s\n",
          SND_LIB_VERSION_STR);

  printf("\nPCM stream types:\n");
  for (val = 0; val <= SND_PCM_STREAM_LAST; val++)
    printf("  %s\n",
      snd_pcm_stream_name((snd_pcm_stream_t)val));

  printf("\nPCM access types:\n");
  for (val = 0; val <= SND_PCM_ACCESS_LAST; val++)
    printf("  %s\n",
      snd_pcm_access_name((snd_pcm_access_t)val));

  printf("\nPCM formats:\n");
  for (val = 0; val <= SND_PCM_FORMAT_LAST; val++)
    if (snd_pcm_format_name((snd_pcm_format_t)val)
      != NULL)
      printf("  %s (%s)\n",
        snd_pcm_format_name((snd_pcm_format_t)val),
        snd_pcm_format_description(
                           (snd_pcm_format_t)val));

  printf("\nPCM subformats:\n");
  for (val = 0; val <= SND_PCM_SUBFORMAT_LAST;
       val++)
    printf("  %s (%s)\n",
      snd_pcm_subformat_name((
        snd_pcm_subformat_t)val),
      snd_pcm_subformat_description((
        snd_pcm_subformat_t)val));

  printf("\nPCM states:\n");
  for (val = 0; val <= SND_PCM_STATE_LAST; val++)
    printf("  %s\n",
           snd_pcm_state_name((snd_pcm_state_t)val));
  */

  /*
  int rc;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val, val2;
  int dir;
  snd_pcm_uframes_t frames;

  // Open PCM device for playback.
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  // Allocate a hardware parameters object.
  snd_pcm_hw_params_alloca(&params);

  // Fill it in with default values.
  snd_pcm_hw_params_any(handle, params);


  // Display information about the PCM interface

  printf("PCM handle name = '%s'\n",
         snd_pcm_name(handle));

  printf("PCM state = %s\n",
         snd_pcm_state_name(snd_pcm_state(handle)));

  snd_pcm_hw_params_get_access(params,
                          (snd_pcm_access_t *) &val);
  printf("access type = %s\n",
         snd_pcm_access_name((snd_pcm_access_t)val));

  snd_pcm_hw_params_get_format(params, &val);
  printf("format = '%s' (%s)\n",
    snd_pcm_format_name((snd_pcm_format_t)val),
    snd_pcm_format_description(
                             (snd_pcm_format_t)val));

  snd_pcm_hw_params_get_subformat(params,
                        (snd_pcm_subformat_t *)&val);
  printf("subformat = '%s' (%s)\n",
    snd_pcm_subformat_name((snd_pcm_subformat_t)val),
    snd_pcm_subformat_description(
                          (snd_pcm_subformat_t)val));

  snd_pcm_hw_params_get_channels(params, &val);
  printf("channels = %d\n", val);

  snd_pcm_hw_params_get_rate(params, &val, &dir);
  printf("rate = %d bps\n", val);

  snd_pcm_hw_params_get_period_time(params,
                                    &val, &dir);
  printf("period time = %d us\n", val);

  snd_pcm_hw_params_get_period_size(params,
                                    &frames, &dir);
  printf("period size = %d frames\n", (int)frames);

  snd_pcm_hw_params_get_buffer_time(params,
                                    &val, &dir);
  printf("buffer time = %d us\n", val);

  snd_pcm_hw_params_get_buffer_size(params,
                         (snd_pcm_uframes_t *) &val);
  printf("buffer size = %d frames\n", val);

  snd_pcm_hw_params_get_periods(params, &val, &dir);
  printf("periods per buffer = %d frames\n", val);

  snd_pcm_hw_params_get_rate_numden(params,
                                    &val, &val2);
  printf("exact rate = %d/%d bps\n", val, val2);

  val = snd_pcm_hw_params_get_sbits(params);
  printf("significant bits = %d\n", val);

  snd_pcm_hw_params_get_tick_time(params,
                                  &val, &dir);
  printf("tick time = %d us\n", val);

  val = snd_pcm_hw_params_is_batch(params);
  printf("is batch = %d\n", val);

  val = snd_pcm_hw_params_is_block_transfer(params);
  printf("is block transfer = %d\n", val);

  val = snd_pcm_hw_params_is_double(params);
  printf("is double = %d\n", val);

  val = snd_pcm_hw_params_is_half_duplex(params);
  printf("is half duplex = %d\n", val);

  val = snd_pcm_hw_params_is_joint_duplex(params);
  printf("is joint duplex = %d\n", val);

  val = snd_pcm_hw_params_can_overrange(params);
  printf("can overrange = %d\n", val);

  val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
  printf("can mmap = %d\n", val);

  val = snd_pcm_hw_params_can_pause(params);
  printf("can pause = %d\n", val);

  val = snd_pcm_hw_params_can_resume(params);
  printf("can resume = %d\n", val);

  val = snd_pcm_hw_params_can_sync_start(params);
  printf("can sync start = %d\n", val);

  snd_pcm_close(handle);
  */

    audio_openHandle(audio_card, &volume_audio_handle);

    volume_elem = audio_findSelem(audio_selem_name, &volume_audio_handle, &volume_audio_sid);
    if (volume_elem == NULL) {
      volume_no_device = 1;
      dbgprintf(DBG_ERROR, "Not able to find audio device: %s\n", audio_selem_name);
      volume_closeAudio();
      return 0;
    }


    volume_handleOpen = 1;
  }
  return 1;
}
//
///////////////////////




// Convert volume value from log db to scale percentage
void volume_dbToPercent(long volValue, long volMin, long volMax, long * volPercent) {
  if (volValue < volMin) {
    *volPercent = 0;
    return;
  }
  if (volValue > volMax) {
    *volPercent = 100;
    return;
  }

  double p = 48.0;
  double scaleS = 100.0 / (p - 1);
  double scaleT = -100 / (p - 1);
  *volPercent = scaleS * pow(p, (double)(volValue - volMin) / (double)(volMax - volMin)) + scaleT;
}



// Set playback volume control for persistant handle
void volume_setVolume(long volume) {
  volume_initAudio(0);
  if (volume_no_device == 1) {
    return;
  }

  int e;
  if ((e = snd_mixer_selem_set_playback_volume_all(volume_elem, volume)) < 0) {
    dbgprintf(DBG_ERROR, "Error Setting Playback Volume! %s\n", snd_strerror(e));
    volume_closeAudio();
  }
}

void volume_setPercent(long volPercent) {
  // Help fat fingers
  if (volPercent < 3) {
    volPercent = 0;
  }

  if (volPercent > 104) {
    volPercent = 104;
  }

  long volLog = pow(48, log(104 - volPercent));
  long volMax = pow(48, log(104));
  double volP = ((double)volLog / (double)volMax);
  double volE = (volume_max - volume_min);
  long volValE = volume_max - volP * volE;

  volume_setVolume(volValE);
}
















// Maybe initialize with a timeout for multiple volume changes in short time
long curDb;
int volume_getVolume(char* card, char* device, long * dbGain) {
  
  snd_mixer_t *volume_get_audio_handle;
  snd_mixer_selem_id_t *volume_get_audio_sid;

  audio_openHandle(card, &volume_get_audio_handle);

  snd_mixer_elem_t* volume_get_elem = audio_findSelem(device, &volume_get_audio_handle, &volume_get_audio_sid);
  if (volume_get_elem == NULL) {
    volume_no_device = 1;
    dbgprintf(DBG_ERROR, "Not able to find audio card: %s, device: %s\n", card, device);
    audio_closeHandle(&volume_get_audio_handle);
    return 0;
  }

  if (snd_mixer_selem_get_playback_volume(volume_get_elem, SND_MIXER_SCHN_FRONT_LEFT, &curDb) < 0) {
    audio_closeHandle(&volume_get_audio_handle);
    return 0;
  }
  audio_closeHandle(&volume_get_audio_handle);

  int changed = 0;
  if (*dbGain != curDb) {
    *dbGain = curDb;
    changed = 1;
  }

  return changed;
}





int volume_getVolumeRange(char* card, char* device, long * dbMin, long *dbMax) {
  
  snd_mixer_t *volume_get_audio_handle;
  snd_mixer_selem_id_t *volume_get_audio_sid;

  audio_openHandle(card, &volume_get_audio_handle);

  snd_mixer_elem_t* volume_get_elem = audio_findSelem(device, &volume_get_audio_handle, &volume_get_audio_sid);
  if (volume_get_elem == NULL) {
    volume_no_device = 1;
    dbgprintf(DBG_ERROR, "Not able to find audio card: %s, device: %s\n", card, device);
    audio_closeHandle(&volume_get_audio_handle);
    return 1;
  }

  if (snd_mixer_selem_get_playback_volume_range(volume_get_elem, dbMin, dbMax) < 0) {
    audio_closeHandle(&volume_get_audio_handle);
    return 1;
  }
  audio_closeHandle(&volume_get_audio_handle);
  
  return 0;
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
