#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <syslog.h>
#include <math.h>
#include <time.h> // system time clocks
#include <unistd.h>
#include <fcntl.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "skydiveorbust.h"
#include "gui/pages.h"

#include "buttons/buttons.h"
#include "queue/queue.h"
#include "mpv/mpv.h"

#include "gui/keyboard/keyboard.h"


void PG_SDOB_SCORECARD_CLEAR_MARKS(struct pg_sdob_scorecard_marks *sc)
{
  sc->size = 0;
  sc->selected = -1;
  sc->last = -1;
  for (size_t s = 0; s < sc->max; s++) {
    sc->arrScorecardId[s] = -1;
    sc->arrScorecardPoints[s] = -1;
    sc->arrScorecardMilli[s] = -1;
    sc->arrScorecardTimes[s] = -1.0;
    sc->arrScorecardTicks[s] = -1.0;
  }
}


// Initialize scorecard marks
struct pg_sdob_scorecard_marks * PG_SDOB_SCORECARD_INIT_MARKS()
{
  struct pg_sdob_scorecard_marks *sc = (struct pg_sdob_scorecard_marks*)malloc(sizeof(struct pg_sdob_scorecard_marks));

  // sc->video_hash = ""; // video filename in meet folder
  // sc->sowt = -1.0;

  sc->max = pg_sdob_scorecard_max;
  sc->tickCnt = 0;

  sc->arrScorecardId = (int*)calloc(sc->max, sizeof(int));
  sc->arrScorecardPoints = (int*)calloc(sc->max, sizeof(int));
  sc->arrScorecardMilli = (int*)calloc(sc->max, sizeof(int));
  sc->arrScorecardTimes = (double*)calloc(sc->max, sizeof(double));
  sc->arrScorecardTicks = (double*)calloc(sc->max, sizeof(double));

  PG_SDOB_SCORECARD_CLEAR_MARKS(sc);

  return sc;
}

// Clear Scorecard Marks,
// Executed thru PG_SDOB_CLEAR_JUDGEMENT


// Initialize Judgement Data and Scorecard Marks
struct pg_sdob_judgement_data * PG_SDOB_INIT_JUDGEMENT() {
  struct pg_sdob_judgement_data *judgement = (struct pg_sdob_judgement_data*)malloc(sizeof(struct pg_sdob_judgement_data));
  judgement->marks = PG_SDOB_SCORECARD_INIT_MARKS();

  judgement->judge = (char*)calloc(64, sizeof(char));
  judgement->video_file = (char*)calloc(256, sizeof(char));
  judgement->meet = (char*)calloc(64, sizeof(char));
  judgement->team = (char*)calloc(128, sizeof(char));
  judgement->teamStr = (char*)calloc(128, sizeof(char));
  judgement->round = (char*)calloc(64, sizeof(char));
  judgement->roundStr = (char*)calloc(64, sizeof(char));

  judgement->sowt = -1.0;
  judgement->workingTime = 0.0;

  judgement->score = 0.00;
  judgement->scoreMax = 0.00;
  judgement->scoreStr = (char*)calloc(32, sizeof(char));
  return judgement;
}

// Clear Judgement Data and Scorecard Marks
void PG_SDOB_CLEAR_JUDGEMENT(struct pg_sdob_judgement_data *judgement)
{
  judgement->sowt = -1.0;
  judgement->workingTime = 0.0;

  judgement->score = 0.00;
  judgement->scoreMax = 0.00;
  CLEAR(judgement->scoreStr, 32);
  PG_SDOB_SCORECARD_CLEAR_MARKS(judgement->marks);
}

// Initialize Player Data
struct pg_sdob_player_data * PG_SDOB_INIT_PLAYER() {
  struct pg_sdob_player_data *player = (struct pg_sdob_player_data*)malloc(sizeof(struct pg_sdob_player_data));
  player->paused = 0;
  player->position = 0.00;
  player->duration = 0.00;
  player->pbrate = 1.00;
  player->pbrateUser = 0.75;
  player->pbrateStr = (char*)calloc(10, sizeof(char));
  player->pbrateUserStr = (char*)calloc(10, sizeof(char));
  player->positionStr = (char*)calloc(32, sizeof(char));
  return player;
}

// Initialize Player Video Chapters
struct pg_sdob_player_chapters * PG_SDOB_INIT_PLAYER_CHAPTERS() {
  struct pg_sdob_player_chapters *chapters = (struct pg_sdob_player_chapters*)malloc(sizeof(struct pg_sdob_player_chapters));
  chapters->len = 0;
  chapters->max = 64;
  chapters->cur = -1;
  chapters->ptr = (double*)calloc(chapters->max, sizeof(double));
  return chapters;
}

// Initialize Player Ticks
struct pg_sdob_player_ticks * PG_SDOB_INIT_PLAYER_TICKS() {
  struct pg_sdob_player_ticks *ticks = (struct pg_sdob_player_ticks*)malloc(sizeof(struct pg_sdob_player_ticks));
  ticks->len = 0;
  ticks->max = 64;
  ticks->lock = 0;
  ticks->ptr = (double*)calloc(ticks->max, sizeof(double));
  return ticks;
}


// Initialize Videolist Files
struct pg_sdob_videolist_files * PG_SDOB_INIT_VIDEOLIST_FILES() {
  struct pg_sdob_videolist_files *files = (struct pg_sdob_videolist_files*)malloc(sizeof(struct pg_sdob_videolist_files));
  files->max = 4096;
  files->len = 256;
  files->size = 0;
  files->cur = -1;
  files->list = (char**)calloc(files->max, sizeof(char*));
  for (int i = 0; i < files->max; ++i) {
    files->list[i] = (char*)calloc(files->len, sizeof(char));
  }
  return files;
}

// Initialize Videolist Folders
struct pg_sdob_videolist_folders * PG_SDOB_INIT_VIDEOLIST_FOLDERS() {
  struct pg_sdob_videolist_folders *folders = (struct pg_sdob_videolist_folders*)malloc(sizeof(struct pg_sdob_videolist_folders));
  folders->max = 128;
  folders->len = 256;
  folders->size = 0;
  folders->cur = -1;
  folders->list = (char**)calloc(folders->max, sizeof(char*));
  for (int i = 0; i < folders->max; ++i) {
    folders->list[i] = (char*)calloc(folders->len, sizeof(char));
  }
  return folders;
}


/*
// Initialize Video Data
struct pg_sdob_video_data * PG_SDOB_INIT_VIDEO() {
  struct pg_sdob_video_data *video = (struct pg_sdob_video_data*)malloc(sizeof(struct pg_sdob_video_data));
  video->duration = 0;
  video->position = 0.00;
  video->chapters = 0;
  return video;
}
*/








/////////////////////////////////////////////
// Page Update Callbacks
//


void pg_sdobPlayerVideoRateChanging(gslc_tsGui *pGui, int changable) {

  // toggle from changing to not changing
  // or
  // force changing off
  if (
      (!changable && pg_sdob_change_video_rate) ||
      changable == 2
  ) {
    pg_sdob_change_video_rate = 0;
    gslc_ElemSetFillEn(pGui, pg_sdobEl[E_SDOB_EL_PL_RATE], false);

  // toggle from not changing to changing
  // or
  // force changing on
  } else if (!changable || changable == 1) {
    pg_sdob_change_video_rate = 1;
    gslc_ElemSetFillEn(pGui, pg_sdobEl[E_SDOB_EL_PL_RATE], true);
  }
}


void pg_sdobSliderSetCurPos(gslc_tsGui *pGui, int slot_scroll) {
  pg_sdob_slot_scroll = slot_scroll;
  gslc_ElemXSliderSetPos(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER], pg_sdob_slot_scroll);
}

// Updated Scorecard Slider Position
void pg_sdobSliderChangeCurPos(gslc_tsGui *pGui, int amt, bool redraw) {
  // Save Current Slider POS as i_slot_old
  int i_slot_old = gslc_ElemXSliderGetPos(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER]);
  i_slot_old = i_slot_old + amt;
  if (i_slot_old < 0) { i_slot_old = pg_sdob_scroll_max; }
  else if (i_slot_old > pg_sdob_scroll_max) { i_slot_old = 0; }

  // Set Slider Pos
  pg_sdobSliderSetCurPos(pGui, i_slot_old);

  // Up scroller indicator
  if (pg_sdob_slot_scroll == 0) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP], GSLC_COL_GRAY_LT1);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  } else {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }

  // Down Scroller indicator
  if (pg_sdob_slot_scroll == pg_sdob_scroll_max) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN], GSLC_COL_GRAY_LT1);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  } else {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }

  if (redraw) {
    gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
  }
}

void pg_sdobSliderResetCurPos(gslc_tsGui *pGui) {
  int i_slot_old = gslc_ElemXSliderGetPos(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER]);
  pg_sdobSliderChangeCurPos(&m_gui, (i_slot_old * -1), true);
}


// Update Scorecard Count
void pg_sdobUpdateCount(gslc_tsGui *pGui, gslc_tsElemRef *pElem) {
  int omission_cost = -2;
  int score = 0, totalScore = sdob_judgement->marks->size, sLen = totalScore;

  for (size_t s = 0; s < sLen; s++) {
    if (s >= sdob_judgement->marks->size) { break; }
    switch (sdob_judgement->marks->arrScorecardPoints[s]) {
      case E_SCORES_BUST:

      break;
      case E_SCORES_OMISSION:
        score += omission_cost;
      break;
      case E_SCORES_POINT:
        score++;
      break;
      case E_SCORES_SOWT:
        totalScore--;
      break;
    }
  }

  // Cannot drop below Zero
  if (score < 0) { score = 0; }
  if (totalScore < 0) { totalScore = 0; }

  sdob_judgement->score = score;
  sdob_judgement->scoreMax = totalScore;
  size_t scoreSz = snprintf(NULL, 0, "%d/%d", score, totalScore) + 1;
  if (scoreSz > 0 && scoreSz <= 32) {
    snprintf(sdob_judgement->scoreStr, scoreSz, "%d/%d", score, totalScore);
  } else {
    strlcpy(sdob_judgement->scoreStr, "?/?", 4);
  }
  gslc_ElemSetTxtStr(pGui, pElem, sdob_judgement->scoreStr);
}




