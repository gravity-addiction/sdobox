#include <syslog.h>
#include <stdlib.h>
#include <time.h>
#include <jansson.h>
#include <curl/curl.h>

#include "libs/shared.h"
#include "./skydiveorbust.h"
#include "./skydiveorbust_scorecard.h"
#include "./skydiveorbust_zmq.h"

#include "libs/md5/md5.h"
#include "libs/queue/queue.h"
#include "libs/mpv-zmq/mpv-zmq.h"
#include "libs/curl-sdob/curl-sdob.h"
#include "libs/dbg/dbg.h"
#include "libs/sdob-zmq/sdob-cache.h"
#include "libs/sdob-zmq/sdob-zmq.h"


int pg_sdobInsertMark(int markSelected, double markTime, int mark) {
// Create new Mark
  int curScorecardSize = sdob_judgement->marks->size;

  // Insert before provided selected mark
  if (markSelected >= 0 && curScorecardSize > 0) {
    // Make room!
    for (size_t i = curScorecardSize; i >= markSelected; i--) {
      if (i == 0) { break; }
      sdob_judgement->marks->arrScorecardPoints[i] = sdob_judgement->marks->arrScorecardPoints[i - 1];
      sdob_judgement->marks->arrScorecardTimes[i] = sdob_judgement->marks->arrScorecardTimes[i - 1];
    }
  } else {
    markSelected = curScorecardSize;
  }
  
  sdob_judgement->marks->size++;
/*
  // Try getting mark time when not present
  mpv_any_u* retTimePos;
  if (markTime < 0 && sdob_devicehost->isHost == 1) {
    if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
      if (retTimePos->hasPtr == 1) { markTime = atof(retTimePos->ptr);
      } else { markTime = retTimePos->floating;
      }
      MPV_ANY_U_FREE(retTimePos);
    }
  }
*/
  // sanity check, never less than 0
  if (markTime < 0) {
    // debug_print("%s\n", "Unable to grab video time!");
    markTime = 0.0;
  }

  dbgprintf(DBG_DEBUG, "INSERT MARK: %d, Sel: %d, T: %f\n", mark, markSelected, markTime);
  dbgprintf(DBG_DEBUG, "Prestart: %f, Total: %f\n", sdob_judgement->prestartTime, sdob_judgement->sopst);
  
  // Determine which type of mark to insert
  if (markSelected == 0 && sdob_judgement->prestartTime > 0 && sdob_judgement->sopst == -1.0) { // Change up Start of Working Time
    mark = E_SCORES_SOPST;

    struct queue_head *itemSOPST = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemSOPST);
    itemSOPST->action = E_Q_SCORECARD_SCORING_SOPST;
    itemSOPST->amt = sdob_judgement->prestartTime;
    itemSOPST->time = markTime;
    itemSOPST->mark = mark;
    pg_sdob_add_action(&itemSOPST);
    // queue_put(itemSOPST, pg_sdobQueue, &pg_sdobQueueLen);

  } else if (
    (sdob_judgement->prestartTime == 0 && markSelected == 0) || 
    (sdob_judgement->prestartTime > 0 && markSelected == 1 && sdob_judgement->sowt == -1.0)
  ) { // Change up Start of Working Time
    // mark = E_SCORES_SOWT;

    struct queue_head *itemSOWT = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemSOWT);
    itemSOWT->action = E_Q_SCORECARD_SCORING_SOWT;
    itemSOWT->amt = sdob_judgement->workingTime;
    itemSOWT->time = markTime;
    itemSOWT->mark = mark;
    pg_sdob_add_action(&itemSOWT);
    // queue_put(itemSOWT, pg_sdobQueue, &pg_sdobQueueLen);
  }

  // Add Marker Time
  sdob_judgement->marks->arrScorecardTimes[markSelected] = markTime;

  // Add Marker Class
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
      sdob_judgement->marks->arrScorecardPoints[i] = sdob_judgement->marks->arrScorecardPoints[i + 1];
      sdob_judgement->marks->arrScorecardTimes[i] = sdob_judgement->marks->arrScorecardTimes[i + 1];
    }

    // Update last record
    if (markSelected < curScorecardSize - 1) {
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





int pg_sdobSOPSTSet(double markTime, double prestartTime) {
  dbgprintf(DBG_DEBUG, "Setting Prestart Time at %f for %f seconds\n", markTime, prestartTime);
  if (markTime < 0) { markTime = 0.0; }
  sdob_judgement->sopst = markTime;
  sdob_judgement->prestartTime = prestartTime;
  // debug_print("SOWT: %f, /home/pi/Videos/%s\n", sdob_judgement->sowt, sdob_judgement->video_file);

  return 1;
}

int pg_sdobSOWTSet(double markTime, double workingTime) {
  if (markTime < 0) { markTime = 0.0; }
  // printf("Setting Working Time at %f for %f seconds\n", markTime, workingTime);
  sdob_judgement->sowt = markTime;
  sdob_judgement->workingTime = workingTime;
  // debug_print("SOWT: %f, /home/pi/Videos/%s\n", sdob_judgement->sowt, sdob_judgement->video_file);

  if (sdob_devicehost->teamStart == 0) {
    struct queue_head *itemStart = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemStart);
    itemStart->action = E_Q_ACTION_MPV_COMMAND;
    size_t startSz = snprintf(NULL, 0, "set;start;%f", sdob_judgement->sowt) + 1;
    if (startSz > 0) {
      itemStart->cmd = (char*)calloc(startSz, sizeof(char));
      snprintf(itemStart->cmd, startSz, "set;start;%f", sdob_judgement->sowt);
      pg_sdob_add_action(&itemStart);
      // queue_put(itemStart, pg_sdobQueue, &pg_sdobQueueLen);
    }

    struct queue_head *itemLength = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemLength);
    itemLength->action = E_Q_ACTION_MPV_COMMAND;
    size_t lengthSz = snprintf(NULL, 0, "set;length;%f", sdob_judgement->workingTime) + 1;
    if (lengthSz > 0) {
      itemLength->cmd = (char*)calloc(lengthSz, sizeof(char));
      snprintf(itemLength->cmd, lengthSz, "set;length;%f", sdob_judgement->workingTime);
      pg_sdob_add_action(&itemLength);
      // queue_put(itemLength, pg_sdobQueue, &pg_sdobQueueLen);
    }
  }

  // gslc_ElemSetRedraw(&m_gui, m_pElemScorecardBox, GSLC_REDRAW_FULL);
  return 1;
}

