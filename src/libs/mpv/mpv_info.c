
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
  eventInfo->pbrateStr = (char*)calloc(10, sizeof(char));
  eventInfo->pbrateUserStr = (char*)calloc(10, sizeof(char));
  eventInfo->positionStr = (char*)calloc(32, sizeof(char));
  eventInfo->folder = (char*)calloc(1024, sizeof(char));
  eventInfo->file = (char*)calloc(512, sizeof(char));
  return eventInfo;
}

void LIBMPV_EVENTS_DESTROY_INFO(struct libMpvPlayerInfo *eventInfo) {
  free(eventInfo->positionStr);
  free(eventInfo->pbrateStr);
  free(eventInfo->pbrateUserStr);
  free(eventInfo->folder);
  free(eventInfo->file);
  free(eventInfo);
}