void pg_sdobUpdateVideoRate(gslc_tsGui *pGui, double playback_rate) {
  if (playback_rate < 0) { return; }
  sdob_player->pbrate = playback_rate;
  int pbPercent = (int)round(playback_rate * 100);
  size_t pbrateSz = snprintf(NULL, 0, "%d%%", pbPercent) + 1;
  if (pbrateSz > 0 && pbrateSz <= 10) {
    snprintf(sdob_player->pbrateStr, pbrateSz, "%d%%", pbPercent);
    gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_PL_RATE], sdob_player->pbrateStr);
  }
}

void pg_sdobUpdateUserDefinedVideoRate(gslc_tsGui *pGui, double playback_rate) {
  if (playback_rate < 0) { return; }
  pg_sdob_player_playback_slow = playback_rate;

  sdob_player->pbrateUser = playback_rate;
  int pbPercent = (int)round(playback_rate * 100);
  size_t pbPercentSz = snprintf(NULL, 0, "%d%%", pbPercent) + 1;
  if (pbPercentSz > 0 && pbPercentSz <= 10) {
    snprintf(sdob_player->pbrateUserStr, pbPercentSz, "%d%%", pbPercent);
    gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], sdob_player->pbrateUserStr);
  }
}


void pg_sdobUpdateJudgeInitials(gslc_tsGui *pGui, char* str) {
  strlcpy(sdob_judgement->judge, str, 64);
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_JUDGE_INITIALS], sdob_judgement->judge);
}

void pg_sdobUpdateMeet(gslc_tsGui *pGui, char* str) {
  strlcpy(sdob_judgement->meet, str, 64);
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_MEET_DESC], sdob_judgement->meet);
}

void pg_sdobUpdateVideoDesc(gslc_tsGui *pGui, char* str) {
  strlcpy(sdob_judgement->video_file, str, 64);
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_VIDEO_DESC], sdob_judgement->video_file);
}


void pg_sdobUpdateTeam(gslc_tsGui *pGui, char* str) {
  size_t dispSz = snprintf(NULL, 0, "T:%s", str) + 1;
  if (dispSz > 0 && dispSz <= 128) {
    strlcpy(sdob_judgement->team, str, 128);
    snprintf(sdob_judgement->teamStr, dispSz, "T:%s", str);
    gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_TEAM_DESC], sdob_judgement->teamStr);
  }
}


void pg_sdobUpdateRound(gslc_tsGui *pGui, char* str) {
  size_t dispSz = snprintf(NULL, 0, "RND:%s", str) + 1;
  if (dispSz > 0 && dispSz <= 64) {
    strlcpy(sdob_judgement->round, str, 64);
    snprintf(sdob_judgement->roundStr, dispSz, "RND:%s", sdob_judgement->round);
    gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_ROUND_DESC], sdob_judgement->roundStr);
  }
}


void pg_sdobUpdatePlayerSlider(gslc_tsGui *pGui) {
  if (pg_sdob_player_move_timepos_lock == 1) { return; }
  pg_sdob_player_move_timepos_lock = 1;

  char* retTime;
  if ((mpvSocketSinglet("time-pos", &retTime)) != -1) {
    // debug_print("Timepos: %s\n", retTime);
    if (retTime == NULL) {
      pg_sdob_player_move_timepos_lock = 0;
      return;
    } else
    if (pg_sdob_player_move > -1) {
      pg_sdob_player_move_timepos_lock = 0;
      free(retTime);
      return;
    }

    sdob_player->position = atof(retTime);
    free(retTime);
  } else {
    sdob_player->position = -1;
  }

  if (sdob_judgement->sowt == -1.0) {

    secs_to_time((int)(sdob_player->position * 1000000), sdob_player->positionStr, 32);
    setSliderPosByTime(pGui);
  } else {
    double insideWorkingTime = (sdob_player->position - sdob_judgement->sowt);
    secs_to_time((int)(insideWorkingTime * 1000000), sdob_player->positionStr, 32);

    int nTick = (int)((insideWorkingTime * 1000) / sdob_judgement->workingTime);
    if (nTick >= 0) {
      gslc_ElemXSliderSetPos(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], nTick);
    }
  }
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], sdob_player->positionStr);

  pg_sdob_player_move_timepos_lock = 0;
}






bool pg_sdobPlayerCbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider* pSlider   = (gslc_tsXSlider*)(pElem->pXData);

  // Moved Slider by Touchscreen
  if (
    // Started Touch/Drag
    pSlider->eTouch == GSLC_TOUCH_DOWN_IN
    || pSlider->eTouch == GSLC_TOUCH_MOVE_IN
    || pSlider->eTouch == GSLC_TOUCH_MOVE_OUT
    // || pSlider->eTouch == GSLC_TOUCH_UP_IN
  ) {
    double move_tmp = (double)((double)gslc_ElemXSliderGetPos(pGui,pElemRef) * .1);
    // printf("Moved Slider %f, Event: %d\n", move_tmp, pSlider->eTouch);

    if (move_tmp < 3) { move_tmp = 0;
    } else if (move_tmp > 997) { move_tmp = 1000;
    }

    // Reset Debounce when Moved enough
    if (fabs(move_tmp - pg_sdob_player_move) > 4) {
      pg_sdob_player_move_debounce = millis();
    }
    if (pg_sdob_player_move == -1) {
      gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], "MOVING...");
    }
    pg_sdob_player_move = move_tmp;
  } else if (pSlider->eTouch) {
    // debug_print("Untracked TOUCH %d\n", pSlider->eTouch);
  }

  // Update the color box
//  gslc_tsElemRef* pElemColor = gslc_PageFindElemById(pGui,E_PG_MAIN,E_ELEM_COLOR);
//  gslc_ElemSetCol(pGui,pElemColor,GSLC_COL_WHITE,colRGB,GSLC_COL_WHITE);

  return 1;
}





/////////////////////////////////////////////
// Scroll Scorecard Box
//
bool pg_sdobScorecardCbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider* pSlider   = (gslc_tsXSlider*)(pElem->pXData);

  // // debug_print("Slider Ref: %d,  %d\n", pElem->nId, pSlider->eTouch);
  // Fetch the new RGB component from the slider
  switch (pElem->nId) {
    case E_SLIDER_SCORECARD:
      // Moved by Touchscreen, Force Redraw
      if (pSlider->eTouch == GSLC_TOUCH_DOWN_IN ||
          pSlider->eTouch == GSLC_TOUCH_MOVE_IN ||
          pSlider->eTouch == GSLC_TOUCH_MOVE_OUT
      ) {
        pg_sdob_slot_scroll = gslc_ElemXSliderGetPos(pGui,pElemRef);
        // debug_print("Scorecard Scroll: %d\n", pg_sdob_slot_scroll);

        // if (pg_sdobEl[E_SDOB_EL_BOX]) {
        //  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
        // }
      }
    default:
      break;
  }
  return 1;
}



void pg_sdobSliderChangeMaxPos(gslc_tsGui *pGui, int amt, bool redraw) {
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER]);
  gslc_tsXSlider* pSlider   = (gslc_tsXSlider*)(pElem->pXData);


  // gslc_ElemXSliderSetPos(pGui, pSliderElemRef, i_slot_scroll);
  pSlider->nPosMax += amt;
  if (pSlider->nPosMax < 1) { pSlider->nPosMax = 1; }
  pg_sdob_scroll_max = pSlider->nPosMax;
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER], GSLC_REDRAW_FULL);

  // debug_print("Change Slider: %d - %d\n", amt, pg_sdob_scroll_max);
  if (redraw) {
    gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
  }
}



bool pg_sdobScorecardDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{

  // pg_sdob_score_count = db_video_score_count(1);
  // int *arrMarks = (int*)calloc(pg_sdob_score_count, sizeof(int));
  // double *arrTimes  = (double*)calloc(pg_sdob_score_count, sizeof(double));
  // db_video_scores(1, arrMarks, arrTimes);

  pg_sdob_score_count = sdob_judgement->marks->size;

  // free(arrScorecardPoint);
  // free(arrScorecardTime);
  // arrScorecardPoint = arrMarks;
  // arrScorecardTime = arrTimes;

  int i_line_start = pg_sdob_slot_scroll; // capture scroll position;
  int i_line = 0; // index for display line
  int i_slot = 0; // Index for slot on display line
  int i_start_mark = i_line_start * pg_sdob_slot_max; // initial point for top left slot
  int i_start_disp = 0; // 0; added to display, use 1; for counting sowt as first point

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);

  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);
  gslc_DrawFrameRect(pGui, pRect, pElem->colElemFrame);

  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);
  // gslc_ElemDraw(pGui, E_PG_SKYDIVEORBUST, E_ELEM_SC_POINT_COUNT);
  // gslc_ElemDraw(pGui, E_PG_SKYDIVEORBUST, E_ELEM_SC_SUBMIT);
  // // debug_print("count: %d, start line: %d\n", pg_sdob_score_count, i_line_start);

  for(i_line = 0; i_line < pg_sdob_line_max; i_line++) {
    for(i_slot = 0; i_slot < pg_sdob_slot_max; i_slot++) {

      int iXCnt = i_slot + (i_line * pg_sdob_slot_max); // slot identifier
      int i_this_mark = iXCnt + i_start_mark;
      char score[16];
      char result[16];

      // gslc_tsElemRef *elem_poi = gslc_PageFindElemById(pGui, E_PG_SKYDIVEORBUST, iXPM);
      // gslc_tsElemRef *elem_res = gslc_PageFindElemById(pGui, E_PG_SKYDIVEORBUST, iXPM);

      // // debug_print("Line: %d, Slot: %d, iX: %d, Mark: %d\n", i_line, i_slot, iXCnt, i_this_mark);
      // Show full rounded number
      if (i_this_mark == 0) {
        sprintf(score, "%s", "S");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_BROWN);
      } else if (i_this_mark < pg_sdob_score_count && (i_this_mark + i_start_disp) % 10 == 0) {
        sprintf(score, "%d", (i_this_mark + i_start_disp));
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_BROWN);
      } else if (i_this_mark < pg_sdob_score_count) {
        sprintf(score, "%d", (i_this_mark + i_start_disp) % 10);
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_YELLOW);
      } else {
        sprintf(score, "%s", " ");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_BLACK);
      }

      // // debug_print("Selected: %d, This: %d, Mark: %d\n", sdob_judgement->marks->selected, i_this_mark, sdob_judgement->marks->arrScorecardPoints[i_this_mark]);
      if (sdob_judgement->marks->selected == i_this_mark) {
        gslc_ElemSetFillEn(pGui, pg_sdob_scorecard_elemsNum[iXCnt], true);
      } else { gslc_ElemSetFillEn(pGui, pg_sdob_scorecard_elemsNum[iXCnt], false); }

      gslc_ElemSetTxtStr(pGui, pg_sdob_scorecard_elemsNum[iXCnt], score);

      // Score Result
      if (i_this_mark < pg_sdob_score_count) {
        // // debug_print("Mark: %d, Milli: %d, Point: %d\n", i_this_mark, sdob_judgement->marks->arrScorecardPoints[i_this_mark], sdob_judgement->marks->arrScorecardMilli[i_this_mark]);
        switch (sdob_judgement->marks->arrScorecardPoints[i_this_mark]) {
          case E_SCORES_POINT:
            sprintf(result, "%s", "/");
            gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsMark[iXCnt], GSLC_COL_GREEN);
          break;
          case E_SCORES_BUST:
            sprintf(result, "%s", "O");
            gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsMark[iXCnt], GSLC_COL_RED);
          break;
          case E_SCORES_OMISSION:
            sprintf(result, "%s", "X");
            gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsMark[iXCnt], GSLC_COL_YELLOW);
          break;
          case E_SCORES_SOWT:
            sprintf(result, "%s", "-");
            gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsMark[iXCnt], GSLC_COL_RED_DK1);
          break;
          default:
            sprintf(result, "%s", " ");
            gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsMark[iXCnt], GSLC_COL_BLACK);
          break;
        }
      } else {
        sprintf(result, "%s", " ");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsMark[iXCnt], GSLC_COL_BLACK);
      }

      if (sdob_judgement->marks->selected == i_this_mark) {
        gslc_ElemSetFillEn(pGui, pg_sdob_scorecard_elemsMark[iXCnt], true);
      } else { gslc_ElemSetFillEn(pGui, pg_sdob_scorecard_elemsMark[iXCnt], false); }

      gslc_ElemSetTxtStr(pGui, pg_sdob_scorecard_elemsMark[iXCnt], result);

      // if (!m_scorecard_is_submitting) {
      //   gslc_ElemDraw(pGui, E_PG_SKYDIVEORBUST, iXPM);
      // }
    }
  }


  return true;
}



