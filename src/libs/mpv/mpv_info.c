
#include <stdio.h>
#include <stdlib.h>

#include "mpv_info.h"

struct libMpvPlayerInfo * LIBMPV_EVENTS_INIT_INFO() {
  struct libMpvPlayerInfo *eventInfo = (struct libMpvPlayerInfo*)malloc(sizeof(struct libMpvPlayerInfo));
  eventInfo->cnt = 0;
  eventInfo->has_file = 0;
  eventInfo->is_loaded = 0;
  eventInfo->is_playing = 0;
  eventInfo->is_seeking = 0;
  eventInfo->position = 0.00;
  eventInfo->duration = 0.00;
  eventInfo->pbrate = 1.00;
  eventInfo->pbrateUser = 0.75;
  eventInfo->pbrateStr = (char*)malloc(10 * sizeof(char));
  eventInfo->pbrateStr[0] = '\0';
  eventInfo->pbrateUserStr = (char*)malloc(10 * sizeof(char));
  eventInfo->pbrateUserStr[0] = '\0';
  eventInfo->positionStr = (char*)malloc(32 * sizeof(char));
  eventInfo->positionStr[0] = '\0';
  eventInfo->folder = (char*)malloc(1024 * sizeof(char));
  eventInfo->folder[0] = '\0';
  eventInfo->file = (char*)malloc(512 * sizeof(char));
  eventInfo->file[0] = '\0';
  return eventInfo;
}

void LIBMPV_EVENTS_DESTROY_INFO(struct libMpvPlayerInfo *eventInfo) {
  free(eventInfo->positionStr);
  free(eventInfo->pbrateStr);
  free(eventInfo->pbrateUserStr);
  free(eventInfo->folder);
  free(eventInfo->file);
  // free(eventInfo);
}

