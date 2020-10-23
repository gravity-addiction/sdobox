#include <syslog.h>
#include <stdlib.h>
#include <time.h>

#include "libs/shared.h"
#include "skydiveorbust.h"
#include "libs/queue/queue.h"
#include "libs/mpv/mpv.h"
#include "libs/mpv/mpv_info.h"


int pg_sdobInsertMark(int markSelected, double markTime, int mark) {
// Create new Mark
  int curScorecardSize = sdob_judgement->marks->size;
  sdob_judgement->marks->size++;

  // Insert before provided selected mark
  if (markSelected >= 0) {
    int sLen = sdob_judgement->marks->size;
    // Make room!
    for (size_t i = sLen; i >= markSelected; i--) {
      sdob_judgement->marks->arrScorecardId[i] = sdob_judgement->marks->arrScorecardId[i - 1];
      sdob_judgement->marks->arrScorecardMilli[i] = sdob_judgement->marks->arrScorecardMilli[i - 1];
      sdob_judgement->marks->arrScorecardPoints[i] = sdob_judgement->marks->arrScorecardPoints[i - 1];
      sdob_judgement->marks->arrScorecardTimes[i] = sdob_judgement->marks->arrScorecardTimes[i - 1];
    }
  } else {
    char* retTimePos;
    markSelected = curScorecardSize;
    if (markTime < 0) {
      if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
        markTime = atof(retTimePos);
        free(retTimePos);
        sdob_judgement->marks->arrScorecardTimes[markSelected] = markTime;
      }
    }

    if (markTime < 0) {
      // debug_print("%s\n", "Unable to grab video time!");
      markTime = 0;
    }
  }

  // debug_print("INSERT MARK: %d, Sel: %d, T: %f\n", mark, markSelected, markTime);
  if (markTime >= 0 && markSelected == 0) { // Change up Start of Working Time
    struct queue_head *itemSOWT = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemSOWT);
    itemSOWT->action = E_Q_SCORECARD_SCORING_SOWT;
    itemSOWT->time = markTime;
    queue_put(itemSOWT, pg_sdobQueue, &pg_sdobQueueLen);
    mark = E_SCORES_SOWT;
  }

  if (mark >= 0) {
    sdob_judgement->marks->arrScorecardPoints[markSelected] = mark;
  } else {
    sdob_judgement->marks->arrScorecardPoints[markSelected] = E_SCORES_POINT;
  }

  // gslc_ElemSetRedraw(&m_gui, m_pElemScorecardBox, GSLC_REDRAW_FULL);
  return (markTime >= 0);
}

void pg_sdobDeleteMark(int markSelected) {

  // Save Last Records of volatile mem so maybe take it
  int curScorecardSize = sdob_judgement->marks->size;
  int lastId = sdob_judgement->marks->arrScorecardId[curScorecardSize - 1];
  int lastMilli = sdob_judgement->marks->arrScorecardMilli[curScorecardSize - 1];
  int lastPoint = sdob_judgement->marks->arrScorecardPoints[curScorecardSize - 1];
  int lastTime = sdob_judgement->marks->arrScorecardTimes[curScorecardSize - 1];

  // new score during copy, just force a retry, blame the box
  if (curScorecardSize != sdob_judgement->marks->size) { return; }
  // all good reduce scorecard size
  sdob_judgement->marks->size--;

  if (markSelected == 0) {
    // Resetting Start time
    pg_sdobSOWTReset();

  } else if (markSelected > 1) {
    // Move everything back one
    for (size_t i = markSelected; i < curScorecardSize - 1; i++) {
      sdob_judgement->marks->arrScorecardId[i] = sdob_judgement->marks->arrScorecardId[i + 1];
      sdob_judgement->marks->arrScorecardMilli[i] = sdob_judgement->marks->arrScorecardMilli[i + 1];
      sdob_judgement->marks->arrScorecardPoints[i] = sdob_judgement->marks->arrScorecardPoints[i + 1];
      sdob_judgement->marks->arrScorecardTimes[i] = sdob_judgement->marks->arrScorecardTimes[i + 1];
    }

    // Update last record
    if (markSelected < curScorecardSize - 1) {
      sdob_judgement->marks->arrScorecardId[curScorecardSize - 2] = lastId;
      sdob_judgement->marks->arrScorecardMilli[curScorecardSize - 2] = lastMilli;
      sdob_judgement->marks->arrScorecardPoints[curScorecardSize - 2] = lastPoint;
      sdob_judgement->marks->arrScorecardTimes[curScorecardSize - 2] = lastTime;
    }
  }
  // gslc_ElemSetRedraw(&m_gui, m_pElemScorecardBox, GSLC_REDRAW_FULL);
}