/////////////////////////////////////////////
// Page Button Callbacks
//
// Slider Up
bool pg_sdobCbBtnSliderUp(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_sdobScoringSelectionLastHidden(pGui);

  pg_sdobSliderChangeCurPos(pGui, -1, true);

  return true;
}
// Slider Down
bool pg_sdobCbBtnSliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Clear selection so people don't get lost
  pg_sdobScoringSelectionLastHidden(pGui);
  pg_sdobSliderChangeCurPos(pGui, 1, true);
  return true;
}

// Change Video Rate
bool pg_sdobCbBtnVideoRate(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // toggle video rate
  pg_sdobPlayerVideoRateChanging(pGui, 0);


  return true;
}


// Change Judge
bool pg_sdobCbBtnChangeJudge(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  pg_keyboard_show(pGui, 3, sdob_judgement->judge, &pg_sdobUpdateJudgeInitials);
  return true;
}


// Meet Desc
bool pg_sdobCbBtnMeetDesc(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SDOB_VIDEOLIST);

  return true;
}

// Video Desc
bool pg_sdobCbBtnVideoDesc(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SDOB_VIDEOLIST);
  return true;
}

// Team Desc
bool pg_sdobCbBtnTeamDesc(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_keyboard_show(pGui, 6, sdob_judgement->team, &pg_sdobUpdateTeam);
//  setKeyboardCallbackFunc(&pg_sdobUpdateTeam);
//  strcpy(keyboardInput, sdob_judgement->team);
//  m_show_keyboard = 6;
//  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_TEAM_DESC], sdob_judgement->team);

  return true;
}

// Round Desc
bool pg_sdobCbBtnRoundDesc(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  pg_keyboard_show(pGui, 3, sdob_judgement->round, &pg_sdobUpdateRound);
//  setKeyboardCallbackFunc(&pg_sdobUpdateRound);
//  strcpy(keyboardInput, sdob_judgement->round);
//  m_show_keyboard = 3;
//  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_ROUND_DESC], sdob_judgement->round);

  return true;
}



// Scorecard Count
bool pg_sdobCbBtnScCount(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SDOB_VIDEOLIST);
  return true;
}

// Submit Button
bool pg_sdobCbBtnSubmitBtn(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  
/*
  struct queue_head *item = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(item);
  item->action = E_Q_SCORECARD_SUBMIT_SCORECARD;
  queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
*/
  touchscreenPageOpen(pGui, E_PG_SDOB_SUBMIT);

  return true;
}


// Pause Callback
bool pg_sdobPlCbBtnPause(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Stop Changing Video Rate
  pg_sdobPlayerVideoRateChanging(pGui, 2);

  // Stop Clearing Scorecard Selection
  if (sdob_judgement->marks->selected > 0) {
    pg_sdobScoringSelectionClear(pGui);
  }

  pg_sdob_player_pause(1);
  return true;
}


// Play Callback
bool pg_sdobPlCbBtnPlay(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Stop Changing Video Rate
  pg_sdobPlayerVideoRateChanging(pGui, 2);

  // Stop Clearing Scorecard Selection
  if (sdob_judgement->marks->selected > 0) {
    pg_sdobScoringSelectionClear(pGui);
  }

  // Toggle Playing
  pg_sdob_player_toggle_play();
  // update video rate as a double check
  pg_sdobUpdateVideoRate(pGui, mpv_video_rate);

  return true;
}

// Forward Callback
bool pg_sdobPlCbBtnForward(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  char* retSpeed;
  double mpvSpeed;

  // Stop Changing Video Rate
  pg_sdobPlayerVideoRateChanging(pGui, 2);

  // Stop Clearing Scorecard Selection
  if (sdob_judgement->marks->selected > 0) {
    pg_sdobScoringSelectionClear(pGui);
  }

  if (mpvSocketSinglet("speed", &retSpeed) != -1) {
    mpvSpeed = atof(retSpeed);
    free(retSpeed);
    if (mpvSpeed == 0) {
      // printf("Bad Speed: %s", retSpeed);
      return true;
    }

    if (mpvSpeed == 2.0) {
      pg_sdobUpdateVideoRate(pGui, mpv_speed(4.0));
    } else if (mpvSpeed == 1.0) {
      pg_sdobUpdateVideoRate(pGui, mpv_speed(2.0));
    } else {
      pg_sdobUpdateVideoRate(pGui, mpv_speed(1.0));
    }
    mpv_set_prop_char("pause", "no");
  }
  return true;
}


// Back Callback
bool pg_sdobPlCbBtnBack(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  char* retSpeed;
  double mpvSpeed;
  char* retTimePos;
  double timePos;

  // Stop Changing Video Rate
  pg_sdobPlayerVideoRateChanging(pGui, 2);

  // Stop Clearing Scorecard Selection
  if (sdob_judgement->marks->selected > 0) {
    pg_sdobScoringSelectionClear(pGui);
  }

  if (mpvSocketSinglet("speed", &retSpeed) != -1) {
    mpvSpeed = atof(retSpeed);
    free(retSpeed);
    if (mpvSpeed == 0) { mpvSpeed = 1.0; }
  } else { mpvSpeed = 1.0; }

  if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
    timePos = atof(retTimePos);
    free(retTimePos);
  // } else if (m_video_pos) {
  //  timePos = (double)time_to_secs(m_video_pos);
  } else {
    timePos = -1;
  }

  if (timePos > 0) {
    timePos += (-1.5 * mpvSpeed);

    size_t cmdSz = strlen("show-text \"Seeking\" 750\n") + 1;
    char *cmd = (char*)malloc(cmdSz * sizeof(char));
    strlcpy(cmd, "show-text \"Seeking\" 750\n", cmdSz);
    mpv_cmd(cmd);

    size_t cmdSeekSz = snprintf(NULL, 0, "seek %f %s\n", timePos, "absolute+exact") + 1;
    char *cmdSeek = (char*)malloc(cmdSeekSz * sizeof(char));
    snprintf(cmdSeek, cmdSeekSz, "seek %f %s\n", timePos, "absolute+exact");
    mpv_cmd(cmdSeek);

  }
  return true;
}


// Defined Slow Callback
bool pg_sdobPlCbBtnUserDefinedSlow(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  char* retSpeed;
  double dSpeed = 1.0;

  // Stop Changing Video Rate
  pg_sdobPlayerVideoRateChanging(pGui, 2);

  // Stop Clearing Scorecard Selection
  if (sdob_judgement->marks->selected > 0) {
    pg_sdobScoringSelectionClear(pGui);
  }

  if (mpvSocketSinglet("speed", &retSpeed) != -1) {
    dSpeed = atof(retSpeed);
    free(retSpeed);
    if (dSpeed >= 1.0) {
      pg_sdobUpdateVideoRate(pGui, mpv_speed(pg_sdob_player_playback_slow));
    }
  }
  pg_sdob_player_pause(0);
  return true;
}




bool pg_skydiveorbustCbBtn(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;
  // struct queue_head *item;

  if (eTouch == GSLC_TOUCH_UP_IN) {
    for (size_t i = 0; i < 30; i++) {
      if (
        ((gslc_tsElem*)pg_sdob_scorecard_elemsNum[i]->pElem)->nId == pElem->nId ||
        ((gslc_tsElem*)pg_sdob_scorecard_elemsMark[i]->pElem)->nId == pElem->nId
      ) {
        int wantedSel = i + (pg_sdob_slot_scroll * pg_sdob_slot_max);
        if (wantedSel < sdob_judgement->marks->size) {
          sdob_judgement->marks->selected = wantedSel;
        } else {
          sdob_judgement->marks->selected = -1;
        }

        // // debug_print("Sel: %d, T: %f\n", i, sdob_judgement->marks->arrScorecardTimes[i]);
        gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

        // if (sdob_judgement->marks->selected == 0) {
        //   pg_sdob_player_pause(1);
        // }
        if (sdob_judgement->marks->selected > -1 &&
            sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected] >= 0
        ) {
          mpv_seek_arg(sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected], "absolute+exact");
        }
        break;
      }
    }
    /*
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
      default:

        pId = pElem->nId;
        if (pId >= 13000) {
          pId -= 13000;
        } else {
          pId -= 10000;
        }
        sdob_judgement->marks->selected = pId;

        // debug_print("Score: %d, Sel: %d, T: %f\n", pId, sdob_judgement->marks->selected, sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected]);
        gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

        if (sdob_judgement->marks->selected >= 0) {
          pg_sdob_player_pause(1);
        }
        if (sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected] >= 0) {
          mpv_seek_arg(sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected], "absolute+exact");
        }
      break;
    }
    */

