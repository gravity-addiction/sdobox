#include <math.h>
#include <stdlib.h>

#include "skydiveorbust.h"
#include "queue/queue.h"
#include "mpv/mpv.h"



// Set sdob_player->paused
void pg_sdob_player_setpause(int paused) {
  sdob_player->paused = paused;
}

// Check sdob_player->paused
void pg_sdob_player_checkpause() {
  char* retPlay;
  if ((mpvSocketSinglet("pause", &retPlay)) != -1) {
    if (retPlay && strcmp(retPlay, "false") == 0) {
      pg_sdob_player_setpause(0);
    } else {
      pg_sdob_player_setpause(1);
    }
    free(retPlay);
  } else {
    pg_sdob_player_setpause(1);
  }
}

// Set sdob_player->pbrate
void pg_sdob_player_setpbrate() {
  char* retSpeed;
  if ((mpvSocketSinglet("speed", &retSpeed)) != -1) {
    sdob_player->pbrate = atof(retSpeed);
    free(retSpeed);
  } else {
    sdob_player->pbrate = 1.0;
  }
}

// Set sdob_player->duration
void pg_sdob_player_setduration() {
  // Update Duration
  char* retDur;
  if ((mpvSocketSinglet("duration", &retDur)) != -1) {
    // printf("Video Duration: Status: %d, %s\n", strlen(retDur), retDur);
    sdob_player->duration = atof(retDur);
    free(retDur);
    // printf("Video Duration Dbl: %f\n", sdob_player->duration);
  } else {
    // printf("%s\n", "No Video Duration!!");
    sdob_player->duration = 0;
  }
}


// Set sdob_chapters->cur
// Current Video Chapter
void pg_sdob_player_setChapterCur() {
  char* chapterRet;
  if ((mpvSocketSinglet("chapter", &chapterRet)) != -1) {
    sdob_chapters->cur = atoi(chapterRet);
    free(chapterRet);
  } else {
    sdob_chapters->cur = -1;
  }
}


// Set player in paused (1) or unpaused (0) state
void pg_sdob_player_pause(int paused) {
  sdob_player->paused = paused;
  pg_sdob_pl_sliderForceUpdate = 1;

  char* cmdStr;
  if (paused) {
    cmdStr = "set pause yes\n";
  } else { cmdStr = "set pause no\n"; }

  size_t cmdSz = strlen(cmdStr) + 1;
  char *cmd = (char*)malloc(cmdSz * sizeof(char));
  strlcpy(cmd, cmdStr, cmdSz);
  mpv_cmd(cmd);
}

// toggle play button
void pg_sdob_player_toggle_play() {
  pg_sdob_player_setpbrate();
  pg_sdob_player_checkpause();

  // printf("Toggle: %s, Speed: %s, Dbl: %f, End: %s\n", retPlay, retSpeed, dSpeed, pEnd);
  if (sdob_player->pbrate > 1.0 || sdob_player->pbrate < 1.0) {
    mpv_speed(1.0);
    pg_sdob_player_pause(0);
  } else if (sdob_player->paused == 0) {
    pg_sdob_player_pause(1);
  } else {
    pg_sdob_player_pause(0);
  }
}






// fetch chapter markers
void pg_sdob_player_video_chapterMarks(gslc_tsGui *pGui) {
  if (!sdob_chapters->len
      || sdob_chapters->len < 1
      || sdob_player->duration <= 0
  ) {
    pg_sdob_player_sliderTicks(pGui, NULL, 0);
    return;
  }

  double *tickMarks = (double*)calloc(sdob_chapters->len, sizeof(double));
  if (tickMarks == NULL) { return; }

  for (size_t i_chapter = 0; i_chapter < sdob_chapters->len; i_chapter++) {
    if (sdob_chapters->ptr[i_chapter]) {
      tickMarks[i_chapter] = (sdob_chapters->ptr[i_chapter] / sdob_player->duration) * 100;
    } else {
      tickMarks[i_chapter] = -1;
    }
  }
  pg_sdob_player_sliderTicks(pGui, tickMarks, sdob_chapters->len);
  free(tickMarks);
}