void pg_sdobMoveMark(int markSelected, int moveAmt) {
  // Change to Selected
  if (markSelected == -1 && moveAmt == 0) {
    sdob_judgement->marks->selected = -1;
  } else {
    // Determine mark to move to

    // start at specified selected value
    if (markSelected >= 0) {
      sdob_judgement->marks->selected = markSelected;
    }

    // Change Amount of selected value
    if (moveAmt != 0) {
      sdob_judgement->marks->selected += moveAmt;
    }

    // Check to see if the selecte value needs to loop end to beginning
    if (moveAmt != 0 && sdob_judgement->marks->selected >= sdob_judgement->marks->size) {
      sdob_judgement->marks->selected = 1;
    }
    // Check to see if the selecte value needs to loop beginning to end
    else if (moveAmt != 0 && sdob_judgement->marks->selected < 1) {
      sdob_judgement->marks->selected = sdob_judgement->marks->size - 1;
    }
  }
  // gslc_ElemSetRedraw(&m_gui, m_pElemScorecardBox, GSLC_REDRAW_FULL);
}






int pg_sdobSOWTSet(double markTime, double workingTime) {
  sdob_judgement->sowt = markTime;
  sdob_judgement->workingTime = workingTime;
  // debug_print("SOWT: %f, /home/pi/Videos/%s\n", sdob_judgement->sowt, sdob_judgement->video_file);

  struct queue_head *itemStart = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemStart);
  itemStart->action = E_Q_ACTION_MPV_COMMAND;
  size_t startSz = snprintf(NULL, 0, "set start %f\n", sdob_judgement->sowt) + 1;
  if (startSz > 0) {
    itemStart->cmd = (char*)calloc(startSz, sizeof(char));
    snprintf(itemStart->cmd, startSz, "set start %f\n", sdob_judgement->sowt);
    queue_put(itemStart, pg_sdobQueue, &pg_sdobQueueLen);
  }

  struct queue_head *itemLength = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemLength);
  itemLength->action = E_Q_ACTION_MPV_COMMAND;
  size_t lengthSz = snprintf(NULL, 0, "set length %f\n", sdob_judgement->workingTime) + 1;
  if (lengthSz > 0) {
    itemLength->cmd = (char*)calloc(lengthSz, sizeof(char));
    snprintf(itemLength->cmd, lengthSz, "set length %f\n", sdob_judgement->workingTime);
    queue_put(itemLength, pg_sdobQueue, &pg_sdobQueueLen);
  }
  // gslc_ElemSetRedraw(&m_gui, m_pElemScorecardBox, GSLC_REDRAW_FULL);
  return 1;
}

int pg_sdobSOWTReset() {
  sdob_judgement->sowt = -1.0;
  // debug_print("Reset SOWT: %f, /home/pi/Videos/%s\n", sdob_judgement->sowt, sdob_judgement->video_file);

  struct queue_head *itemStart = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemStart);
  itemStart->action = E_Q_ACTION_MPV_COMMAND;
  size_t cmdStartSz = strlen("set start 0\n") + 1;
  if (cmdStartSz > 0) {
    itemStart->cmd = (char*)calloc(cmdStartSz, sizeof(char));
    strlcpy(itemStart->cmd, "set start 0\n", cmdStartSz);
    queue_put(itemStart, pg_sdobQueue, &pg_sdobQueueLen);
  }

  struct queue_head *itemEnd = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemEnd);
  itemEnd->action = E_Q_ACTION_MPV_COMMAND;
  size_t cmdEndSz = strlen("set length 100%\n") + 1;
  if (cmdEndSz > 0) {
    itemEnd->cmd = (char*)calloc(cmdEndSz, sizeof(char));
    strlcpy(itemEnd->cmd, "set length 100%\n", cmdEndSz);
    queue_put(itemEnd, pg_sdobQueue, &pg_sdobQueueLen);
  }

//  struct queue_head *itemChapters = malloc(sizeof(struct queue_head));
//  INIT_QUEUE_HEAD(itemChapters);
//  itemChapters->action = E_Q_PLAYER_VIDEO_INFO;
//  queue_put(itemChapters, pg_sdobQueue, &pg_sdobQueueLen);


  // struct queue_head *itemLength = malloc(sizeof(struct queue_head));
  // INIT_QUEUE_HEAD(itemLength);
  // itemLength->action = E_Q_ACTION_MPV_COMMAND;
  // char *nCmd = (char*)calloc(cmdSz, sizeof(char));
  // char retFormat[20];
  // secs_to_time((int)(sdob_video->duration * 1000), retFormat, 20);

  // FIX SIZE_T AND retFormat (if needed)
  // size_t cmdSz = snprintf(nCmd, strlen("set end %d\n") + 20;
  // if (cmdSz > 0) {
  //   itemLength->cmd = (char*)calloc(cmdSz, sizeof(char));
  //   snprintf(itemLength->cmd, cmdSz, "set end %s\n", retFormat);
  //   queue_put(itemLength, pg_sdobQueue, &pg_sdobQueueLen);
  // }

  // gslc_ElemSetRedraw(&m_gui, m_pElemScorecardBox, GSLC_REDRAW_FULL);
  return 1;
}


int pg_sdobScoringMarkHidden(int markI) {
  // Make sure the mark is viewable
  int i_slotTop = (pg_sdob_slot_scroll * pg_sdob_slot_max);
  int i_slotBottom = i_slotTop + (pg_sdob_line_max * pg_sdob_slot_max);
  if (markI < i_slotTop) {
    return -1;
  } else if (sdob_judgement->marks->selected >= i_slotBottom) {
    return 1;
  }
  return 0;
}