/*
      case E_SLIDER_BTN_UP:
        pg_skydiveorbustSliderChangeCurPos(pGui, -1, true);
      break;
      case E_SLIDER_BTN_DOWN:
        pg_skydiveorbustSliderChangeCurPos(pGui, 1, true);
      break;

      case E_ELEM_SC_JUDGE_INIT:
        // Change Judge
        // printf("SC Callback\n");
        setKeyboardCallbackFunc(&pg_sdobUpdateJudgeInitials);
        strcpy(keyboardInput, sdob_judgement->judge);
        m_show_keyboard = 3;
        gslc_ElemSetTxtStr(&m_gui, pg_sdobEl[E_SDOB_EL_JUDGE_INITIALS], sdob_judgement->judge);
      break;

      case E_ELEM_SC_TEAM:
        // Change Team
        setKeyboardCallbackFunc(&pg_skydiveorbustUpdateTeam);
        strcpy(keyboardInput, sdob_judgement->team);
        m_show_keyboard = 6;
      break;
      case E_ELEM_SC_ROUND:
        // Change Round
        setKeyboardCallbackFunc(&pg_skydiveorbustUpdateRound);
        strcpy(keyboardInput, sdob_judgement->round);
        m_show_keyboard = 3;
      break;
      case E_ELEM_SC_SUBMIT:
        item = malloc(sizeof(struct queue_head));
        INIT_QUEUE_HEAD(item);
        item->action = E_Q_SCORECARD_SUBMIT_SCORECARD;
        queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
      break;
      case E_ELEM_SC_CLEAR:
        PG_SDOB_CLEAR_JUDGEMENT(sdob_judgement);
      break;
      default:
        pId = pElem->nId;
        if (pId >= 13000) {
          pId -= 13000;
        } else {
          pId -= 10000;
        }
        sdob_judgement->marks->selected = pId;

        // debug_print("Score: %d, Sel: %d, T: %f\n", pId, sdob_judgement->marks->selected, sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected]);
        gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

        if (sdob_judgement->marks->selected >= 0) {
          struct queue_head *item = malloc(sizeof(struct queue_head));
          INIT_QUEUE_HEAD(item);
          item->action = E_Q_ACTION_MPV_COMMAND;
          item->cmd = "set pause yes\n";
          queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
        }
        if (sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected] >= 0) {

          mpv_seek_arg(sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected], "absolute+exact");
        }
      break;
    }
*/
  }
  return true;
}




/////////////////////////////////////////////
// GUI Slice Init
//
void pg_sdobGuiInit(gslc_tsGui *pGui, gslc_tsRect pRect) {
  int ePage = E_PG_SKYDIVEORBUST;

  gslc_PageAdd(pGui, ePage, pg_sdobElem, pg_sdobElTotal,
               pg_sdobElemRef, E_SDOB_EL_MAX + 100);

  if ((
    pg_sdobEl[E_SDOB_CLEAN] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, (gslc_tsRect){0, 0, 480, 320})
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_CLEAN], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }

  //////////////////////////////////////////
  // Create vertical scrollbar for scorecard
  if ((
    pg_sdobEl[E_SDOB_EL_SC_SLIDER] = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO,
        ePage, &pg_sdob_sc_slider, (gslc_tsRect){442,145,40,60},
        0, pg_sdob_scroll_max, 0, 10, true)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER], GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemXSliderSetStyle(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER], true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
    gslc_ElemXSliderSetPosFunc(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER], &pg_sdobScorecardCbSlidePos);
  }

  //////////////////////////////////////////
  // Create vertical scrollbar Up Arrow
  if ((
    pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
        ePage, (gslc_tsRect){440,110,40,30}, (char*)"^",
        0, E_FONT_MONO18, &pg_sdobCbBtnSliderUp)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP], true);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_UP], GSLC_ALIGN_MID_MID);
  }

  //////////////////////////////////////////
  // Create vertical scrollbar Down Arrow
  if ((
    pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
        ePage, (gslc_tsRect){440,210,40,30}, (char*)"v",
        0, E_FONT_MONO18, &pg_sdobCbBtnSliderDown)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN], GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN], true);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_SC_SLIDER_DOWN], GSLC_ALIGN_MID_MID);
  }


  // Judge Initials
  if ((
    pg_sdobEl[E_SDOB_EL_JUDGE_INITIALS] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
        ePage, (gslc_tsRect) {3,71,53,38},
        (char*)" ", 0, E_FONT_MONO28, &pg_sdobCbBtnChangeJudge)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_JUDGE_INITIALS], GSLC_COL_BLUE);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_JUDGE_INITIALS], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_JUDGE_INITIALS], GSLC_ALIGN_MID_MID);
  }

  // Total Scored Points
  if ((
    pg_sdobEl[E_SDOB_EL_SC_COUNT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
        ePage, (gslc_tsRect) {245,71,88,38},
        (char*)" ", 0, E_FONT_MONO18, &pg_sdobCbBtnScCount)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT], false);
  }

  // Video File Description
  if ((
    pg_sdobEl[E_SDOB_EL_VIDEO_DESC] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect) {0,260,230,20},
          (char*)" ", 0, E_FONT_MONO18, &pg_sdobCbBtnVideoDesc)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_VIDEO_DESC], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_VIDEO_DESC],GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_VIDEO_DESC],GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_VIDEO_DESC],false);
  }

  // Meet Description
  if ((
    pg_sdobEl[E_SDOB_EL_MEET_DESC] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect) {0,240,230,20},
          (char*)" ", 0, E_FONT_MONO18, &pg_sdobCbBtnMeetDesc)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_MEET_DESC], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_MEET_DESC], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_MEET_DESC], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_MEET_DESC], false);
  }

  // Team Description
  if ((
    pg_sdobEl[E_SDOB_EL_TEAM_DESC] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect) {65,71,100,38},
          (char*)" ", 0, E_FONT_MONO18, &pg_sdobCbBtnTeamDesc)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_TEAM_DESC], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_TEAM_DESC], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_TEAM_DESC], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_TEAM_DESC], false);
  }

  // Round Description
  if ((
    pg_sdobEl[E_SDOB_EL_ROUND_DESC] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect) {165,71,80,38},
          (char*)" ", 0, E_FONT_MONO18, &pg_sdobCbBtnRoundDesc)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_ROUND_DESC], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_ROUND_DESC], GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_ROUND_DESC], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_ROUND_DESC], false);
  }

 // Submit Button
  if ((
    pg_sdobEl[E_SDOB_EL_BTN_SUBMIT] = gslc_ElemCreateBtnTxt(pGui, E_ELEM_SC_SUBMIT,
          ePage, (gslc_tsRect) {333,71,107,38},
          (char*)"Submit", 0, E_FONT_MONO18, &pg_sdobCbBtnSubmitBtn)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_BTN_SUBMIT], GSLC_COL_BLACK);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_BTN_SUBMIT], GSLC_COL_GRAY, GSLC_COL_RED_DK1, GSLC_COL_GREEN);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_BTN_SUBMIT], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobEl[E_SDOB_EL_BTN_SUBMIT], true);
  }

