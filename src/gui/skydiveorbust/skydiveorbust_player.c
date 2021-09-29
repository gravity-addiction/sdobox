#include <math.h>
#include <stdlib.h>
#include <pthread.h>

#include "skydiveorbust.h"
#include "libs/queue/queue.h"
#include "libs/mpv2/mpv2.h"
#include "libs/dbg/dbg.h"



void pg_sdob_player_clear() {
  
}

// Set sdob_chapters->cur
// Current Video Chapter
void pg_sdob_player_setChapterCur() {
  /*
  mpv_any_u* chapterRet;
  if ((mpvSocketSinglet("chapter", &chapterRet)) != -1) {
    sdob_chapters->cur = chapterRet->integer;
    MPV_ANY_U_FREE(chapterRet);
  } else {
    sdob_chapters->cur = -1;
  }
  */
}


// fetch chapter markers
void pg_sdob_player_video_chapterMarks(gslc_tsGui *pGui) {
  if (!sdob_chapters->len
      || sdob_chapters->len < 1
      || libMpvVideoInfo->duration <= 0
  ) {
    pg_sdob_player_sliderTicks(pGui, NULL, 0);
    return;
  }

  uint16_t *tickMarks = (uint16_t*)calloc(sdob_chapters->len, sizeof(uint16_t));
  if (tickMarks == NULL) { return; }

  for (size_t i_chapter = 0; i_chapter < sdob_chapters->len; i_chapter++) {
    if (sdob_chapters->ptr[i_chapter]) {
      tickMarks[i_chapter] = (sdob_chapters->ptr[i_chapter] / libMpvVideoInfo->duration) * 1000;
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
    size_t chapterListSz = (sizeof(uint16_t) * sdob_chapters->max);
    uint16_t *oldList = realloc(sdob_chapters->ptr, chapterListSz);
    sdob_chapters->ptr = oldList;
  }

  for (size_t i_chapter = 0; i_chapter < sdob_chapters->max; i_chapter++) {
    if (i_chapter < len) {
      size_t mallocSz = snprintf(NULL, 0, pg_sdob_playerChapterTimeFmt, i_chapter) + 1;
      char *cmd = malloc(mallocSz);
      snprintf(cmd, mallocSz, pg_sdob_playerChapterTimeFmt, i_chapter);

      /*
      mpv_any_u* retChapterTime;
      if (mpvSocketSinglet(cmd, &retChapterTime) != -1) {
        sdob_chapters->ptr[i_chapter] = retChapterTime->floating;
        MPV_ANY_U_FREE(retChapterTime);
      } else {
        sdob_chapters->ptr[i_chapter] = -1;
      }
      */
      free(cmd);
    } else {
      sdob_chapters->ptr[i_chapter] = -1;
    }
  }
}


void pg_sdob_player_video_chapters() {
  /*
  mpv_any_u* retChapters;
  if ((mpvSocketSinglet("chapter-list/count", &retChapters)) != -1) {
    pg_sdob_player_video_chapterList(retChapters->integer);
    MPV_ANY_U_FREE(retChapters);
  }
  */
}




void pg_sdob_player_sliderTicks(gslc_tsGui *pGui, uint16_t *tickMarks, uint8_t tickCnt) {
  if (tickCnt < 1) {
    gslc_ElemXSliderSetTicks(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], NULL, 0);
    return;
  }
  pthread_mutex_lock(&sdob_player_ticks->lock);

  if (tickCnt > sdob_player_ticks->max) {
    // printf("Upping Ticks\n");
    sdob_player_ticks->max += 64;
    uint16_t * oldTicks;
    oldTicks = realloc(sdob_player_ticks->ptr, (sdob_player_ticks->max * sizeof(uint16_t)));
    sdob_player_ticks->ptr = oldTicks;
  }

  sdob_player_ticks->len = tickCnt;
  for (size_t t = 0; t < sdob_player_ticks->max; ++t) {
    if (t < tickCnt) {
      sdob_player_ticks->ptr[t] = tickMarks[t] * 10;
    } else {
      sdob_player_ticks->ptr[t] = -1;
    }
  }

  gslc_ElemXSliderSetTicks(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], sdob_player_ticks->ptr, sdob_player_ticks->len);
  pthread_mutex_unlock(&sdob_player_ticks->lock);
}

void setSliderPos(gslc_tsGui *pGui, int16_t nPercent) {
  // printf("Set Slider Pos: %d\n", nPercent);
  gslc_ElemXSliderSetPos(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], (nPercent * 10));
}

void setSliderPosByTime(gslc_tsGui *pGui) {
  int16_t nTick = 0;
  dbgprintf(DBG_DEBUG, "Set Slider Pos By Time: Pos: %f - Dur: %f\n", libMpvVideoInfo->position, libMpvVideoInfo->duration);
  // Calculate percentage
  if (libMpvVideoInfo->position && libMpvVideoInfo->duration > 0) {
    // printf("Set Slider Pos: %f, Dur: %f\n", libMpvVideoInfo->position, libMpvVideoInfo->duration);
    nTick = (libMpvVideoInfo->position * 1000) / libMpvVideoInfo->duration;

    if (nTick >= 0) {
      gslc_ElemXSliderSetPos(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], nTick);
    }
  }
}