void pg_sdobScoringMarks(gslc_tsGui *pGui) {
//  printf("Scoreing Marks: ");

  sdob_judgement->marks->tickCnt = sdob_judgement->marks->size;
  if (sdob_judgement->marks->tickCnt < 1) { return; }

  for(int i = 0; i < sdob_judgement->marks->tickCnt; ++i) {
    if (sdob_judgement->marks->arrScorecardTimes[i] >= 0 && !pg_sdob_timeline_zoom_workingtime) {
      // Get percent in working time ((mark - sowt) / wt)
      sdob_judgement->marks->arrScorecardTicks[i] = ((sdob_judgement->marks->arrScorecardTimes[i] / libMpvVideoInfo->duration) * 100);
    } else if (sdob_judgement->marks->arrScorecardTimes[i] >= 0) {
      // Get percent in working time ((mark - sowt) / wt)
      sdob_judgement->marks->arrScorecardTicks[i] = (((sdob_judgement->marks->arrScorecardTimes[i] - sdob_judgement->sowt) / sdob_judgement->workingTime) * 100);
    } else {
      sdob_judgement->marks->arrScorecardTicks[i] = -1.0;
    }
  }

  // Ticks exist, map then to element
  pg_sdob_player_sliderTicks(pGui, sdob_judgement->marks->arrScorecardTicks, sdob_judgement->marks->tickCnt);
}


void pg_sdobScoringSelectionClear(gslc_tsGui *pGui) {
  if (sdob_judgement->marks->selected >= 0) {
    sdob_judgement->marks->selected = -1;
    gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
  }
  sdob_judgement->marks->last = -1;
}

void pg_sdobScoringSelectionLastHidden(gslc_tsGui *pGui) {
  // Look for ->last to reselect
  if (sdob_judgement->marks->selected == -1 &&
      sdob_judgement->marks->selected > -1 &&
      pg_sdobScoringMarkHidden(sdob_judgement->marks->last) == 0
  ) {
    sdob_judgement->marks->selected = sdob_judgement->marks->last;
    sdob_judgement->marks->last = -1;
    gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

  } else // Check if current selection is now hidden, unselect
  if (pg_sdobScoringMarkHidden(sdob_judgement->marks->selected)) {
    sdob_judgement->marks->last = (int)sdob_judgement->marks->selected;
    sdob_judgement->marks->selected = -1;
    gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
  }

}



// --------------------------
// SYSLOG Scorecard
// --------------------------
int pg_sdobSubmitScorecard() {
//   Jan 26 12:49:01 Eddie MyAVPlayer[82462]: SUBMISSION/MEET2019 4143,1,JR,0,01/26/2019 12:49:01,Group14-12_2.mp4,403.600000,148.866667,/,/,/,/,O,/,/,/,/,/,O,/,/,/,/,O,/,/,/,/,/,/,/,/
  if (!sdob_judgement || !sdob_judgement->marks->size) {
    return 0;
  }

  time_t current_time;
  char c_time_string[26];
  struct tm* tm_info;
  time(&current_time);
  tm_info = localtime(&current_time);
  strftime(c_time_string, 26, "%m/%d/%Y %H:%M:%S", tm_info);
  char* submit_fmt = "SUBMISSION/%s %s,%s,%s,0,%s,%s,%f,%f,%s";

  // Mark plus comma for each space
  size_t markSpace = (sdob_judgement->marks->size * 2);
  // last space doesn't use a comma
  if (markSpace > 0) { --markSpace; }
  char *csv_score = (char*)calloc(markSpace + 1, sizeof(char));
  size_t csvLen = 0;
  csv_score[csvLen] = '\0';
  int madeMark = 0;
  for (size_t s = 0; s < sdob_judgement->marks->size; s++) {
    if (madeMark) {
      strcat(csv_score, ",");
      ++csvLen;
    }

    switch (sdob_judgement->marks->arrScorecardPoints[s]) {
      case E_SCORES_POINT:
        strcat(csv_score, "/");
        ++csvLen;
        madeMark = 1;
      break;
      case E_SCORES_BUST:
        strcat(csv_score, "O");
        ++csvLen;
        madeMark = 1;
      break;
      case E_SCORES_OMISSION:
        strcat(csv_score, "X");
        ++csvLen;
        madeMark = 1;
      break;
      case E_SCORES_SOWT:
        madeMark = 0;
      break;
      default:
        strcat(csv_score, "?");
        ++csvLen;
        madeMark = 1;
      break;
    }
    csv_score[csvLen] = '\0';
  }


  openlog ("touchapp", LOG_NDELAY | LOG_PID, LOG_LOCAL1);
  syslog (LOG_NOTICE, submit_fmt,
      (char*)sdob_judgement->meet, (char*)sdob_judgement->team, (char*)sdob_judgement->round,
      (char*)sdob_judgement->judge, c_time_string, sdob_judgement->video_file,
      libMpvVideoInfo->duration, sdob_judgement->sowt, csv_score);
  closelog();

  if (csv_score != NULL) { free(csv_score); }
  return 1;
}