//  printf("Slot Max: %d, Line Max: %d\n", pg_sdob_slot_max, pg_sdob_line_max);

  //////////////////////////////////////////
  // Create Scorecard Box
  if ((
    pg_sdobEl[E_SDOB_EL_BOX] = gslc_ElemCreateBox(pGui, GSLC_ID_AUTO, ePage, pRect)
  ) != NULL) {
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);
    // Set the callback function to handle all drawing for the element
    gslc_ElemSetDrawFunc(pGui, pg_sdobEl[E_SDOB_EL_BOX], &pg_sdobScorecardDraw);
    // Set the callback function to update content automatically
    // gslc_ElemSetTickFunc(pGui, pg_sdobEl[E_SDOB_EL_BOX], NULL);
  }

  // Build Scorecard Elements
  int i_xSpace = (int)(pRect.w / pg_sdob_slot_max); // width between slot columns
  int i_ySpace = 60; // height between slot rows
  int i_line, i_slot, iXCnt, iX, iY, iH;

  // Generate Initial Elements for Scorebuttons
  for(i_line = 0; i_line < pg_sdob_line_max; i_line++) {
    for(i_slot = 0; i_slot < pg_sdob_slot_max; i_slot++) {

      // Determine Point Location on Scorecard
      iXCnt = (i_slot + (i_line * pg_sdob_slot_max)); // slot display identifier
      iX = (1 + (i_slot * i_xSpace));
      iY = (120 + (i_line * i_ySpace));
      iH = 26;

//      printf("Id: %d, Slot %d, Line: %d, Slot Max: %d - X: %d\n", iXCnt, i_slot, i_line, pg_sdob_slot_max, iX);


      // Point #
      if ((
        pg_sdob_scorecard_elemsNum[iXCnt] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {iX,iY,25,iH}, (char*)" ", 0, E_FONT_MONO24, &pg_skydiveorbustCbBtn)
      ) != NULL ) {
        gslc_ElemSetCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_BLACK, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK);
        gslc_ElemSetTxtAlign(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_ALIGN_MID_MID);
        gslc_ElemSetFillEn(pGui, pg_sdob_scorecard_elemsNum[iXCnt], false);
        gslc_ElemSetFrameEn(pGui, pg_sdob_scorecard_elemsNum[iXCnt], false);
        gslc_ElemSetGlowEn(pGui, pg_sdob_scorecard_elemsNum[iXCnt], false);
      }

      // Scored Point
      if ((
        pg_sdob_scorecard_elemsMark[iXCnt] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
            (gslc_tsRect) {iX,iY+iH,25,iH}, (char*)" ", 0, E_FONT_MONO24, &pg_skydiveorbustCbBtn)
      ) != NULL ) {
        // Result Style Properties
        gslc_ElemSetCol(pGui, pg_sdob_scorecard_elemsMark[iXCnt], GSLC_COL_BLACK, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK);
        gslc_ElemSetTxtAlign(pGui, pg_sdob_scorecard_elemsMark[iXCnt], GSLC_ALIGN_MID_MID);
        gslc_ElemSetFillEn(pGui, pg_sdob_scorecard_elemsMark[iXCnt], false);
        gslc_ElemSetFrameEn(pGui, pg_sdob_scorecard_elemsMark[iXCnt], false);
        gslc_ElemSetGlowEn(pGui, pg_sdob_scorecard_elemsMark[iXCnt], false);
      }


    }
  }







  //////////////////////////////////////////
  // Create Player Controls

  // Pause
  if ((
    pg_sdobEl[E_SDOB_EL_PL_PAUSE] = gslc_ElemCreateBtnImg(pGui, GSLC_ID_AUTO,
        ePage, (gslc_tsRect){208,0,64,64},
        gslc_GetImageFromFile(IMG_BTN_PAUSE,GSLC_IMGREF_FMT_BMP16),
        gslc_GetImageFromFile(IMG_BTN_PAUSE_SEL,GSLC_IMGREF_FMT_BMP16),
        &pg_sdobPlCbBtnPause)
  ) != NULL) {

  }

  // Play
  if ((
    pg_sdobEl[E_SDOB_EL_PL_PLAY] = gslc_ElemCreateBtnImg(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect){312,0,64,64},
          gslc_GetImageFromFile(IMG_BTN_PLAY,GSLC_IMGREF_FMT_BMP16),
          gslc_GetImageFromFile(IMG_BTN_PLAY_SEL,GSLC_IMGREF_FMT_BMP16),
          &pg_sdobPlCbBtnPlay)
  ) != NULL) {

  }

  // Forward
  if ((
    pg_sdobEl[E_SDOB_EL_PL_FORWARD] = gslc_ElemCreateBtnImg(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect){416,0,64,64},
          gslc_GetImageFromFile(IMG_BTN_FORWARD,GSLC_IMGREF_FMT_BMP16),
          gslc_GetImageFromFile(IMG_BTN_FORWARD_SEL,GSLC_IMGREF_FMT_BMP16),
          &pg_sdobPlCbBtnForward)
  ) != NULL) {

  }

  // Back
  if ((
    pg_sdobEl[E_SDOB_EL_PL_BACK] = gslc_ElemCreateBtnImg(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect){0,0,64,64},
          gslc_GetImageFromFile(IMG_BTN_BACK,GSLC_IMGREF_FMT_BMP16),
          gslc_GetImageFromFile(IMG_BTN_BACK_SEL,GSLC_IMGREF_FMT_BMP16),
          &pg_sdobPlCbBtnBack)
  ) != NULL) {

  }


  // Video User Defined Playback Rate
  if ((
    pg_sdobEl[E_SDOB_EL_PL_USER_RATE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
        ePage, (gslc_tsRect) {104,0,64,64},
        (char*)" ", 0,
        E_FONT_MONO2, &pg_sdobPlCbBtnUserDefinedSlow)
  ) != NULL) {

    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], GSLC_COL_WHITE);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], false);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], true);
  }


  // Slider
  if ((
    pg_sdobEl[E_SDOB_EL_PL_SLIDER] = gslc_ElemXSliderCreate(pGui, GSLC_ID_AUTO,
          ePage, &pg_sdob_pl_slider, (gslc_tsRect){15,285,450,35},
          0, 1000, m_nPosPlayer, 10, false)
  ) != NULL) {

    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_COL_RED, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetFillEn(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], false);
    gslc_ElemXSliderSetStyle(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], true, GSLC_COL_RED_DK4, 0, 25, GSLC_COL_GRAY_DK2);
    gslc_ElemXSliderSetPosFunc(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], &pg_sdobPlayerCbSlidePos);
  }


  // Video Position Timestamp
  if ((
    pg_sdobEl[E_SDOB_EL_PL_POS] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO,
        ePage, (gslc_tsRect) {230,245,170,35},
        (char*)" ", 0, E_FONT_MONO2)
  ) != NULL) {

    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], GSLC_COL_ORANGE);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], false);
  }


  // Video Rate
  if ((
    pg_sdobEl[E_SDOB_EL_PL_RATE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect){400,245,80,35},
          (char*)" ", 0, E_FONT_MONO2, &pg_sdobCbBtnVideoRate)
  ) != NULL) {

    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_PL_RATE], GSLC_COL_ORANGE);
    gslc_ElemSetCol(pGui,pg_sdobEl[E_SDOB_EL_PL_RATE], GSLC_COL_WHITE, GSLC_COL_BLUE, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui,pg_sdobEl[E_SDOB_EL_PL_RATE], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui,pg_sdobEl[E_SDOB_EL_PL_RATE], false);
    gslc_ElemSetFrameEn(pGui,pg_sdobEl[E_SDOB_EL_PL_RATE ],false);
  }

/*
// Video Duration Timestamp
  pElemRef = gslc_ElemCreateTxt(pGui, E_ELEM_VIDEO_DURATION, E_PG_PLAYER,
          (gslc_tsRect) {10,185,460,25}, m_video_duration, strlen(m_video_duration), E_FONT_MONO24);
  gslc_ElemSetTxtCol(pGui, pElemRef, GSLC_COL_ORANGE);
  gslc_ElemSetCol(pGui,pElemRef,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui,pElemRef,GSLC_ALIGN_MID_RIGHT);
  m_pElemVideoDuration = pElemRef; // Save for quick access
*/

}














/////////////////////////////////////////////
// Buttons
//
void pg_skydiveorbustButtonRotaryCW() {
  struct queue_head *item;

  char* cmdFSTxt = "frame-step\n";
  size_t cmdFSSz = strlen(cmdFSTxt) + 1;

  if (pg_sdob_change_video_rate) {
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_ACTION_VIDEO_RATE_ADJUST;
    item->amt = .05;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    struct queue_head *itemUser = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemUser);
    itemUser->action = E_Q_ACTION_VIDEO_RATE_USER;
    queue_put(itemUser, pg_sdobQueue, &pg_sdobQueueLen);


  } else if (sdob_judgement->marks->selected == 0) {
    // Adjust SOWT Framestep
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_ACTION_MPV_COMMAND;
    if (cmdFSSz > 0) {
      item->cmd = (char*)malloc(cmdFSSz * sizeof(char));
      strlcpy(item->cmd, cmdFSTxt, cmdFSSz);
      queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    }

  } else if (sdob_judgement->marks->selected < 0) {
    // Framestep
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_ACTION_MPV_COMMAND;
    if (cmdFSSz > 0) {
      item->cmd = (char*)malloc(cmdFSSz * sizeof(char));
      strlcpy(item->cmd, cmdFSTxt, cmdFSSz);
      queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    }
  } else {
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_MOVE_SCORE_SELECTED;
    item->amt = 1;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  }
}

void pg_skydiveorbustButtonRotaryCCW() {
  struct queue_head *item;
  char* cmdFBSTxt = "frame-back-step\n";
  size_t cmdFBSSz = strlen(cmdFBSTxt) + 1;

  if (pg_sdob_change_video_rate) {
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_ACTION_VIDEO_RATE_ADJUST;
    item->amt = -.05;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    struct queue_head *itemUser = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemUser);
    itemUser->action = E_Q_ACTION_VIDEO_RATE_USER;
    queue_put(itemUser, pg_sdobQueue, &pg_sdobQueueLen);
  } else if (sdob_judgement->marks->selected == 0) {
    // Adjust SOWT Framestep
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_ACTION_MPV_COMMAND;
    if (cmdFBSSz > 0) {
      item->cmd = (char*)malloc(cmdFBSSz * sizeof(char));
      strlcpy(item->cmd, cmdFBSTxt, cmdFBSSz);
      queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    }

  } else if (sdob_judgement->marks->selected < 0) {
    struct queue_head *itemTxt = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemTxt);
    itemTxt->action = E_Q_ACTION_MPV_COMMAND;
    size_t cmdSeekSz = strlen("show-text \"Seeking\" 750\n") + 1;
    if (cmdSeekSz > 0) {
      itemTxt->cmd = (char*)malloc(cmdSeekSz * sizeof(char));
      strlcpy(itemTxt->cmd, "show-text \"Seeking\" 750\n", cmdSeekSz);
      queue_put(itemTxt, pg_sdobQueue, &pg_sdobQueueLen);
    }

    // Framestep
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_ACTION_MPV_COMMAND;
    if (cmdFBSSz > 0) {
      item->cmd = (char*)malloc(cmdFBSSz * sizeof(char));
      strlcpy(item->cmd, cmdFBSTxt, cmdFBSSz);
      queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    }

  } else {
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_MOVE_SCORE_SELECTED;
    item->amt = -1;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  }
}

void pg_skydiveorbustButtonLeftPressed() {
  struct queue_head *item;

  // Scorecard Clicks
  if (sdob_judgement->marks->selected == 0) {
    // Reset SOWT
    char* retTimePos;
    double markTime = 0.00;
    if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
      markTime = atof(retTimePos);
      free(retTimePos);
      sdob_judgement->marks->arrScorecardTimes[0] = markTime;
      struct queue_head *itemSOWT = malloc(sizeof(struct queue_head));
      INIT_QUEUE_HEAD(itemSOWT);
      itemSOWT->action = E_Q_SCORECARD_SCORING_SOWT;
      itemSOWT->time = markTime;
      queue_put(itemSOWT, pg_sdobQueue, &pg_sdobQueueLen);
      pg_sdobScoringSelectionClear(&m_gui);

    }

  } else if (sdob_judgement->marks->selected < 0) {
    // Add to queue E_Q_SCORECARD_INSERT_MARK
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->mark = E_SCORES_BUST;
    item->milli = millis();
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  } else {
    // Add to queue E_Q_SCORECARD_UPDATE_MARK
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_UPDATE_MARK;
    item->selected = sdob_judgement->marks->selected;
    item->mark = E_SCORES_BUST;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  }
}

void pg_skydiveorbustButtonRightPressed() {
  struct queue_head *item;

// printf("Right Pressed: %d", sdob_judgement->marks->selected);
  // Scorecard Clicks
  if (sdob_judgement->marks->selected == 0) {
    // Reset SOWT
    char* retTimePos;
    double markTime = 0.00;

    if ((mpvSocketSinglet("time-pos", &retTimePos)) != -1) {
      markTime = atof(retTimePos);
      free(retTimePos);
      sdob_judgement->marks->arrScorecardTimes[0] = markTime;
      struct queue_head *itemSOWT = malloc(sizeof(struct queue_head));
      INIT_QUEUE_HEAD(itemSOWT);
      itemSOWT->action = E_Q_SCORECARD_SCORING_SOWT;
      itemSOWT->time = markTime;
      queue_put(itemSOWT, pg_sdobQueue, &pg_sdobQueueLen);
      pg_sdobScoringSelectionClear(&m_gui);
    }
  } else if (sdob_judgement->marks->selected < 0) {
    // Insert Mark, E_SCORES_BUST
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->mark = E_SCORES_POINT;
    item->milli = millis();
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  } else {
    // Add to queue E_Q_SCORECARD_UPDATE_MARK
    item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_UPDATE_MARK;
    item->selected = sdob_judgement->marks->selected;
    item->mark = E_SCORES_POINT;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  }
}