void pg_sdob_player_video_chapterList(int len) {
  char* pg_sdob_playerChapterTimeFmt = "chapter-list/%d/time";

  sdob_chapters->len = len;
  if (len > sdob_chapters->max) {
    sdob_chapters->max += 64;
    size_t chapterListSz = (sizeof(double) * sdob_chapters->max);
    double *oldList = realloc(sdob_chapters->ptr, chapterListSz);
    sdob_chapters->ptr = oldList;
  }

  for (size_t i_chapter = 0; i_chapter < sdob_chapters->max; i_chapter++) {
    if (i_chapter < len) {
      size_t mallocSz = snprintf(NULL, 0, pg_sdob_playerChapterTimeFmt, i_chapter) + 1;
      char *cmd = malloc(mallocSz);
      snprintf(cmd, mallocSz, pg_sdob_playerChapterTimeFmt, i_chapter);

      char* retChapterTime;
      if (mpvSocketSinglet(cmd, &retChapterTime) != -1) {
        sdob_chapters->ptr[i_chapter] = atof(retChapterTime);
        free(retChapterTime);
      } else {
        sdob_chapters->ptr[i_chapter] = -1;
      }
      free(cmd);
    } else {
      sdob_chapters->ptr[i_chapter] = -1;
    }
  }
}


void pg_sdob_player_video_chapters() {
  char* retChapters;
  if ((mpvSocketSinglet("chapter-list/count", &retChapters)) != -1) {
    pg_sdob_player_video_chapterList(atoi(retChapters));
    free(retChapters);
  }
}




void pg_sdob_player_sliderTicks(gslc_tsGui *pGui, double *tickMarks, int tickCnt) {
  if (tickCnt < 1) {
    gslc_ElemXSliderSetTicks(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], NULL, 0);
    return;
  }
  while (sdob_player_ticks->lock) { return; }
  sdob_player_ticks->lock = 1;

  if (tickCnt > sdob_player_ticks->max) {
    // printf("Upping Ticks\n");
    sdob_player_ticks->max += 64;
    size_t tickSz = (sizeof(double) * sdob_player_ticks->max);
    double * oldTicks;
    oldTicks = realloc(sdob_player_ticks->ptr, tickSz);
    sdob_player_ticks->ptr = oldTicks;
  }

  sdob_player_ticks->len = tickCnt;
  for (size_t t = 0; t < sdob_player_ticks->max; ++t) {
    if (t < tickCnt) {
      sdob_player_ticks->ptr[t] = tickMarks[t];
    } else {
      sdob_player_ticks->ptr[t] = -1;
    }
  }

  gslc_ElemXSliderSetTicks(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], sdob_player_ticks->ptr, sdob_player_ticks->len);
  sdob_player_ticks->lock = 0;
}




void pg_sdob_player_videoInfo(gslc_tsGui *pGui) {
  // video_display_fps();
  if (sdob_judgement->sowt >= 0) { return; }

  // Update Duration
  pg_sdob_player_setduration();
  // Update Chapters
  pg_sdob_player_video_chapters();
  pg_sdob_player_video_chapterMarks(pGui);
  // Ticks exist, map then to element

/*
  // Both Chapters and Duration we can set the slider ticks
  if (i_chapter_cnt && sdob_player->duration) {

    // Ticks exist, map then to element
    if (arrTLen > 0) {
      gslc_ElemXSliderSetTicks(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], arrT, arrTLen);
      gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
    }

  }
*/

  // Get Filename / Hash
  // char retFilename[1024];
  // mpv_get_prop("filename", retFilename, sizeof(retFilename));
  // fileinfo_create(retFilename);


  // video_estimated_display_fps();
  // video_position();
  // video_position_percent();

  // Redraw after getting video info
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
}




void setSliderPos(gslc_tsGui *pGui, int16_t nPercent) {
  // printf("Set Slider Pos: %d\n", nPercent);
  gslc_ElemXSliderSetPos(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], (nPercent * 10));
}

void setSliderPosByTime(gslc_tsGui *pGui) {
  int16_t nTick = 0;
  // printf("Set Slider Pos By Time\n");
  // Calculate percentage
  if (sdob_player->position && sdob_player->duration > 0) {
    // printf("Set Slider Pos: %f, Dur: %f\n", sdob_player->position, sdob_player->duration);
    nTick = (sdob_player->position * 1000) / sdob_player->duration;

    if (nTick >= 0) {
      gslc_ElemXSliderSetPos(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], nTick);
    }
  }
}