int pg_sdobSOWTReset() {
  sdob_judgement->sopst = -1.0;
  sdob_judgement->sowt = -1.0;
  // debug_print("Reset SOWT: %f, /home/pi/Videos/%s\n", sdob_judgement->sowt, sdob_judgement->video_file);

  if (sdob_devicehost->teamStart == 0) {
    struct queue_head *itemStart = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemStart);
    itemStart->action = E_Q_ACTION_MPV_COMMAND;
    size_t cmdStartSz = strlen("set;start;0") + 1;
    if (cmdStartSz > 0) {
      itemStart->cmd = (char*)calloc(cmdStartSz, sizeof(char));
      strlcpy(itemStart->cmd, "set;start;0", cmdStartSz);
      pg_sdob_add_action(&itemStart);
      // queue_put(itemStart, pg_sdobQueue, &pg_sdobQueueLen);
    }

    struct queue_head *itemEnd = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemEnd);
    itemEnd->action = E_Q_ACTION_MPV_COMMAND;
    size_t cmdEndSz = strlen("set;length;100%") + 1;
    if (cmdEndSz > 0) {
      itemEnd->cmd = (char*)calloc(cmdEndSz, sizeof(char));
      strlcpy(itemEnd->cmd, "set;length;100%", cmdEndSz);
      pg_sdob_add_action(&itemEnd);
      // queue_put(itemEnd, pg_sdobQueue, &pg_sdobQueueLen);
    }
  }