void pg_skydiveorbustButtonRotaryPressed() {
  if (sdob_judgement->marks->selected == 0) {
    // Reset SOWT

  } else if (sdob_judgement->marks->selected < 0) {

  } else {
    struct queue_head *itemSeek = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(itemSeek);
    itemSeek->action = E_Q_ACTION_MPV_COMMAND;

    double timePos = sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected];
    size_t seekSz = snprintf(NULL, 0, "seek %f %s\n", timePos, "absolute+exact") + 1;
    if (seekSz > 0) {
      itemSeek->cmd = (char*)malloc(seekSz * sizeof(char));
      snprintf(itemSeek->cmd, seekSz, "seek %f %s\n", timePos, "absolute+exact");
      queue_put(itemSeek, pg_sdobQueue, &pg_sdobQueueLen);
    }

    // struct queue_head *itemMove = malloc(sizeof(struct queue_head));
    // INIT_QUEUE_HEAD(itemMove);
    // itemMove->action = E_Q_SCORECARD_MOVE_SCORE_SELECTED;
    // // Clear Selection
    // // itemMove->selected = -1;
    // // queue_put(itemMove, pg_sdobQueue, &pg_sdobQueueLen);
  }
}

void pg_skydiveorbustButtonLeftHeld() {
  if (sdob_judgement->marks->selected >= 0) {
    struct queue_head *item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_DELETE_MARK;
    item->selected = sdob_judgement->marks->selected;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  } else {

  }
}

void pg_skydiveorbustButtonRightHeld() {
  if (sdob_judgement->marks->selected >= 0) {
    struct queue_head *item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->selected = sdob_judgement->marks->selected;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  } else {

  }
}

void pg_skydiveorbustButtonRotaryHeld() {
  if (sdob_judgement->marks->selected >= 0) {
    struct queue_head *item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_SCORECARD_UPDATE_MARK;
    item->mark = E_SCORES_OMISSION;
    item->selected = sdob_judgement->marks->selected;
    queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  } else {

  }
}

void pg_skydiveorbustButtonDoubleHeld() {
  touchscreenPageClose(&m_gui, E_PG_SKYDIVEORBUST);
  touchscreenPageOpen(&m_gui, E_PG_MAIN);
}

void pg_skydiveorbustButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_skydiveorbustButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_skydiveorbustButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_skydiveorbustButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_skydiveorbustButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_skydiveorbustButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_skydiveorbustButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_skydiveorbustButtonRightHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, &pg_skydiveorbustButtonRotaryHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, &pg_skydiveorbustButtonDoubleHeld);
}

void pg_skydiveorbustButtonUnsetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, NULL);
}








void pg_sdob_mpv_timepos_thread() {
  // Grab Player Time
  if (
    (!sdob_player->paused && sdob_player->duration > 0)
    || pg_sdob_pl_sliderForceUpdate == 1
  ) {
    pg_sdobUpdatePlayerSlider(&m_gui);
    pg_sdob_pl_sliderForceUpdate = 0;
  }
}

// ------------------------
// MPV Output TimePos Thread
// ------------------------
PI_THREAD (pg_sdobMpvTimeposThread)
{
  if (pg_sdobMpvTimeposThreadRunning) {
    // debug_print("%s\n", "Not Starting MPV TimePos Thread, Already Started");
    return NULL;
  }
  pg_sdobMpvTimeposThreadRunning = 1;

  if (pg_sdobMpvTimeposThreadKill) {
    // debug_print("%s\n", "Not Starting MPV TimePos Thread, Stop Flag Set");
    pg_sdobMpvTimeposThreadRunning = 0;
    return NULL;
  }

  // debug_print("%s\n", "Starting MPV TimePos Thread");


  while (!pg_sdobMpvTimeposThreadKill) {
    pg_sdob_mpv_timepos_thread();

    if (!pg_sdobMpvTimeposThreadKill) { usleep(300000); }
  }

  // printf("%s\n", "Closing TimePos Thread");
  pg_sdobMpvTimeposThreadRunning = 0;
  return NULL;
}


int pg_sdobMpvTimeposThreadStart() {
  // debug_print("%s\n", "pg_sdobMpvTimeposThreadStart()");
  if (pg_sdobMpvTimeposThreadRunning) { return 0; }

  pg_sdob_pl_sliderForceUpdate = 1;
  // debug_print("SkydiveOrBust MPV TimePos Thread Spinup: %d\n", pg_sdobMpvTimeposThreadRunning);
  pg_sdobMpvTimeposThreadKill = 0;
  return piThreadCreate(pg_sdobMpvTimeposThread);
}

