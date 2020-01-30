
#include <stdio.h>
#include <stdlib.h>

#include "shared.h"
#include "audio.h"

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

void volume_mute() {
  size_t cmdSz = strlen("amixer sset PCM toggle") + 1;
  char *fullpath = malloc(cmdSz);
  snprintf(fullpath, cmdSz, "%s", "amixer sset PCM toggle");
  // run_system_cmd(fullpath);
  free(fullpath);
}