//  struct queue_head *itemChapters = malloc(sizeof(struct queue_head));
//  INIT_QUEUE_HEAD(itemChapters);
//  itemChapters->action = E_Q_PLAYER_VIDEO_INFO;
//  pg_sdob_add_action(&itemChapters);
//  // queue_put(itemChapters, pg_sdobQueue, &pg_sdobQueueLen);


  // struct queue_head *itemLength = malloc(sizeof(struct queue_head));
  // INIT_QUEUE_HEAD(itemLength);
  // itemLength->action = E_Q_ACTION_MPV_COMMAND;
  // char *nCmd = (char*)calloc(cmdSz, sizeof(char));
  // char retFormat[20];
  // secs_to_time((int)(sdob_video->duration * 1000), retFormat, 20);

  // FIX SIZE_T AND retFormat (if needed)
  // size_t cmdSz = snprintf(nCmd, strlen("set;end;%d") + 20;
  // if (cmdSz > 0) {
  //   itemLength->cmd = (char*)calloc(cmdSz, sizeof(char));
  //   snprintf(itemLength->cmd, cmdSz, "set;end;%s", retFormat);
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
  // printf("Scoreing Marks: %d, Tick Cnt: %d\n", sdob_judgement->marks->size, sdob_judgement->marks->tickCnt);

  sdob_judgement->marks->tickCnt = sdob_judgement->marks->size;
  if (sdob_judgement->marks->tickCnt < 1) { return; }

  for(int i = 0; i < sdob_judgement->marks->tickCnt; ++i) {
    if (!pg_sdob_timeline_zoom_workingtime) {
      // Get percent in working time ((mark - sowt) / wt)
      sdob_judgement->marks->arrScorecardTicks[i] = ((sdob_judgement->marks->arrScorecardTimes[i] / libmpvCache->player->duration) * 100);
    } else if (sdob_judgement->marks->arrScorecardTimes[i] >= 0) {
      // Get percent in working time ((mark - sowt) / wt)
      sdob_judgement->marks->arrScorecardTicks[i] = (((sdob_judgement->marks->arrScorecardTimes[i] - sdob_judgement->sowt) / sdob_judgement->workingTime) * 100);
    } else {
      sdob_judgement->marks->arrScorecardTicks[i] = 0;
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
  if (!sdob_judgement || !sdob_judgement->marks) {
    return 0;
  }

  char* s = NULL;

  json_t *root = json_object();
  json_t *json_prestarttime = json_object();
  json_t *json_workingtime = json_object();
  json_t *json_marks = json_array();

  json_object_set_new(root, "prestartTime", json_prestarttime);
  json_object_set_new(json_prestarttime, "start", json_real(sdob_judgement->sopst));
  json_object_set_new(json_prestarttime, "time", json_real(sdob_judgement->prestartTime));
  json_object_set_new(root, "workingTime", json_workingtime);
  json_object_set_new(json_workingtime, "start", json_real(sdob_judgement->sowt));
  json_object_set_new(json_workingtime, "time", json_real(sdob_judgement->workingTime));
  json_object_set_new(root, "name", json_string(sdob_judgement->judge));
  json_object_set_new(root, "comp", json_string(sdob_judgement->compStr));
  json_object_set_new(root, "compId", json_string(sdob_judgement->comp));
  json_object_set_new(root, "event", json_string(sdob_judgement->eventStr));
  json_object_set_new(root, "eventId", json_string(sdob_judgement->event));
  json_object_set_new(root, "teamNumber", json_string(sdob_judgement->team));
  json_object_set_new(root, "rnd", json_string(sdob_judgement->round));
  json_object_set_new(root, "marks", json_marks);
  

  for (size_t s = 0; s < sdob_judgement->marks->size; s++) {
    json_t *json_mark = json_object();
    json_array_append(json_marks, json_mark);
    
    switch (sdob_judgement->marks->arrScorecardPoints[s]) {
      case E_SCORES_POINT:
        json_object_set_new(json_mark, "class", json_string("point"));
      break;
      case E_SCORES_BUST:
        json_object_set_new(json_mark, "class", json_string("bust"));
      break;
      case E_SCORES_OMISSION:
        json_object_set_new(json_mark, "class", json_string("omission"));
      break;
      case E_SCORES_SOWT:
        json_object_set_new(json_mark, "class", json_string("blank"));
      break;
      default:
        json_object_set_new(json_mark, "class", json_string(""));
      break;
    }
    json_object_set_new(json_mark, "time", json_real(sdob_judgement->marks->arrScorecardTimes[s]));
    // json_object_set_new(json_mark, "tick", json_real(s));
    json_decref(json_mark);
  }
  
  // Create Filepath and MD5 Hash
  char dest[strlen(sdob_judgement->video->local_folder) + strlen(sdob_judgement->video->video_file) + 2];
  combineFilePath(dest, sdob_judgement->video->local_folder, sdob_judgement->video->video_file);
  
  /*
  if (sdob_devicehost->isHost == 1) {
    char* md5H;
    int md5x = md5HashFile(dest, &md5H);
    if (md5x == 0) {
      // printf("MD5: %d - %s\n", md5x, md5H);
      json_object_set_new(root, "md5", json_string(md5H));
      free(md5H);
    }
  }
  */

  // Update roomid from SdobCache
  if (libSdobCache->server->roomid == NULL) {
    libsdob_zmq_getroomid();
  }
  json_object_set_new(root, "roomid", json_string(libSdobCache->server->roomid));

  // Grab Device token
  FILE *tokenFile;
  char tokenStr[8];
  tokenFile = fopen("/opt/sdobox/device.token", "r");
  if (tokenFile == NULL) {
    strlcpy(tokenStr, "NOTOKENS", 8);
  } else {
    fgets(tokenStr, 8, tokenFile);
    fclose(tokenFile);
  }
  json_object_set_new(root, "device", json_string(tokenStr));

  // Dump JSON and decref
  s = json_dumps(root, 0);
  json_decref(root);

  // CURL Submit Scorecard to Server
  // if (curl_sdob_submit_scorecard(s, (long)strlen(s)) != 0) {
  //   dbgprintf(DBG_ERROR, "%s\n", "Unable to submit score to server");
  // };
  // char* scoreReply;
  if (libsdob_zmq_scoring_send(s) < 0) {
    dbgprintf(DBG_DEBUG, "Failed to send scoring data");
  }

  return 1;
}

int pg_sdobSubmitScorecard_JRSystem() {
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
      (char*)sdob_judgement->video->local_folder, (char*)sdob_judgement->team, (char*)sdob_judgement->round,
      (char*)sdob_judgement->judge, c_time_string, sdob_judgement->video->video_file,
      libmpvCache->player->duration, sdob_judgement->sowt, csv_score);
  closelog();

  if (csv_score != NULL) { free(csv_score); }
  return 1;
}