void pg_sdobMpvTimeposThreadStop() {
  // debug_print("%s\n", "pg_sdobMpvTimeposThreadStop()");
  // Shutdown MPV FIFO Thread
  if (pg_sdobMpvTimeposThreadRunning) {
    pg_sdobMpvTimeposThreadKill = 1;
    int shutdown_cnt = 0;
    while (pg_sdobMpvTimeposThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    // debug_print("SkydiveOrBust MPV TimePos Thread Shutdown %d\n", shutdown_cnt);
  }
}






// ------------------------
// MPV Socket Thread
// ------------------------
PI_THREAD (pg_sdobMpvSocketThread)
{
  if (pg_sdobMpvSocketThreadRunning) {
    // debug_print("%s\n", "Not Starting MPV Event Thread, Already Started");
    return NULL;
  }
  pg_sdobMpvSocketThreadRunning = 1;

  if (pg_sdobMpvSocketThreadKill) {
    // debug_print("%s\n", "Not Starting MPV Event Thread, Stop Flag Set");
    pg_sdobMpvSocketThreadRunning = 0;
    return NULL;
  }

  // debug_print("%s\n", "Starting MPV Event Thread");

  struct sockaddr_un addr;
  // char buf[100];

  // char* mpv_chapter_ret;

  int socket_try = 0;

  int lastSize = 0;
  // struct queue_head *item;

  // Wait for socket to arrive
  while (!pg_sdobMpvSocketThreadKill && socket_try < 10 && access(mpv_socket_path, R_OK) == -1) {
    // debug_print("Waiting to See %s\n", mpv_socket_path);
    socket_try++;
    usleep(1000000);
  }

  if ((pg_sdob_player_mpv_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    // debug_print("%s\n", "MPV Socket Error");
    pg_sdobMpvSocketThreadKill = 1;
  }
  // Set Socket Non-Blocking
  setnonblock(pg_sdob_player_mpv_fd);

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  if (*mpv_socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path+1, mpv_socket_path+1, sizeof(addr.sun_path)-2);
  } else {
    strncpy(addr.sun_path, mpv_socket_path, sizeof(addr.sun_path)-1);
  }

  if (connect(pg_sdob_player_mpv_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
    // MPV Connect Error
    pg_sdobMpvSocketThreadKill = 1;
  }

  // Grab MPV Events, sent in JSON format
  while(!pg_sdobMpvSocketThreadKill) {
    if (!fd_is_valid(pg_sdob_player_mpv_fd)) {
      // printf("FD Re-Connect: %d, %d\n", pg_sdob_player_mpv_fd_timer, millis());
      // try closing fd
      if (pg_sdob_player_mpv_fd) { close(pg_sdob_player_mpv_fd); }
      // reconnect fd
      if (connect(pg_sdob_player_mpv_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        // MPV Connect Error
        pg_sdobMpvSocketThreadKill = 1;
      }
    }

    if (lastSize != sdob_judgement->marks->size) {
      // printf("Last Size Differ!\n");
// Create Queue Entry
      // Scorecard size changed
      lastSize = sdob_judgement->marks->size;

      // Update Slider Max Positions
      int sliderMax = (int)(sdob_judgement->marks->size / pg_sdob_slot_max);
      if (sliderMax != pg_sdob_scroll_max) {
        pg_sdobSliderChangeMaxPos(&m_gui, (sliderMax - pg_sdob_scroll_max), false);
      }

      // Increase line after screen is fill
      // printf("Max Score: %d, LS: %d\n", pg_sdob_slot_max * pg_sdob_line_max, lastSize);
      if (lastSize > pg_sdob_slot_max * pg_sdob_line_max && lastSize % pg_sdob_slot_max == 1) {
        pg_sdobSliderChangeCurPos(&m_gui, 1, true);
      }
      gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
      usleep(100);
    }


// Grab Next Socket Line
    char* mpv_event_ret; // = malloc(128);
    // int rc = getline(&mpv_event_ret, 256, pg_sdob_player_mpv_fd);
    int rc = sgetline(pg_sdob_player_mpv_fd, &mpv_event_ret);
    if (rc > 0) {
      //printf("Got Back: '%s'\n", mpv_event_ret);

      char* json_event; // = malloc(128);
      int rcE = ta_json_parse(mpv_event_ret, "event", &json_event);
      //printf("MPV Event: %s Len: %d\n", json_event, rcE);
      free(mpv_event_ret);
      if (rcE == 0) { continue; }


      if (strcmp(json_event, "seek") == 0
          || strcmp(json_event, "playback-restart") == 0
      ) {
        struct queue_head *itemA = malloc(sizeof(struct queue_head));
        INIT_QUEUE_HEAD(itemA);
        itemA->action = E_Q_PLAYER_VIDEO_CHECKPAUSE;
        queue_put(itemA, pg_sdobQueue, &pg_sdobQueueLen);

        struct queue_head *itemB = malloc(sizeof(struct queue_head));
        INIT_QUEUE_HEAD(itemB);
        itemB->action = E_Q_PLAYER_SLIDER_UPDATE;
        queue_put(itemB, pg_sdobQueue, &pg_sdobQueueLen);

      }
      else if (strcmp(json_event, "pause") == 0) {
        struct queue_head *item = malloc(sizeof(struct queue_head));
        INIT_QUEUE_HEAD(item);
        item->action = E_Q_PLAYER_VIDEO_SETPAUSE;
        item->mark = 1;
        queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

      }
      // else if (strcmp(json_event, "unpause") == 0) {
      else if (strcmp(json_event, "unpause") == 0) {
        struct queue_head *item = malloc(sizeof(struct queue_head));
        INIT_QUEUE_HEAD(item);
        item->action = E_Q_PLAYER_VIDEO_SETPAUSE;
        item->mark = 0;
        queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
      }
      // MPV Video Ended
      else if (strcmp(json_event, "end-file") == 0) {
        // Update GUI Display of Display Ended
        // endVideoPlayer();

      }
      // Meta Updated
      else if (strcmp(json_event, "metadata-update") == 0) {
        // printf("Meta update\n");
        struct queue_head *item = malloc(sizeof(struct queue_head));
        INIT_QUEUE_HEAD(item);
        item->action = E_Q_PLAYER_VIDEO_INFO;
        queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
      }
      // Chapter Changed
      else if (strcmp(json_event, "chapter-change") == 0) {
        struct queue_head *item = malloc(sizeof(struct queue_head));
        INIT_QUEUE_HEAD(item);
        item->action = E_Q_PLAYER_CHAPTER_CHANGED;
        queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

      }
      // File Loaded (Unpause)
      else if (strcmp(json_event, "file-loaded") == 0) {
        // sdob_player->paused = 0;
      }
      free(json_event);
    usleep(100);
    }

  // No Work needed done
    else {
      // Nothing to Do, Sleep for a moment
      usleep(200000);
    }
  }
  // close
  // debug_print("%s\n", "Closing MPV RPC");
  pg_sdobMpvSocketThreadRunning = 0;
  return NULL;
}


int pg_sdobMpvSocketThreadStart() {
  // debug_print("%s\n", "pg_sdobMpvSocketThreadStart()");
  if (pg_sdobMpvSocketThreadRunning) { return 0; }

  // debug_print("SkydiveOrBust MPV Socket Thread Spinup: %d\n", pg_sdobMpvSocketThreadRunning);
  pg_sdobMpvSocketThreadKill = 0;
  return piThreadCreate(pg_sdobMpvSocketThread);
}

void pg_sdobMpvSocketThreadStop() {
  // debug_print("%s\n", "pg_sdobMpvSocketThreadStop()");
  // Shutdown MPV Socket Thread
  if (pg_sdobMpvSocketThreadRunning) {
    pg_sdobMpvSocketThreadKill = 1;
    int shutdown_cnt = 0;
    while (pg_sdobMpvSocketThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    // debug_print("SkydiveOrBust MPV Socket Thread Shutdown %d\n", shutdown_cnt);
  }
}


/////////////////////////////////////////////
// Initialization
//

// GUI init
void pg_skydiveorbust_init(gslc_tsGui *pGui) {
  // debug_print("%s\n", "Page SkydiveOrBust Init");

  // Dynamically Allocate Page Elements
  pg_sdobElTotal = E_SDOB_EL_MAX + 150;
  pg_sdobElem = (gslc_tsElem *)malloc(pg_sdobElTotal * sizeof(gslc_tsElem));
  // pg_sdobElemRef = (gslc_tsElemRef*)calloc(pg_sdobElTotal, sizeof(gslc_tsElemRef));
  pg_sdobEl = (gslc_tsElemRef **)malloc(pg_sdobElTotal * sizeof(gslc_tsElemRef*));
  

  //////////////////////////////
  // Queue Initializer
  pg_sdobQueue = ALLOC_QUEUE_ROOT();
  pg_sdobQueueLen = 0;
  pg_sdobMpvSocketThreadKill = 0; // Stopping SDOB Thread
  pg_sdobMpvSocketThreadRunning = 0; // Running flag for SDOB Thread
  pg_sdobThreadKill = 0; // Stopping SDOB Thread
  pg_sdobThreadRunning = 0; // Running flag for SDOB Thread



  ///////////////////
  // MPV Initializer
  mpv_init(pGui);





  /////////////////////////////
  // Scorecard Box Initializer
  pg_sdob_line_max = 2; // maximum number of lines to display on scorecard, each line is points and results
  pg_sdob_slot_max = 15; // maximum number of slots per line
  pg_sdob_scroll_max = 1; // Initial number of positions on scrollcard scrollbar;
  pg_sdob_slot_scroll = 0; // position of vertical scrollbar
  pg_sdob_score_count = 0; // Number of points to display
  pg_sdob_scorecard_max = 500; // Initial array resize for storeing marks, currently no realloc functionality
//  i_slot_start = 0; // first slot to show on scorecard, all other slots created from this index
//  i_last_score = -1; // track the last main loop score count update

  sdob_judgement = PG_SDOB_INIT_JUDGEMENT();



  // GUI Init
  gslc_tsRect rScores = {0,110,440,130};
  pg_sdobGuiInit(pGui, rScores);

  // Judge Initials
  pg_sdobUpdateJudgeInitials(pGui, "");

  // Team
  pg_sdobUpdateTeam(pGui, "");

  // Round
  pg_sdobUpdateRound(pGui, "1");

  // Video Desc
  pg_sdobUpdateVideoDesc(pGui, "");

  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);



  //////////////
  // Player Info
  sdob_player = PG_SDOB_INIT_PLAYER();
  sdob_chapters = PG_SDOB_INIT_PLAYER_CHAPTERS();
  sdob_player_ticks = PG_SDOB_INIT_PLAYER_TICKS();
  sdob_files = PG_SDOB_INIT_VIDEOLIST_FILES();
  sdob_folders = PG_SDOB_INIT_VIDEOLIST_FOLDERS();

  //sdob_video = PG_SDOB_INIT_VIDEO();

  // Flag for changing playback speed by rotary knob
  pg_sdob_change_video_rate = 0;

  // Check for playback slow default
  if (!pg_sdob_player_playback_slow_default) {
    pg_sdob_player_playback_slow_default = .75;
  }
  // Check for playback slow set
  if (!pg_sdob_player_playback_slow) {
    pg_sdob_player_playback_slow = pg_sdob_player_playback_slow_default;
  }

  pg_sdob_player_move_debounce = 0;
  pg_sdob_player_move = -1;
  pg_sdob_player_move_timepos = 0;
  pg_sdob_player_move_timepos_lock = 0;
  pg_sdob_player_mpv_fd_timer = 0;
  pg_sdobUpdateVideoRate(pGui, mpv_video_rate);
  pg_sdobUpdateUserDefinedVideoRate(pGui, pg_sdob_player_playback_slow);


  // Load File (Temp)
  char* tmpMeet = "MEET2020";
  char* tmpFile = "Group1-12_2.mp4";
  mpv_loadfile("skydiveorbust", tmpMeet, tmpFile, "replace", "fullscreen=yes");
  pg_sdobUpdateMeet(pGui, tmpMeet);
  pg_sdobUpdateVideoDesc(pGui, tmpFile);

  pg_sdob_pl_sliderForceUpdate = 1;

  //////////////////////////////
  // Finish up
  // Clear function pointer, indicate it's been run
  cbInit[E_PG_SKYDIVEORBUST] = NULL;
}


void pg_sdob_scorecard_insert_mark(gslc_tsGui *pGui, int selected, double time, int mark) {
  pg_sdobInsertMark(selected, time, mark);
  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

  // Update Scorecard Ticks
  pg_sdobScoringMarks(pGui);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);

  pg_sdobUpdatePlayerSlider(pGui);
}

void pg_sdob_scorecard_update_mark(gslc_tsGui *pGui, int selected, int mark) {
  sdob_judgement->marks->arrScorecardPoints[selected] = mark;
  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

  // Update Scorecard Ticks
  pg_sdobScoringMarks(pGui);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
}


void pg_sdob_scorecard_delete_mark(gslc_tsGui *pGui, int selected) {
  pg_sdobDeleteMark(selected);
  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

  // Update Scorecard Ticks
  pg_sdobScoringMarks(pGui);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
}


void pg_sdob_scorecard_score_selected(gslc_tsGui *pGui, int selected, double amt) {
  // Updated Selected Score
  pg_sdobMoveMark(selected, amt);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

  // Make sure the mark is viewable
  int markHidden = pg_sdobScoringMarkHidden(sdob_judgement->marks->selected);
  if (markHidden < 0) {
    pg_sdobSliderSetCurPos(pGui, (int)(sdob_judgement->marks->selected % pg_sdob_line_max));
  } else if (markHidden > 0) {
    pg_sdobSliderSetCurPos(pGui, (int)(sdob_judgement->marks->selected % pg_sdob_line_max) + 1);
  }

  // Update Scorecard Ticks
  pg_sdobScoringMarks(pGui);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
}

void pg_sdob_scorecard_score_sowt(gslc_tsGui *pGui, double time) {
  // Reset Ticks to Judge Marks
  // CLEAR_PLAYER_TICKS
  if (sdob_judgement->sowt == -1.0) {
    pg_sdob_player_sliderTicks(pGui, NULL, 0);
    gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
  }

  // Set Start of Working Time
  pg_sdobSOWTSet(time);

  pg_sdob_pl_sliderForceUpdate = 1;
}

void pg_sdob_scorecard_clear(gslc_tsGui *pGui) {
  PG_SDOB_CLEAR_JUDGEMENT(sdob_judgement);

  // // UNREM TO CLEAN TEAM AND ROUND
  // pg_sdobUpdateTeam(pGui, "");
  // pg_sdobUpdateRound(pGui, "");

  // Reset Scorecard Scroller
  pg_sdobSliderResetCurPos(pGui);

  // Reset SOWT and Video Length
  pg_sdobSOWTReset();

  if (sdob_player->paused) { pg_sdobUpdatePlayerSlider(pGui); }

  // Return to Chapter Marks
  pg_sdob_player_video_chapterMarks(pGui);

  // CLEAR_PLAYER_TICKS
  // pg_sdob_player_sliderTicks(pGui, NULL, 0);

  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
}


void pg_sdob_player_chaptersRefresh(gslc_tsGui *pGui) {
  pg_sdob_player_setduration();
  pg_sdob_player_video_chapters();
  pg_sdob_player_video_chapterMarks(pGui);

  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
}


void pg_sdob_player_sliderForceUpdate() {
  pg_sdob_pl_sliderForceUpdate = 1;
}


// GUI Thread
int pg_skydiveorbust_thread(gslc_tsGui *pGui) {
  if (m_bQuit) { return 0; } // Quit flag set, do no more work


  ////////////////////////
  if (pg_sdobQueueLen > 0) {
    // Fetch Items to deal with in queue
    struct queue_head *item = queue_get(pg_sdobQueue, &pg_sdobQueueLen);
    if (item) {
      switch (item->action) {
        // Buttons, Knob, Keyboard actions
        case E_Q_SCORECARD_KEY:
          // if (item->key != NULL) { keyboard_proxy(item->key); }
        break;

        // Insert Mark
        case E_Q_SCORECARD_INSERT_MARK:
          pg_sdob_scorecard_insert_mark(pGui, item->selected, item->time, item->mark);
        break;


        // Update Mark
        case E_Q_SCORECARD_UPDATE_MARK:
          pg_sdob_scorecard_update_mark(pGui, item->selected, item->mark);
        break;

        // Delete Mark
        case E_Q_SCORECARD_DELETE_MARK:
          pg_sdob_scorecard_delete_mark(pGui, item->selected);
        break;

        case E_Q_SCORECARD_MOVE_SCORE_SELECTED:
          pg_sdob_scorecard_score_selected(pGui, item->selected, item->amt);
        break;

        case E_Q_SCORECARD_SCORING_SOWT:
          pg_sdob_scorecard_score_sowt(pGui, item->time);
        break;

        case E_Q_SCORECARD_SUBMIT_SCORECARD:
          // Submit scorecard to syslog
          pg_sdobSubmitScorecard();
        printf("SUIBMIT SCORES!!\n");
        // No break, clear scorecard after submit
        case E_Q_SCORECARD_CLEAR:
        printf("CLEAR SCORES!!\n");
          pg_sdob_scorecard_clear(pGui);
        break;

        case E_Q_PLAYER_SLIDER_CHAPTERS:
          pg_sdob_player_chaptersRefresh(pGui);
        break;

        case E_Q_PLAYER_CHAPTER_CHANGED:
          pg_sdob_player_setChapterCur();
        break;

        case E_Q_PLAYER_VIDEO_INFO:
          pg_sdob_player_videoInfo(pGui);
        break;

        // Writes commands to open FD Socket
        case E_Q_ACTION_MPV_COMMAND:
          mpv_cmd(item->cmd);
        break;


        case E_Q_ACTION_PLAY_PAUSE:
          // toggle_play_pause();
        break;


        case E_Q_ACTION_VIDEO_RATE:
          pg_sdobUpdateVideoRate(pGui, mpv_speed(item->amt));
        break;

        case E_Q_ACTION_VIDEO_RATE_ADJUST:
          pg_sdobUpdateVideoRate(pGui, mpv_speed_adjust(item->amt));
        break;

        case E_Q_ACTION_VIDEO_RATE_USER:
          pg_sdobUpdateUserDefinedVideoRate(pGui, mpv_video_rate);
        break;

        case E_Q_PLAYER_VIDEO_PAUSE:
          pg_sdob_player_pause(1);
        break;
        case E_Q_PLAYER_VIDEO_UNPAUSE:
          pg_sdob_player_pause(0);
        break;
        case E_Q_PLAYER_VIDEO_SETPAUSE:
          pg_sdob_player_setpause(item->mark);
        break;
        case E_Q_PLAYER_VIDEO_CHECKPAUSE:
          pg_sdob_player_checkpause();
        break;

        case E_Q_PLAYER_SLIDER_UPDATE:
          pg_sdob_player_sliderForceUpdate();
        break;

      }

      free(item);
      return 1;
    }
  }

    /*
  } else if (m_video_percent_new >= 0 || m_video_percent_tmp >= 0) {
    // Set Slider Position, Based on debounce of pg_sdob_slider_debounce_delay
    bSliderDelay = (millis() - nSliderPosLastFired) > pg_sdob_slider_debounce_delay;
    if (bSliderDelay) {
      nSliderPosLastFired = millis();
      if (m_video_percent_new >= 0) {
        // debug_print("Setting Pos Percent New %f\n", m_video_percent_new);
        mpv_seek_arg(m_video_percent_new, "absolute-percent+exact");
        m_video_percent_tmp = -1;
        m_video_percent_new = -2;
      } else {
        // debug_print("Setting Pos Percent Tmp %f\n", m_video_percent_tmp);
        mpv_seek_arg(m_video_percent_tmp, "absolute-percent");
        m_video_percent_tmp = -1;
      }
    }
  */




  ////////////////////////
  // Player Slider Moved
  if (pg_sdob_player_move > -1
    && (millis() - pg_sdob_player_move_debounce) > 750
  ) {
    // Execute Slider Move
    if (sdob_judgement->sowt == -1.0) {
      mpv_seek_arg(pg_sdob_player_move, "absolute-percent+exact");
    } else {
      mpv_seek_arg((sdob_judgement->sowt + (sdob_judgement->workingTime * (pg_sdob_player_move * .01))), "absolute+exact");
    }
    pg_sdob_player_move_debounce = millis();
    pg_sdob_player_move = -1;
    return 1;
  /*} else
  if (sdob_player->paused == 0
      && (millis() - pg_sdob_player_move_timepos) > 500
  ) {
    pg_sdob_player_move_timepos = millis();
    // printf("Slider Update\n");
    pg_sdobUpdatePlayerSlider(pGui);
  */
  }

  return 0;
}





// ------------------------
// MPV Output TimePos Thread
// ------------------------
PI_THREAD (pg_sdobThread)
{
  if (pg_sdobThreadRunning) {
    // debug_print("%s\n", "Not Starting MPV TimePos Thread, Already Started");
    return NULL;
  }
  pg_sdobThreadRunning = 1;

  if (pg_sdobThreadKill) {
    // debug_print("%s\n", "Not Starting MPV TimePos Thread, Stop Flag Set");
    pg_sdobThreadRunning = 0;
    return NULL;
  }

  // debug_print("%s\n", "Starting MPV TimePos Thread");


  while (!pg_sdobThreadKill) {
    if (!pg_skydiveorbust_thread(&m_gui)) {
      usleep(250000);
    }
  }

  // debug_print("%s\n", "Closing MPV Fifo");
  pg_sdobThreadRunning = 0;
  return NULL;
}


int pg_sdobThreadStart() {
  // debug_print("%s\n", "pg_sdobThreadStart()");
  if (pg_sdobThreadRunning) { return 0; }

  pg_sdob_pl_sliderForceUpdate = 1;
  // printf("SkydiveOrBust MPV TimePos Thread Spinup: %d\n", pg_sdobThreadRunning);
  pg_sdobThreadKill = 0;
  return piThreadCreate(pg_sdobThread);
}

void pg_sdobThreadStop() {
  // debug_print("%s\n", "pg_sdobThreadStop()");
  // Shutdown MPV FIFO Thread
  if (pg_sdobThreadRunning) {
    pg_sdobThreadKill = 1;
    int shutdown_cnt = 0;
    while (pg_sdobThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    // printf("SkydiveOrBust MPV TimePos Thread Shutdown %d\n", shutdown_cnt);
  }
}



// GUI Open
void pg_skydiveorbust_open(gslc_tsGui *pGui) {

  // debug_print("%s\n", "Page SkydiveOrBust Setting Button Functions");
  pg_skydiveorbustButtonSetFuncs();
  ////////////////////////////
  // Start SDOB Thread
  // // debug_print("%s\n", "Page SkydiveOrBust Stopping MPV TimePos Thread");
  pg_sdobMpvTimeposThreadStart();
  // // debug_print("%s\n", "Page SkydiveOrBust Stopping MPV Socket Thread");
  pg_sdobMpvSocketThreadStart();
  // // debug_print("%s\n", "Page SkydiveOrBust Starting Thread");
  //-/ pg_sdobThreadStart();

  // Reset Scorecard Slider to Top
  // pg_sdobSliderResetCurPos(pGui);

  // debug_print("%s\n", "Page SkydiveOrBust Started");
}


// GUI Close
void pg_skydiveorbust_close(gslc_tsGui *pGui) {
  ////////////////////////////
  // Stop SDOB Thread
  // // debug_print("%s\n", "Page SkydiveOrBust Stopping Thread");
  //-/ pg_sdobThreadStop();
  // // debug_print("%s\n", "Page SkydiveOrBust Stopping MPV Socket Thread");
  pg_sdobMpvSocketThreadStop();
  // // debug_print("%s\n", "Page SkydiveOrBust Stopping MPV TimePos Thread");
  pg_sdobMpvTimeposThreadStop();
}

// GUI Destroy
void pg_skydiveorbust_destroy(gslc_tsGui *pGui) {
  size_t cmdStopSz = snprintf(NULL, 0, "%s\n", "stop") + 1;
  char *cmdStop = (char*)malloc(cmdStopSz * sizeof(char));
  snprintf(cmdStop, cmdStopSz, "%s\n", "stop");
  mpv_cmd(cmdStop);

  // Free Judgement Info
  free(sdob_judgement->judge);
  free(sdob_judgement->video_file);
  free(sdob_judgement->meet);
  free(sdob_judgement->team);
  free(sdob_judgement->teamStr);
  free(sdob_judgement->round);
  free(sdob_judgement->roundStr);
  free(sdob_judgement->scoreStr);

  free(sdob_judgement->marks->arrScorecardId);
  free(sdob_judgement->marks->arrScorecardPoints);
  free(sdob_judgement->marks->arrScorecardMilli);
  free(sdob_judgement->marks->arrScorecardTimes);
  free(sdob_judgement->marks->arrScorecardTicks);

  free(sdob_judgement->marks);
  free(sdob_judgement);

  // Free Player
  free(sdob_player->pbrateStr);
  free(sdob_player->pbrateUserStr);
  free(sdob_player->positionStr);
  free(sdob_player);

  // Free Chapters
  free(sdob_chapters->ptr);
  free(sdob_chapters);

  // Free Folders List
  int i;
  for (i = 0; i < sdob_folders->max; ++i) {
    free(sdob_folders->list[i]);
  }
  free(sdob_folders->list);
  free(sdob_folders);

  // Free Files List
  for (i = 0; i < sdob_files->max; ++i) {
    free(sdob_files->list[i]);
  }
  free(sdob_files->list);
  free(sdob_files);

  // Free Ticks
  free(sdob_player_ticks->ptr);
  free(sdob_player_ticks);

  // printf("Page SkydiveOrBust Destroyed\n");
}


void __attribute__ ((constructor)) pg_skydiveorbust_constructor(void) {
  cbInit[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_init;
  cbOpen[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_open;
  cbThread[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_thread;
  cbClose[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_close;
  cbDestroy[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_destroy;
}

void __attribute__ ((destructor)) pg_skydiveorbust_destructor(void) {

}

