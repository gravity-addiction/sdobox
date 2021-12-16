#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <syslog.h>
#include <math.h>
#include <time.h> // system time clocks
#include <unistd.h>
#include <fcntl.h>
#include <wiringPi.h> // Gordons Wiring Pi
#include <regex.h>
#include <assert.h>
#include <jansson.h>
#include <libconfig.h>
#include <errno.h>

#include "./skydiveorbust.h"
#include "./skydiveorbust_zmq.h"

#include "gui/pages.h"

#include "libs/buttons/buttons.h"
#include "libs/queue/queue.h"
#include "libs/mpv-zmq/mpv-zmq.h"
#include "libs/fbcp/fbcp.h"
#include "libs/dbg/dbg.h"
#include "libs/zhelpers/zhelpers-conn.h"
#include "libs/zhelpers/zhelpers-tx.h"
#include "libs/zhelpers/zhelpers.h"
#include "libs/mpv-zmq/mpv-zmq.h"
#include "libs/mpv-zmq/mpv-zmq-helper.h"
// #include "libs/json/json.h"
// #include "libs/curl-sdob/curl-sdob.h"
// #include "libs/ulfius/websocket_api.h"

#include "gui/keyboard/keyboard.h"



void PG_SDOB_SCORECARD_CLEAR_MARKS(struct pg_sdob_scorecard_marks *sc)
{
  sc->size = 0;
  sc->selected = -1;
  sc->last = -1;
  for (size_t s = 0; s < sc->max; s++) {
    sc->arrScorecardPoints[s] = -1;
    sc->arrScorecardTimes[s] = 0;
    sc->arrScorecardTicks[s] = 0;
  }
}

// Initialize Host Device Info
struct pg_sdob_device_host * PG_SDOB_DEVICE_HOST()
{
  struct pg_sdob_device_host *sdh = (struct pg_sdob_device_host*)malloc(sizeof(struct pg_sdob_device_host));
  sdh->cnt = 1; // 1 for initalization in thread
  sdh->seenCnt = 0;
  sdh->isHost = 0; // 1 is host device, 0 is not
  return sdh;
}


// Initialize scorecard marks
struct pg_sdob_scorecard_marks * PG_SDOB_SCORECARD_INIT_MARKS()
{
  struct pg_sdob_scorecard_marks *sc = (struct pg_sdob_scorecard_marks*)malloc(sizeof(struct pg_sdob_scorecard_marks));

  // sc->video_hash = ""; // video filename in meet folder
  // sc->sowt = -1.0;

  sc->max = pg_sdob_scorecard_max;
  sc->tickCnt = 0;

  sc->arrScorecardPoints = (int*)malloc(sc->max * sizeof(int));
  sc->arrScorecardTimes = (double*)malloc(sc->max * sizeof(double));
  sc->arrScorecardTicks = (uint16_t*)malloc(sc->max * sizeof(uint16_t));

  PG_SDOB_SCORECARD_CLEAR_MARKS(sc);

  return sc;
}

// Clear Scorecard Marks,
// Executed thru PG_SDOB_CLEAR_JUDGEMENT

// Initialize Video Data
struct pg_sdob_video_data * PG_SDOB_INIT_VIDEO_DATA() {
  struct pg_sdob_video_data *video_data = (struct pg_sdob_video_data*)malloc(sizeof(struct pg_sdob_video_data));
  video_data->video_type = 0;
  video_data->video_file = (char*)malloc(256 * sizeof(char));
  video_data->local_folder = (char*)malloc(256 * sizeof(char));
  video_data->url = (char*)malloc(512 * sizeof(char));
  video_data->video_file[0] = '\0';
  video_data->local_folder[0] = '\0';
  video_data->url[0] = '\0';
  return video_data;
}

// Clear Video Data
void PG_SDOB_CLEAR_VIDEO_DATA(struct pg_sdob_video_data *video_data)
{
  video_data->video_type = 0;
  CLEAR(video_data->video_file, 256);
  CLEAR(video_data->local_folder, 256);
  CLEAR(video_data->url, 512);
}
// Clear Video Data
void PG_SDOB_FREE_VIDEO_DATA(struct pg_sdob_video_data *vd)
{
  free(vd->video_file);
  free(vd->local_folder);
  free(vd->url);
}


// Initialize Judgement Data and Scorecard Marks
struct pg_sdob_judgement_data * PG_SDOB_INIT_JUDGEMENT() {
  struct pg_sdob_judgement_data *judgement = (struct pg_sdob_judgement_data*)malloc(sizeof(struct pg_sdob_judgement_data));
  judgement->video = PG_SDOB_INIT_VIDEO_DATA();
  judgement->marks = PG_SDOB_SCORECARD_INIT_MARKS();

  judgement->ruleSet = (char*)malloc(64 * sizeof(char));
  judgement->ruleSet[0] = '\0';
  judgement->judge = (char*)malloc(64 * sizeof(char));
  judgement->judge[0] = '\0';
  judgement->event = (char*)malloc(128 * sizeof(char));
  judgement->event[0] = '\0';
  judgement->eventStr = (char*)malloc(128 * sizeof(char));
  judgement->eventStr[0] = '\0';
  judgement->comp = (char*)malloc(128 * sizeof(char));
  judgement->comp[0] = '\0';
  judgement->compStr = (char*)malloc(128 * sizeof(char));
  judgement->compStr[0] = '\0';
  judgement->team = (char*)malloc(128 * sizeof(char));
  judgement->team[0] = '\0';
  judgement->teamStr = (char*)malloc(128 * sizeof(char));
  judgement->teamStr[0] = '\0';
  judgement->round = (char*)malloc(64 * sizeof(char));
  judgement->round[0] = '\0';
  judgement->roundStr = (char*)malloc(64 * sizeof(char));
  judgement->roundStr[0] = '\0';

  judgement->sopst = -1.0;
  judgement->prestartTime = 0.0;
  judgement->sowt = -1.0;
  judgement->workingTime = 0.0;
  judgement->postFreezeFrameTime = 0.0;
  judgement->tossStartCount = 0;

  judgement->score = 0.00;
  judgement->scoreMax = 0.00;
  judgement->scoreStr = (char*)malloc(32 * sizeof(char));
  judgement->scoreStr[0] = '\0';
  return judgement;
}

// Clear Judgement Data and Scorecard Marks
void PG_SDOB_CLEAR_JUDGEMENT(struct pg_sdob_judgement_data *judgement)
{
  judgement->sopst = -1.0;
  judgement->sowt = -1.0;
  judgement->score = 0.00;
  judgement->scoreMax = 0.00;
  CLEAR(judgement->scoreStr, 32);

  PG_SDOB_SCORECARD_CLEAR_MARKS(judgement->marks);
}

void PG_SDOB_CLEAR_JUDGEMENT_META(struct pg_sdob_judgement_data *judgement)
{
  // judgement->prestartTime = 0.0;
  // judgement->workingTime = 0.0;
  // CLEAR(judgement->ruleSet, 64);
  // CLEAR(judgement->judge, 64);
  CLEAR(judgement->event, 128);
  CLEAR(judgement->eventStr, 128);
  CLEAR(judgement->comp, 128);
  CLEAR(judgement->compStr, 128);
  CLEAR(judgement->team, 128);
  CLEAR(judgement->teamStr, 128);
  CLEAR(judgement->round, 64);
  CLEAR(judgement->roundStr,64);

  // PG_SDOB_CLEAR_VIDEO_DATA(judgement->video);
}






////////////////////////////////////////////////////////////////

// All code related to video rounds processing - the parsing of the
// video filename into a list of candidate event/team/rounds.

// Possible matching filenames:
//  USIS2020_(8_1_801)_(8_1_802).mpg
//  USIS2020_(4_10_401)_(4_9_GK4)_(4J_8_317)_(4J_8_317).30fps.4k.mp4

// No set limit on how many rounds are present.
// The first field of a round is an event descriptor used to trigger
// a change in default working time (to 50.0 if it starts with '8').

// The full pattern is mainly just used to decide initially if the
// filename obeys the desired pattern, and to give the first subpattern
// to strip the "mee name" descriptor.
// Anything after the first period (dot) is ignored.
#define SVR_WHOLENAME "^\\([^_]\\+\\)\\(_([^_]\\+_[^_]\\+_[^_]\\+)\\)\\+\\."

// Used to extract a event,team,round from the string, one by one
#define SVR_PARTIAL   "^_(\\([^_]\\+\\)_\\([^_]\\+\\)_\\([^_]\\+\\))"

// Extra event,team,round from omniskore saved videos
#define SVR_OMNISKORE "^\\([^_]\\+\\)_\\([^_]\\+\\)_\\([^_]\\+\\)_\\([^_]\\+\\)\\."

#define SVR_TAMMY "^\\([^_]\\+\\)_\\([^_]\\+\\)_\\(R[^_]\\+\\)_\\([0-9]\\+\\)\\."

static regex_t r_svr_tammy; // r_svr_wholefilename, r_svr_partial, r_svr_omniskore, 
static double default_working_time = 35.0;
// static char svr_parsed_meet[64];

static void regexcompfail(char* msg, char* expression, int status, regex_t* rptr) {
  char buffer[256];
  size_t len = sizeof(buffer);
  regerror(status, rptr, buffer, len);
  dbgprintf(DBG_ERROR, "Failed to compile %s %s, %s\n", msg, expression, buffer);
  abort();
}

static void init_svr_regexp() {
  /*int compstate = regcomp(&r_svr_wholefilename, SVR_WHOLENAME, 0);
  if (compstate)
    regexcompfail("wholename", SVR_WHOLENAME, compstate, &r_svr_wholefilename);

  compstate = regcomp(&r_svr_partial, SVR_PARTIAL, 0);
  if (compstate)
    regexcompfail("partial", SVR_PARTIAL, compstate, &r_svr_partial);

  compstate = regcomp(&r_svr_omniskore, SVR_OMNISKORE, 0);
  if (compstate)
    regexcompfail("omniskore", SVR_OMNISKORE, compstate, &r_svr_omniskore);
  */
  int compstate = regcomp(&r_svr_tammy, SVR_TAMMY, 0);
  if (compstate)
    regexcompfail("tammy", SVR_TAMMY, compstate, &r_svr_tammy);
}

static void destroy_svr_regexp() {
  /*
  regfree(&r_svr_wholefilename);
  regfree(&r_svr_partial);
  regfree(&r_svr_omniskore);
  */
  regfree(&r_svr_tammy);
}

static void destroy_sdob_video_rounds() {
  for(size_t i=0;i<sdob_num_current_rounds;++i) {
    free(sdob_current_rounds[i].eventname);
    free(sdob_current_rounds[i].teamnumber);
    free(sdob_current_rounds[i].round);
  }
  free(sdob_current_rounds);
  sdob_current_rounds = NULL;
  sdob_num_current_rounds = 0;
}
/*
static void parse_video_rounds(char* vfilename) {

  regmatch_t matches[4];

  // First nuke any prior state:
  destroy_sdob_video_rounds();

  int m = regexec(&r_svr_wholefilename, vfilename, 2, matches, 0);
  if (m) {
    // not a match, restore default working time
    dbgprintf(DBG_SVR|DBG_INFO, "video filename '%s' does not match svr pattern\n", vfilename);
    default_working_time = 35.0;
    return;
  }

  // Store the meet name
  memset(svr_parsed_meet,0,sizeof(svr_parsed_meet));
  strncpy(svr_parsed_meet, vfilename, MIN(63,matches[1].rm_eo));
  dbgprintf(DBG_INFO|DBG_SVR, "SVR: meet = %s\n", svr_parsed_meet);

  char* subline = vfilename + matches[1].rm_eo;
  int sublinelength = strlen(vfilename) - matches[1].rm_eo;
  // first pass count up:
  while(sublinelength > 0) {
    m = regexec(&r_svr_partial, subline, 1, matches, 0);
    if (m)
      break;
    ++sdob_num_current_rounds;
    subline += matches[0].rm_eo;
    sublinelength -= matches[0].rm_eo;
  }

  dbgprintf(DBG_INFO|DBG_SVR, "SVR: sdob_num_current_rounds=%u\n", sdob_num_current_rounds);

  sdob_current_rounds = malloc(sdob_num_current_rounds
                               * sizeof(struct pg_sdob_video_round_record));
  subline = vfilename + matches[1].rm_eo;
  for(size_t i=0;i<sdob_num_current_rounds;++i) {
    m = regexec(&r_svr_partial, subline, 4, matches, 0);
    assert(m == 0);
    sdob_current_rounds[i].eventname =
      strndup(subline+matches[1].rm_so,
              matches[1].rm_eo - matches[1].rm_so);
    sdob_current_rounds[i].round =
      strndup(subline+matches[2].rm_so,
              matches[2].rm_eo - matches[2].rm_so);
    sdob_current_rounds[i].teamnumber =
      strndup(subline+matches[3].rm_so,
              matches[3].rm_eo - matches[3].rm_so);

    dbgprintf(DBG_SVR|DBG_INFO, "SVR: [%d] event=%s, team=%s, round=%s\n",
              i, sdob_current_rounds[i].eventname,
              sdob_current_rounds[i].teamnumber,
              sdob_current_rounds[i].round);

    subline += matches[0].rm_eo;
  }

  // The pattern only permits patterns with at least ONE of these records
  assert(sdob_num_current_rounds > 0);

  dbgprintf(DBG_SVR|DBG_INFO, "SVR: working time set to %.1f\n", default_working_time);
}
*/
////////////////////////////////////////////////////////////////

// Initialize Player Video Chapters
struct pg_sdob_player_chapters * PG_SDOB_INIT_PLAYER_CHAPTERS() {
  struct pg_sdob_player_chapters *chapters = (struct pg_sdob_player_chapters*)malloc(sizeof(struct pg_sdob_player_chapters));
  chapters->len = 0;
  chapters->max = 64;
  chapters->cur = -1;
  chapters->ptr = (uint16_t*)malloc(chapters->max * sizeof(uint16_t));
  return chapters;
}

// Initialize Player Ticks
struct pg_sdob_player_ticks * PG_SDOB_INIT_PLAYER_TICKS() {
  struct pg_sdob_player_ticks *ticks = (struct pg_sdob_player_ticks*)malloc(sizeof(struct pg_sdob_player_ticks));
  ticks->len = 0;
  ticks->max = 64;
  pthread_mutex_init(&ticks->lock, NULL);
  ticks->ptr = (uint16_t*)malloc(ticks->max * sizeof(uint16_t));
  return ticks;
}


// Initialize Videolist Files
struct pg_sdob_videolist_files * PG_SDOB_INIT_VIDEOLIST_FILES() {
  struct pg_sdob_videolist_files *files = (struct pg_sdob_videolist_files*)malloc(sizeof(struct pg_sdob_videolist_files));
  files->max = 4096;
  files->len = 256;
  files->size = 0;
  files->cur = -1;
  files->list = (char**)malloc(files->max * sizeof(char*));
  for (int i = 0; i < files->max; ++i) {
    files->list[i] = (char*)malloc(files->len * sizeof(char));
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
  folders->list = (char**)malloc(folders->max * sizeof(char*));
  for (int i = 0; i < folders->max; ++i) {
    folders->list[i] = (char*)malloc(folders->len * sizeof(char));
  }
  return folders;
}




/////////////////////////////////////////////
// Page Update Callbacks
//

void pg_sdobSliderSetCurPos(gslc_tsGui *pGui, int ss) {
  if (ss < 0) { ss = pg_sdob_scroll_max; }
  else if (ss > pg_sdob_scroll_max) { ss = 0; }
  pg_sdob_slot_scroll = ss;
}

// Updated Scorecard Slider Position
void pg_sdobSliderChangeCurPos(gslc_tsGui *pGui, int amt) {
  // Add amt
  int ss = pg_sdob_slot_scroll + amt;

  // Set Slider Pos
  pg_sdobSliderSetCurPos(pGui, ss);
}

void pg_sdobSliderResetCurPos(gslc_tsGui *pGui) {
  pg_sdobSliderSetCurPos(pGui, 0);
}






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

// Update Scorecard Count
void pg_sdobUpdateCount(gslc_tsGui *pGui, gslc_tsElemRef *pElem) {
  const int omission_cost = -3;
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
      case E_SCORES_SOPST:
        totalScore--;
      break;
      case E_SCORES_SOWT:
        // Count this as a point when prestart time is active
        if (sdob_judgement->prestartTime == 0.0) {
          totalScore--;
        }
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
  // libmpvCache->player->pbrate = playback_rate;
  int pbPercent = (int)round(playback_rate * 100);
  size_t pbrateSz = snprintf(NULL, 0, "%d%%", pbPercent) + 1;
  if (pbrateSz > 0 && pbrateSz <= 10) {
    snprintf(libmpvCache->player->pbrateStr, pbrateSz, "%d%%", pbPercent);
    gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_PL_RATE], libmpvCache->player->pbrateStr);
  }
}

void pg_sdobUpdateUserDefinedVideoRate(gslc_tsGui *pGui, double playback_rate) {
  if (playback_rate < 0) { return; }
  pg_sdob_player_playback_slow = playback_rate;

  libmpvCache->player->pbrateUser = playback_rate;
  int pbPercent = (int)round(playback_rate * 100);
  size_t pbPercentSz = snprintf(NULL, 0, "%d%%", pbPercent) + 1;
  if (pbPercentSz > 0 && pbPercentSz <= 10) {
    snprintf(libmpvCache->player->pbrateUserStr, pbPercentSz, "%d%%", pbPercent);
    gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], libmpvCache->player->pbrateUserStr);
  }
}

// Setup Device GUI As Host or Slave
void pg_sdobUpdateHostDeviceInfo(gslc_tsGui *pGui) {
  if (sdob_devicehost->isHost == 0) {
    // gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_BTN_MIRROR], false);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_PLAY], false);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_STOP], false);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_FORWARD], false);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_BACK], false);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], false);
    // gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], false);
    // gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], false);
    // gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_RATE], false);
  } else {
    // gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_BTN_MIRROR], false);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_PLAY], true);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_STOP], true);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_FORWARD], true);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_BACK], true);
    gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_USER_RATE], true);
    // gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], true);
    // gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], true);
    // gslc_ElemSetVisible(pGui, pg_sdobEl[E_SDOB_EL_PL_RATE], true);
  }
}


void pg_sdobUpdateJudgeInitials(gslc_tsGui *pGui, char* str) {
  strlcpy(sdob_judgement->judge, str, 64);
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_JUDGE_INITIALS], sdob_judgement->judge);
}

void sdob_selectEventComp(gslc_tsGui* pGui, char* compId, char* compStr) {
  size_t dispSz = snprintf(NULL, 0, "%s", compStr) + 1;
  if (dispSz > 0 && dispSz <= 128) {
    strlcpy(sdob_judgement->comp, compId, 128);
    snprintf(sdob_judgement->compStr, dispSz, "%s", compStr);
    gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SBOD_EL_DESC_ONE], sdob_judgement->compStr);
  }
}

void pg_sdobUpdateEventFromLocalFolder(gslc_tsGui *pGui, char* str) {
  char *lastslash = strrchr(str, '/');
  if(!lastslash || lastslash == str) {
    strlcpy(sdob_judgement->eventStr, str, 128);
  } else {
    strlcpy(sdob_judgement->eventStr, lastslash + 1, 128);
  }
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SBOD_EL_DESC_ONE], sdob_judgement->eventStr);
}




void sdob_selectEventTeamRound(gslc_tsGui* pGui, unsigned roundIndex) {

  // pg_sdobUpdateEventFromLocalFolder(pGui, svr_parsed_meet);
  // pg_sdobUpdateComp(&m_gui, sdob_judgement->comp, sdob_judgement->compStr);
  pg_sdobUpdateTeam(pGui, sdob_current_rounds[roundIndex].teamnumber);
  pg_sdobUpdateRound(pGui, sdob_current_rounds[roundIndex].round);

  // Extreme hack -- but it's the only way to configure this
  if (sdob_current_rounds[roundIndex].eventname[0] == '8')
    default_working_time = 50.0;
  else
    default_working_time = 35.0;
}


void pg_sdobUpdateVideoDesc(gslc_tsGui *pGui, char* str) {
/*  strlcpy(sdob_judgement->video->video_file, str, 256);
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SBOD_EL_DESC_TWO], sdob_judgement->video->video_file);

  parse_video_rounds(str);

  if (sdob_current_rounds && sdob_num_current_rounds > 0) {
    sdob_selectEventTeamRound(pGui, 0);
  }
*/
}

void pg_sdobUpdateVideoDescOne(gslc_tsGui *pGui, char* str) {
  strlcpy(sdob_judgement->eventStr, str, 128);
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SBOD_EL_DESC_ONE], sdob_judgement->eventStr);
}

void pg_sdobUpdateVideoDescTwo(gslc_tsGui *pGui, char* str) {
  strlcpy(sdob_judgement->compStr, str, 128);
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SBOD_EL_DESC_TWO], sdob_judgement->compStr);
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

void pg_sdobUpdateEvent(gslc_tsGui *pGui, char* eventStr) {

  // printf("Event Folder: %s\n", eventStr);
  if (strcmp(eventStr, "2016-USPA-CF") == 0) {
    strlcpy(sdob_judgement->eventStr, "2016 USPA Nationals", 128);
  } else if (strcmp(eventStr, "2017-USPA-CF") == 0) {
    strlcpy(sdob_judgement->eventStr, "2017 USPA Canopy Nationals", 128);
  } else if (strcmp(eventStr, "2018-USPA-CF") == 0) {
    strlcpy(sdob_judgement->eventStr, "2018 USPA Canopy Nationals", 128);
  } else if (strcmp(eventStr, "2019-USPA-CF") == 0) {
    strlcpy(sdob_judgement->eventStr, "2019 USPA Canopy Nationals", 128);
  } else if (strcmp(sdob_judgement->eventStr, "20CFGHOST") == 0) {
    strlcpy(sdob_judgement->eventStr, "2020 CF Ghost Nationals", 128);
  } else if (eventStr[0] != '\0') {
    strlcpy(sdob_judgement->eventStr, eventStr, 128);
  } else {
    strlcpy(sdob_judgement->eventStr, "Unknown", 128);
  }
  // pg_sdobUpdateVideoDescOne(pGui, sdob_judgement->eventStr);

}

void pg_sdobUpdateComp(gslc_tsGui *pGui, char* compStr) {

  // printf("Comp: %s\n", compStr);
  if (strcmp(compStr, "CF2S") == 0 || strcmp(compStr, "CF2SO") == 0 ||
       strcmp(compStr, "2S") == 0 || strcmp(compStr, "2SO") == 0
  ) {
    pg_sdobUpdateScoringSettings(pGui, "cf2WaySequentials");
    strlcpy(sdob_judgement->compStr, "2way Seq Open", 128);

  } else if (strcmp(compStr, "CF2SPA") == 0 || strcmp(compStr, "2SPA") == 0 || strcmp(compStr, "2SA") == 0) {
    pg_sdobUpdateScoringSettings(pGui, "cf2WaySequentials");
    strlcpy(sdob_judgement->compStr, "2way Seq ProAm", 128);

  } else if (strcmp(compStr, "CF4S") == 0 || strcmp(compStr, "CF4SO") == 0
              || strcmp(compStr, "4S") == 0 || strcmp(compStr, "4SO") == 0
  ) {
    pg_sdobUpdateScoringSettings(pGui, "cf4waySequentials");
    strlcpy(sdob_judgement->compStr, "4way Seq Open", 128);

  } else if (strcmp(compStr, "CF4R") == 0 || strcmp(compStr, "CF4RO") == 0
              || strcmp(compStr, "4R") == 0 || strcmp(compStr, "4RO") == 0
  ) {
    pg_sdobUpdateScoringSettings(pGui, "cfRotations");
    strlcpy(sdob_judgement->compStr, "4way Rotes Open", 128);

  } else if (compStr[0] != '\0') {
    strlcpy(sdob_judgement->compStr, compStr, 128);

  } else {
    strlcpy(sdob_judgement->compStr, " ", 128);
  }
  pg_sdobUpdateVideoDescTwo(pGui, sdob_judgement->compStr);

}


void pg_sdobUpdatePlayerSlider(gslc_tsGui *pGui) {
  if (pg_sdob_player_move_timepos_lock == 1) { return; }
  pg_sdob_player_move_timepos_lock = 1;
/*
  mpv_any_u* retTime;
  if ((mpvSocketSinglet("time-pos", &retTime)) != -1) {
    if (retTime == NULL) {
      pg_sdob_player_move_timepos_lock = 0;
      MPV_ANY_U_FREE(retTime);
      return;
    } else
    if (pg_sdob_player_move > -1) {
      pg_sdob_player_move_timepos_lock = 0;
      MPV_ANY_U_FREE(retTime);
      return;
    } 
    
    dbgprintf(DBG_DEBUG, "Timepos: %f\n", retTime->floating);
    if (retTime->hasPtr == 1) {
      // printf("Has PTR\n");
      libmpvCache->player->position = atof(retTime->ptr);
    } else {
      // printf("No Ptr: %f\n", retTime->floating);
      libmpvCache->player->position = retTime->floating;
    }
    
    MPV_ANY_U_FREE(retTime);
  } else {
    libmpvCache->player->position = 0;
  }

  if (sdob_judgement->sowt == -1.0 || !pg_sdob_timeline_zoom_workingtime) {

    secs_to_time((int)(libmpvCache->player->position * 1000000), libmpvCache->player->positionStr, 32);
    setSliderPosByTime(pGui);
  } else {
    double insideWorkingTime = (libmpvCache->player->position - sdob_judgement->sowt);
    secs_to_time((int)(insideWorkingTime * 1000000), libmpvCache->player->positionStr, 32);

    int nTick = (int)((insideWorkingTime * 1000) / sdob_judgement->workingTime);
    if (nTick >= 0) {
      gslc_ElemXSliderSetPos(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], nTick);
    }
  }
  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_PL_POS], libmpvCache->player->positionStr);
  */
  pg_sdob_player_move_timepos_lock = 0;
}


void pg_sdobUpdateScoringSettings(gslc_tsGui *pGui, char* str) {
  
  if (strcmp(str, "cfRotations") == 0 || strcmp(str, "CF4R") == 0) {
    sdob_judgement->prestartTime = 30.0;
    sdob_judgement->workingTime = 90.0;
    sdob_judgement->postFreezeFrameTime = 0.0;
    sdob_judgement->tossStartCount = 0;
    strlcpy(sdob_judgement->ruleSet, str, 64);

  } else if (strcmp(str, "cf2WaySequentials") == 0 || strcmp(str, "CF2") == 0 || strcmp(str, "CF2S") == 0) {
    sdob_judgement->prestartTime = 30.0;
    sdob_judgement->workingTime = 60.0;
    sdob_judgement->postFreezeFrameTime = 0.0;
    sdob_judgement->tossStartCount = 0;
    strlcpy(sdob_judgement->ruleSet, str, 64);

  } else if (strcmp(str, "cf4waySequentials") == 0 || strcmp(str, "CF4") == 0 || strcmp(str, "CF4S") == 0) {
    sdob_judgement->prestartTime = 30.0;
    sdob_judgement->workingTime = 120.0;
    sdob_judgement->postFreezeFrameTime = 0.0;
    sdob_judgement->tossStartCount = 0;
    strlcpy(sdob_judgement->ruleSet, str, 64);

  } else if (strcmp(str, "fsSpeed") == 0 || strcmp(str, "FSSPEED") == 0) {
    sdob_judgement->prestartTime = 45.0;
    sdob_judgement->workingTime = 0.0;
    sdob_judgement->postFreezeFrameTime = 0.0;
    sdob_judgement->tossStartCount = 0;
    strlcpy(sdob_judgement->ruleSet, str, 64);

  } else {
    sdob_judgement->prestartTime = 0.0;
    sdob_judgement->workingTime = 35.0;
    sdob_judgement->postFreezeFrameTime = 0.0;
    sdob_judgement->tossStartCount = 0;
    strlcpy(sdob_judgement->ruleSet, "fs", 64);

  }
}


bool pg_sdobPlayerCbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  if (sdob_devicehost->isHost == 0) { return true; }

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
    dbgprintf(DBG_SLIDER, "Moved Slider %f, Event: %d\n", move_tmp, pSlider->eTouch);

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
 //  int i_max = pg_sdob_line_max * pg_sdob_slot_max;

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsRect     pRect     = pElem->rElem;




  gslc_DrawFillRect(pGui, pRect, pElem->colElemFill);
  gslc_DrawFrameRect(pGui, pRect, pElem->colElemFrame);

  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);
  // gslc_ElemDraw(pGui, E_PG_SKYDIVEORBUST, E_ELEM_SC_POINT_COUNT);
  // gslc_ElemDraw(pGui, E_PG_SKYDIVEORBUST, E_ELEM_SC_SUBMIT);
  // printf("count: %d, start line: %d\n", pg_sdob_score_count, i_line_start);
  // printf("Prestart: %f, sopst: %f, sowt: %f\n", sdob_judgement->prestartTime, sdob_judgement->sopst, sdob_judgement->sowt);

  for(i_line = 0; i_line < pg_sdob_line_max; i_line++) {
    for(i_slot = 0; i_slot < pg_sdob_slot_max; i_slot++) {

      int iXCnt = i_slot + (i_line * pg_sdob_slot_max); // slot identifier
      int i_this_mark = iXCnt + i_start_mark;
      char score[16];
      char result[16];

      // gslc_tsElemRef *elem_poi = gslc_PageFindElemById(pGui, E_PG_SKYDIVEORBUST, iXPM);
      // gslc_tsElemRef *elem_res = gslc_PageFindElemById(pGui, E_PG_SKYDIVEORBUST, iXPM);

      // printf("Line: %d, Slot: %d, iX: %d, Mark: %d\n", i_line, i_slot, iXCnt, i_this_mark);
      // Show full rounded number
      if (i_this_mark == 0 && sdob_judgement->prestartTime > 0.0 && sdob_judgement->sopst == -1.0) {
        sprintf(score, "%s", "");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_WHITE);

      } else if (i_this_mark == 0 && sdob_judgement->prestartTime > 0.0 && sdob_judgement->sopst > -1.0) {
        sprintf(score, "%s", "X");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_WHITE);


      } else if (i_this_mark == 1 && sdob_judgement->prestartTime > 0.0 && sdob_judgement->sowt == -1.0) {
        sprintf(score, "%s", "");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_WHITE);

      } else if (i_this_mark == 1 && sdob_judgement->prestartTime > 0.0 && sdob_judgement->sowt > -1.0) {
        sprintf(score, "%s", "S");
        if ((sdob_judgement->prestartTime + sdob_judgement->sopst) > sdob_judgement->sowt) {
          gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_GREEN);
        } else {
          gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_WHITE);
        }
      } else if (i_this_mark == 0 && sdob_judgement->prestartTime == 0.0 && sdob_judgement->sowt == -1.0) {
        sprintf(score, "%s", "");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_WHITE);

      } else if (i_this_mark == 0 && sdob_judgement->prestartTime == 0.0 && sdob_judgement->sowt > -1.0) {
        sprintf(score, "%s", "S");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_WHITE);

      } else if (i_this_mark == pg_sdob_score_count) {
        sprintf(score, "%s", "E");
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_WHITE);

      } else if (i_this_mark < pg_sdob_score_count && (i_this_mark + i_start_disp) % 10 == 0) {
        sprintf(score, "%d", (i_this_mark + i_start_disp));
        gslc_ElemSetTxtCol(pGui, pg_sdob_scorecard_elemsNum[iXCnt], GSLC_COL_WHITE);

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
        // // debug_print("Mark: %d, Point: %d\n", i_this_mark, sdob_judgement->marks->arrScorecardPoints[i_this_mark]);
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
    gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
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

  pg_sdobSliderChangeCurPos(pGui, -1);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

  return true;
}

// Slider Down
bool pg_sdobCbBtnSliderDown(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Clear selection so people don't get lost
  pg_sdobScoringSelectionLastHidden(pGui);
  pg_sdobSliderChangeCurPos(pGui, 1);
  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
  return true;
}

// Change Video Rate
bool pg_sdobCbBtnVideoRate(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // toggle video rate
  pg_sdobPlayerVideoRateChanging(pGui, 0);

  return true;
}







// Change Judge
bool pg_sdobCbBtnChangeJudge(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (cbInit[E_PG_KEYBOARD] != NULL) { cbInit[E_PG_KEYBOARD](pGui); }
  pg_keyboard_shiftOn = 1;
  pg_keyboard_show(pGui, 3, sdob_judgement->judge, &pg_sdobUpdateJudgeInitials);
  return true;
}

// Team Desc
bool pg_sdobCbBtnTeamDesc(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (sdob_current_rounds) {
    if (sdob_num_current_rounds > 0) {
      // Automated mode if file declares the triplets
      touchscreenPageOpen(pvGui, E_PG_SDOB_ROUNDLIST);
    }
  }
  else {
    // Manual mode w/keyboard
    if (cbInit[E_PG_KEYBOARD] != NULL) { cbInit[E_PG_KEYBOARD](pGui); }
    pg_keyboard_shiftOn = 0;
    pg_keyboard_show(pGui, 6, sdob_judgement->team, &pg_sdobUpdateTeam);
  }

//  setKeyboardCallbackFunc(&pg_sdobUpdateTeam);
//  strcpy(keyboardInput, sdob_judgement->team);
//  m_show_keyboard = 6;
//  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_TEAM_DESC], sdob_judgement->team);

  return true;
}

// Round Desc
bool pg_sdobCbBtnRoundDesc(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (sdob_current_rounds) {
    if (sdob_num_current_rounds > 0) {
      // Automated mode if file declares the triplets
      touchscreenPageOpen(pvGui, E_PG_SDOB_ROUNDLIST);
    }
  }
  else {
    // Manual mode w/keyboard
    if (cbInit[E_PG_KEYBOARD] != NULL) { cbInit[E_PG_KEYBOARD](pGui); }
    pg_keyboard_shiftOn = 0;
    pg_keyboard_show(pGui, 3, sdob_judgement->round, &pg_sdobUpdateRound);
  }

//  setKeyboardCallbackFunc(&pg_sdobUpdateRound);
//  strcpy(keyboardInput, sdob_judgement->round);
//  m_show_keyboard = 3;
//  gslc_ElemSetTxtStr(pGui, pg_sdobEl[E_SDOB_EL_ROUND_DESC], sdob_judgement->round);

  return true;
}

// Meet Desc
bool pg_sdobCbBtnMeetDesc(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SDOB_VIDEOLIST);

  return true;
}

// Video Desc
bool pg_sdobCbBtnVideoDesc(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SDOB_VIDEOLIST);
  return true;
}





// Scorecard Count
bool pg_sdobCbBtnScCount(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  touchscreenPageOpen(pGui, E_PG_SDOB_DISCIPLINELIST);
  return true;
}



// Submit Button
bool pg_sdobCbBtnSubmitBtn(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }

  if (!strlen(sdob_judgement->team) || !strlen(sdob_judgement->round)) {
    // Prohibit submission if we do not have a valid team/round
    return true;
  }

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

/*
  struct queue_head *item = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(item);
  item->action = E_Q_SCORECARD_SUBMIT_SCORECARD;
  pg_sdob_add_action(&item);
  // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
*/
  touchscreenPageOpen(pGui, E_PG_SDOB_SUBMIT);

  return true;
}



// Mirror Button
bool pg_sdobCbBtnMirrorBtn(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  guislice_wrapper_mirror_toggle(pGui);
  return true;
}



// Pause Callback
bool pg_sdobPlCbBtnPlayPause(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (libmpvCache->player->is_loaded) {
    struct queue_head *item = malloc(sizeof(struct queue_head));
    INIT_QUEUE_HEAD(item);
    item->action = E_Q_PLAYER_VIDEO_TOGGLE;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    // mpv_playpause_toggle();
  } else {
    // struct pg_sdob_video_data *newVid = PG_SDOB_INIT_VIDEO_DATA();
    // strlcpy(newVid->local_folder, sdob_judgement->video->local_folder, 256);
    // strlcpy(newVid->video_file, sdob_judgement->video->video_file, 256);
    // strlcpy(newVid->url, sdob_judgement->video->url, 512); 
    //pg_skydiveorbust_loadvideo(pGui, newVid);
  }

  return true;
}


// Play Callback
bool pg_sdobPlCbBtnPlay(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  
  struct queue_head *itemClean = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemClean);
  itemClean->action = E_Q_SDOB_CLEAR_SELECTING;
  pg_sdob_add_action(&itemClean);
  // queue_put(itemClean, pg_sdobQueue, &pg_sdobQueueLen);

  struct queue_head *item = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(item);
  item->action = E_Q_PLAYER_VIDEO_PLAY;
  pg_sdob_add_action(&item);
  // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

  return true;
}


// Stop Callback
bool pg_sdobPlCbBtnStop(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  struct queue_head *itemClean = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemClean);
  itemClean->action = E_Q_SDOB_CLEAR_SELECTING;
  pg_sdob_add_action(&itemClean);
  // queue_put(itemClean, pg_sdobQueue, &pg_sdobQueueLen);

  struct queue_head *item = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(item);
  item->action = E_Q_PLAYER_VIDEO_STOP;
  pg_sdob_add_action(&item);
  libmpvCache->player->is_loaded = 0;

  struct queue_head *itemClear = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemClear);
  itemClear->action = E_Q_SDOB_CLEAR;
  pg_sdob_add_action(&itemClear);
  // queue_put(itemClear, pg_sdobQueue, &pg_sdobQueueLen);

  return true;
}


// Forward Callback
bool pg_sdobPlCbBtnForward(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  struct queue_head *itemClean = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemClean);
  itemClean->action = E_Q_SDOB_CLEAR_SELECTING;
  pg_sdob_add_action(&itemClean);
  // queue_put(itemClean, pg_sdobQueue, &pg_sdobQueueLen);

  struct queue_head *item = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(item);
  item->action = E_Q_PLAYER_VIDEO_FORWARD;
  pg_sdob_add_action(&item);
  // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  // mpv_seek(2);

  return true;
}


// Back Callback
bool pg_sdobPlCbBtnBack(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  // gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  struct queue_head *itemClean = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(itemClean);
  itemClean->action = E_Q_SDOB_CLEAR_SELECTING;
  pg_sdob_add_action(&itemClean);
  // queue_put(itemClean, pg_sdobQueue, &pg_sdobQueueLen);

  struct queue_head *item = malloc(sizeof(struct queue_head));
  INIT_QUEUE_HEAD(item);
  item->action = E_Q_PLAYER_VIDEO_BACKWARD;
  pg_sdob_add_action(&item);
  // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  // mpv_seek(-2);

  return true;
}




// Defined Slow Callback
bool pg_sdobPlCbBtnUserDefinedSlow(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN || sdob_devicehost->isHost == 0) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  // Stop Changing Video Rate
  pg_sdobPlayerVideoRateChanging(pGui, 2);

  // Stop Clearing Scorecard Selection
  if (sdob_judgement->marks->selected > 0) {
    pg_sdobScoringSelectionClear(pGui);
  }

  if (libmpvCache->player->pbrateUser == libmpvCache->player->pbrate) {
    pg_sdobUpdateVideoRate(pGui, mpv_speed(1.0));
  } else {
    pg_sdobUpdateVideoRate(pGui, mpv_speed(libmpvCache->player->pbrateUser));
  }

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
        if (wantedSel <= sdob_judgement->marks->size) {
          sdob_judgement->marks->selected = wantedSel;
        } else {
          sdob_judgement->marks->selected = -1;
        }

        dbgprintf(DBG_DEBUG, "Sel: %d, T: %f\n", i, sdob_judgement->marks->arrScorecardTimes[i]);
        gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

        // if (sdob_judgement->marks->selected == 0) {
        //   mpv_pause();
        // }
        if (sdob_judgement->marks->selected == sdob_judgement->marks->size) {
          mpv_seek_arg((sdob_judgement->sowt + sdob_judgement->workingTime), "absolute+exact");
        } else if (sdob_judgement->marks->selected > -1 &&
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
          mpv_pause();
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
          item->cmd = strdup("set;pause;yes");
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



 // Submit Button
  if ((
    pg_sdobEl[E_SDOB_EL_BTN_SUBMIT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect) {333,71,107,38},
          (char*)"Submit", 0, E_FONT_MONO18, &pg_sdobCbBtnSubmitBtn)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_BTN_SUBMIT], GSLC_COL_BLACK);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_BTN_SUBMIT], GSLC_COL_GRAY, GSLC_COL_RED_DK1, GSLC_COL_GREEN);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_BTN_SUBMIT], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobEl[E_SDOB_EL_BTN_SUBMIT], true);
  }
/*
 // Mirror Button
  if ((
    pg_sdobEl[E_SDOB_EL_BTN_MIRROR] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect) {440,110,40,130},
          (char*)"M", 0, E_FONT_MONO18, &pg_sdobCbBtnMirrorBtn)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SDOB_EL_BTN_MIRROR], GSLC_COL_BLACK);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_BTN_MIRROR], GSLC_COL_GRAY, GSLC_COL_RED_DK1, GSLC_COL_GREEN);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SDOB_EL_BTN_MIRROR], GSLC_ALIGN_MID_MID);
    gslc_ElemSetFillEn(pGui, pg_sdobEl[E_SDOB_EL_BTN_MIRROR], true);
  }
*/



  // Row One Under Scorecard (Event)
  if ((
    pg_sdobEl[E_SBOD_EL_DESC_ONE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect) {0,280,230,20},
          (char*)" ", 0, E_FONT_MONO18, &pg_sdobCbBtnMeetDesc)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SBOD_EL_DESC_ONE], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SBOD_EL_DESC_ONE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SBOD_EL_DESC_ONE], GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SBOD_EL_DESC_ONE], false);
  }

  // Row Two Under Scorecard (Comp)
  if ((
    pg_sdobEl[E_SBOD_EL_DESC_TWO] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect) {0,300,230,20},
          (char*)" ", 0, E_FONT_MONO18, &pg_sdobCbBtnVideoDesc)
  ) != NULL) {
    gslc_ElemSetTxtCol(pGui, pg_sdobEl[E_SBOD_EL_DESC_TWO], GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, pg_sdobEl[E_SBOD_EL_DESC_TWO],GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
    gslc_ElemSetTxtAlign(pGui, pg_sdobEl[E_SBOD_EL_DESC_TWO],GSLC_ALIGN_MID_LEFT);
    gslc_ElemSetFrameEn(pGui, pg_sdobEl[E_SBOD_EL_DESC_TWO],false);
  }



  dbgprintf(DBG_DEBUG, "Slot Max: %d, Line Max: %d\n", pg_sdob_slot_max, pg_sdob_line_max);

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

      dbgprintf(DBG_DEBUG, "Id: %d, Slot %d, Line: %d, Slot Max: %d - X: %d\n", iXCnt, i_slot, i_line, pg_sdob_slot_max, iX);


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
//  if ((
//    pg_sdobEl[E_SDOB_EL_PL_PAUSE] = gslc_ElemCreateBtnImg(pGui, GSLC_ID_AUTO,
//        ePage, (gslc_tsRect){208,0,64,64},
//        gslc_GetImageFromFile(IMG_BTN_PAUSE,GSLC_IMGREF_FMT_BMP16),
//        gslc_GetImageFromFile(IMG_BTN_PAUSE_SEL,GSLC_IMGREF_FMT_BMP16),
//        &pg_sdobPlCbBtnPlayPause)
//  ) != NULL) {
//
//  }

  // Play
  if ((
    pg_sdobEl[E_SDOB_EL_PL_PLAY] = gslc_ElemCreateBtnImg(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect){208,0,64,64},
          gslc_GetImageFromFile(IMG_BTN_PLAY,GSLC_IMGREF_FMT_BMP16),
          gslc_GetImageFromFile(IMG_BTN_PLAY_SEL,GSLC_IMGREF_FMT_BMP16),
          &pg_sdobPlCbBtnPlayPause)
  ) != NULL) {

  }

  // Stop
  if ((
    pg_sdobEl[E_SDOB_EL_PL_STOP] = gslc_ElemCreateBtnImg(pGui, GSLC_ID_AUTO,
          ePage, (gslc_tsRect){312,0,64,64},
          gslc_GetImageFromFile(IMG_BTN_STOP,GSLC_IMGREF_FMT_BMP16),
          gslc_GetImageFromFile(IMG_BTN_STOP_SEL,GSLC_IMGREF_FMT_BMP16),
          &pg_sdobPlCbBtnStop)
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
        (char*)"100%", 0,
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
          ePage, &pg_sdob_pl_slider, (gslc_tsRect){15,240,450,35},
          0, 1000, pg_sdobPlayerPos, 10, false)
  ) != NULL) {

    gslc_ElemSetCol(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_COL_RED, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemSetFillEn(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], false);
    gslc_ElemXSliderSetStyle(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], true, GSLC_COL_RED_DK4, 0, 25, GSLC_COL_GRAY_DK2);
    gslc_ElemXSliderSetPosFunc(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], &pg_sdobPlayerCbSlidePos);
  }


  // Video Position Timestamp
  if ((
    pg_sdobEl[E_SDOB_EL_PL_POS] = gslc_ElemCreateTxt(pGui, GSLC_ID_AUTO,
        ePage, (gslc_tsRect) {230,285,170,35},
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
          ePage, (gslc_tsRect){400,285,80,35},
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

  if (pg_sdob_change_video_rate) {
    item = new_qhead();
    item->action = E_Q_ACTION_VIDEO_RATE_ADJUST;
    item->amt = .05;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    // struct queue_head *itemUser = new_qhead();
    // itemUser->action = E_Q_ACTION_VIDEO_RATE_USER;
    // queue_put(itemUser, pg_sdobQueue, &pg_sdobQueueLen);


  } else if (sdob_judgement->marks->selected <= 0) {
    // Framestep
    //
    // It has been observed that the "frame-step" function in mpv
    // applies the latest speed setting to the single-frame playback.
    // This means a spin of the frame-forward knob can be slow if the
    // speed had been set to a low value.  Change the speed setting
    // here to 10x.  The next thing that resumes playing has to re-set
    // the speed no matter what.
    //
    // The constant changing of speed seems to not play nice with mpv right now
    // thinking start a debounce delay after the second click within a short time
    // static const char cmdFSTxt[] = "frame-step\n";
      // "set speed 10.0\n"
      // "show-text \"\"\n"          /* clears the "Speed: 10.00" on the OSD */
      // "frame-step\n";
    // libmpv_zmq_cmd(strdup(cmdFSTxt));
    item = new_qhead();
    item->action = E_Q_ACTION_MPV_COMMAND;
    item->cmd = strdup("frame-step");
    pg_sdob_add_action(&item);

    // item = new_qhead();
    // item->action = E_Q_PLAYER_VIDEO_FORWARD_STEP;
    // pg_sdob_add_action(&item);

    // Reset the Speed back to current
    // struct queue_head *itemSpeed = new_qhead();
    // itemSpeed->action = E_Q_ACTION_VIDEO_RATE;
    // itemSpeed->amt = libmpvCache->player->pbrate;

    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    // queue_put(itemSpeed, pg_sdobQueue, &pg_sdobQueueLen);
  } else {
    item = new_qhead();
    item->action = E_Q_SCORECARD_MOVE_SCORE_SELECTED;
    item->amt = 1;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  }
}

void pg_skydiveorbustButtonRotaryCCW() {
  struct queue_head *item;


  if (pg_sdob_change_video_rate) {
    item = new_qhead();
    item->action = E_Q_ACTION_VIDEO_RATE_ADJUST;
    item->amt = -.05;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    // struct queue_head *itemUser = new_qhead();
    // itemUser->action = E_Q_ACTION_VIDEO_RATE_USER;
    // queue_put(itemUser, pg_sdobQueue, &pg_sdobQueueLen);


  // Not on a selection mark
  } else if (sdob_judgement->marks->selected <= 0) {
    // Adjust SOWT Framestep
    // Frameback step really horrible, rather move back to last keyframe
    item = new_qhead();
    item->action = E_Q_ACTION_MPV_COMMAND;
    item->cmd = strdup("frame-back-step");
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    // mpv_seek(-2);


  // Skip to next point
  } else {
    item = new_qhead();
    item->action = E_Q_SCORECARD_MOVE_SCORE_SELECTED;
    item->amt = -1;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
  }
}

void pg_skydiveorbust_update_duration(double duration) {
  libmpvCache->player->duration = duration;
}


void pg_skydiveorbustButtonLeftPressed() {
  struct queue_head *item;

  dbgprintf(DBG_DEBUG, "Left Pressed: %d\n", sdob_judgement->marks->selected);

  if (sdob_judgement->marks->selected < 0) {
    // Add to queue E_Q_SCORECARD_INSERT_MARK
    // double markTime = libmpv_zmq_get_prop_double("playback-time");

    item = new_qhead();
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->mark = E_SCORES_BUST;
    item->time = mpv_calc_marktime(libmpvCache->player);
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

  } else {
    // Add to queue E_Q_SCORECARD_UPDATE_MARK
    item = new_qhead();
    item->action = E_Q_SCORECARD_UPDATE_MARK;
    item->selected = sdob_judgement->marks->selected;
    item->mark = E_SCORES_BUST;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    pg_sdobScoringSelectionClear(&m_gui);
  }
}

void pg_skydiveorbustButtonRightPressed() {
  struct queue_head *item;

  dbgprintf(DBG_DEBUG, "Right Pressed: %d - %f - %f, %d\n", sdob_judgement->marks->selected, sdob_judgement->prestartTime, sdob_judgement->sopst, sdob_devicehost->isHost);
  // Scorecard Clicks

  if (sdob_judgement->marks->selected < 0) {
    // double markTime = libmpv_zmq_get_prop_double("playback-time");

    item = new_qhead();
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->mark = E_SCORES_POINT;
    item->time = mpv_calc_marktime(libmpvCache->player);;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

  } else {
    // Add to queue E_Q_SCORECARD_UPDATE_MARK
    item = new_qhead();
    item->action = E_Q_SCORECARD_UPDATE_MARK;
    item->selected = sdob_judgement->marks->selected;
    item->mark = E_SCORES_POINT;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    pg_sdobScoringSelectionClear(&m_gui);
  }
}

void pg_skydiveorbustButtonRotaryPressed() {
  if (sdob_judgement->marks->selected == 0) {
    // Reset SOWT

  } else if (sdob_judgement->marks->selected < 0) {

  } else {
    struct queue_head *itemSeek = new_qhead();
    itemSeek->action = E_Q_ACTION_MPV_COMMAND;

    double timePos = sdob_judgement->marks->arrScorecardTimes[sdob_judgement->marks->selected];
    size_t seekSz = snprintf(NULL, 0, "seek;%f;%s;", timePos, "absolute+exact") + 1;
    if (seekSz > 0) {
      itemSeek->cmd = (char*)malloc(seekSz * sizeof(char));
      snprintf(itemSeek->cmd, seekSz, "seek;%f;%s;", timePos, "absolute+exact");
      pg_sdob_add_action(&itemSeek);
      // queue_put(itemSeek, pg_sdobQueue, &pg_sdobQueueLen);
    }

    // struct queue_head *itemMove = new_qhead();
    // itemMove->action = E_Q_SCORECARD_MOVE_SCORE_SELECTED;
    // // Clear Selection
    // // itemMove->selected = -1;
    // // queue_put(itemMove, pg_sdobQueue, &pg_sdobQueueLen);
  }
}

void pg_skydiveorbustButtonLeftHeld() {
  if (sdob_judgement->marks->selected >= 0) {
    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_DELETE_MARK;
    item->selected = sdob_judgement->marks->selected;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    // Skip Release Action
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] = 2;    
  } else {

  }
}

void pg_skydiveorbustButtonRightHeld() {
  if (sdob_judgement->marks->selected >= 0) {
    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_INSERT_MARK;
    item->selected = sdob_judgement->marks->selected;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
    
    // Skip Release Action
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] = 2;
  } else {

  }
}

void pg_skydiveorbustButtonRotaryHeld() {
  if (sdob_judgement->marks->selected >= 0) {
    struct queue_head *item = new_qhead();
    item->action = E_Q_SCORECARD_UPDATE_MARK;
    item->mark = E_SCORES_OMISSION;
    item->selected = sdob_judgement->marks->selected;
    pg_sdob_add_action(&item);
    // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);

    // Skip Release Action
    lib_buttonsLastInterruptAction[E_BUTTON_ROTARY_HELD] = 2;
  } else {
    guislice_wrapper_mirror_toggle(&m_gui);
  }
}

void pg_skydiveorbustButtonDoubleHeld() {
  lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] = 2;
  lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] = 2;
  if (m_page_stackLen == 0) {
    touchscreenPageOpen(&m_gui, E_PG_MAIN);
  } else {
    touchscreenPageGoBack(&m_gui);
  }
}

void pg_skydiveorbustButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_skydiveorbustButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_skydiveorbustButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_PRESSED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_PRESSED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_PRESSED, NULL);
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
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_PRESSED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_PRESSED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_PRESSED, NULL);  
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, NULL);
}










// External updates executed on each new TimePos
void pg_sdob_mpv_timepos_thread() {
  // Grab Player Time
  // printf("Move Slide(Forcd: %d) %d, %d, %f\n", pg_sdob_pl_sliderForceUpdate, sdob_devicehost->isHost, libmpvCache->player->is_playing, libmpvCache->player->duration);

  if ( 
    ( libmpvCache->player->is_playing == 1 &&
      libmpvCache->player->duration > 0.00
    ) || pg_sdob_pl_sliderForceUpdate == 1
  ) {
    pg_sdob_pl_sliderForceUpdate = 0;
    pg_sdobUpdatePlayerSlider(&m_gui);

    // Check prestart
    if (
      sdob_judgement->sowt == -1 &&
      sdob_judgement->prestartTime > 0 && sdob_judgement->sopst > -1.0 && 
      (sdob_judgement->prestartTime + sdob_judgement->sopst) <= libmpvCache->player->position
    ) {
      struct queue_head *itemSOWT = new_qhead();
      itemSOWT->action = E_Q_SCORECARD_SCORING_SOWT;
      itemSOWT->amt = sdob_judgement->workingTime;
      itemSOWT->time = (sdob_judgement->prestartTime + sdob_judgement->sopst);
      pg_sdob_add_action(&itemSOWT);
      // queue_put(itemSOWT, pg_sdobQueue, &pg_sdobQueueLen);
      sendBeep();
    }

  } else if (libmpvCache->player->has_seeked == 1 && libmpvCache->player->is_seeking == 0) {
    libmpvCache->player->has_seeked = 0;
    pg_sdobUpdatePlayerSlider(&m_gui);
  }
}



// ------------------------
// MPV Output Events Thread
// ------------------------
void * pg_sdobMpvEventsThread(void *input) {
  if (pg_sdobMpvEventsThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting MPV Events Thread, Already Started");
    return NULL;
  }
  void *videoserver = NULL;

  pg_sdobMpvEventsThreadRunning = 1;
  
  if (pg_sdobMpvEventsThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting MPV Events Thread, Stop Flag Set");
    goto cleanup;
  }
  if (libmpvCache->server->videoserver == NULL) {
    dbgprintf(DBG_DEBUG, "%s\n", "Finding Video Server");

    config_t cfg;
    config_init(&cfg);
    // Read the file. If there is an error, report it and exit.
    if (access(config_path, F_OK) == -1 || !config_read_file(&cfg, config_path)) {
      dbgprintf(DBG_DEBUG, "Cannot Find config_path: %s\n", config_path);
      config_destroy(&cfg);
      goto cleanup;
    }

    const char * retVideoServer;
    if (config_lookup_string(&cfg, "videoserver", &retVideoServer)) {
      libmpvCache->server->videoserver = strdup(retVideoServer);
    } else {
      printf("No videoserver configuration in ~/.config/sdobox/sdobox.conf\n");
      config_destroy(&cfg);
      goto cleanup;
    }
    config_destroy(&cfg);
  }

  dbgprintf(DBG_DEBUG, "%s\n", "Starting MPV Events Thread");
  int rc = 0;

  // Try starting time server
  rc = zmq_connect_socket(&videoserver, libmpvCache->server->videoserver, ZMQ_SUB);
  // printf("Time Server: %d - %d\n", rc, ZMQ_SUB);
  while (rc < 0 && !pg_sdobMpvEventsThreadKill) {
    zmq_close(videoserver);
    sleep(2);
    rc = zmq_connect_socket(&videoserver, libmpvCache->server->videoserver, ZMQ_SUB);
  }

  // Did connect, try initialize subscriptions
  if (rc > -1 && !pg_sdobMpvEventsThreadKill) {
    dbgprintf(DBG_DEBUG, "Video Server Connected, %s - %d\n", libmpvCache->server->videoserver, rc);
    const char *filtervideoserver = "";
    rc = zmq_setsockopt (videoserver, ZMQ_SUBSCRIBE, filtervideoserver, strlen(filtervideoserver));

    // printf("Timeserver RC: %d, Error: %d\n", rc, errno);
    // Failed to initialize subscriptions;
    if (rc < 0) {
      dbgprintf(DBG_DEBUG, "%s\n", "Cannot Subscribe to Video Server");   
      // printf("%s\n", "Shutdown Events Thread");
      goto cleanup;
    }
 
    // 0MQs sockets to poll
    zmq_pollitem_t items [] = {
      { videoserver, 0, ZMQ_POLLIN, 0 }
    };
    
    while (!pg_sdobMpvEventsThreadKill) {
      rc = zmq_poll (items, 1, -1);
      if (!m_bQuit && rc > -1) {  
        if (items[0].revents & ZMQ_POLLIN) {
          char *str = s_recv(videoserver);
          dbgprintf(DBG_DEBUG, "videoserver: %s\n", str);

          if (strcmp(str, "timer") == 0) {
            libmpvCache->player->position_update = s_clock();
            libmpvCache->player->position = strtod(s_recv(videoserver), NULL);
            pg_sdob_mpv_timepos_thread();
          } else if (strcmp(str, "duration") == 0) {
            const char* myDuration = s_recv(videoserver) + 9;
            // printf("Duration: %s\n", myDuration);
            libmpvCache->player->duration = strtod(myDuration, NULL);
            pg_sdob_pl_sliderForceUpdate = 1;
            pg_sdob_mpv_timepos_thread();
          } else if (strcmp(str, "file-loaded") == 0) {
            struct queue_head *item = malloc(sizeof(struct queue_head));
            INIT_QUEUE_HEAD(item);
            item->action = E_Q_SCORECARD_CLEAR;
            pg_sdob_add_action(&item);
            // queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
            libmpvCache->player->is_loaded = 1;
            pg_sdob_pl_sliderForceUpdate = 1;
          } else if (strcmp(str, "pause") == 0) {
            libmpvCache->player->is_playing = 0;
          } else if (strcmp(str, "unpause") == 0) {
            libmpvCache->player->is_playing = 1;
          }
          free(str);
        }
      }
    }
  } else {
    goto cleanup;
  }
  
 cleanup:
  zmq_close(videoserver);
  videoserver = NULL; 
  // printf("%s\n", "Closing TimePos and Events Thread");
  pg_sdobMpvEventsThreadRunning = 0;
  pthread_exit(NULL);
}


int pg_sdobMpvEventsThreadStart() {
  dbgprintf(DBG_DEBUG, "%s\n", "pg_sdobMpvEventsThreadStart()");
  if (pg_sdobMpvEventsThreadRunning) { return 0; }

  // pg_sdob_pl_sliderForceUpdate = 1;
  dbgprintf(DBG_DEBUG, "SkydiveOrBust MPV Events Thread Spinup: %d\n", pg_sdobMpvEventsThreadRunning);
  pg_sdobMpvEventsThreadKill = 0;
  pthread_t tid;
  return pthread_create(&tid, NULL, &pg_sdobMpvEventsThread, NULL);
}

void pg_sdobMpvEventsThreadStop() {
  dbgprintf(DBG_DEBUG, "%s\n", "pg_sdobMpvEventsThreadStop()");
  // Shutdown MPV FIFO Thread
  int shutdown_cnt = 0;
  if (pg_sdobMpvEventsThreadRunning) {
    pg_sdobMpvEventsThreadKill = 1;
    while (pg_sdobMpvEventsThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
  }
  dbgprintf(DBG_DEBUG, "SkydiveOrBust MPV Events Thread Shutdown %d\n", shutdown_cnt);
}



// ------------------------
// MPV Output TimePos Thread
// ------------------------
void * pg_sdobMpvTimeposThread(void *input) {
  if (pg_sdobMpvTimeposThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting MPV TimePos Thread, Already Started");
    return NULL;
  }
  
  pg_sdobMpvTimeposThreadRunning = 1;
  
  if (pg_sdobMpvTimeposThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting MPV TimePos Thread, Stop Flag Set");
    goto cleanup;
  }

  dbgprintf(DBG_DEBUG, "%s\n", "Starting MPV TimePos Thread");
  int rc = 0;
    
  while (!pg_sdobMpvTimeposThreadKill) {
    // printf("Timestamp: %f @ %lld\n", libmpvCache->player->position, libmpvCache->player->position_update);
    
    double realTime = libmpvCache->player->position;
    if (libmpvCache->player->is_playing) {
      double nDiff = (((double)(s_clock() - libmpvCache->player->position_update)) * libmpvCache->player->pbrate) / 1000;
      realTime = realTime + nDiff;
    }
      
    secs_to_time((int)(realTime * 1000000), libmpvCache->player->positionStr, 32);
    setSliderPosByTime(&m_gui);
    gslc_ElemSetTxtStr(&m_gui, pg_sdobEl[E_SDOB_EL_PL_POS], libmpvCache->player->positionStr);

    usleep(1000000);
  }
  
 cleanup:
  // printf("%s\n", "Closing TimePos and Events Thread");
  pg_sdobMpvTimeposThreadRunning = 0;
  pthread_exit(NULL);
}


int pg_sdobMpvTimeposThreadStart() {
  dbgprintf(DBG_DEBUG, "%s\n", "pg_sdobMpvTimeposThreadStart()");
  if (pg_sdobMpvTimeposThreadRunning) { return 0; }

  // pg_sdob_pl_sliderForceUpdate = 1;
  dbgprintf(DBG_DEBUG, "SkydiveOrBust MPV TimePos Thread Spinup: %d\n", pg_sdobMpvTimeposThreadRunning);
  pg_sdobMpvTimeposThreadKill = 0;
  pthread_t tid;
  return pthread_create(&tid, NULL, &pg_sdobMpvTimeposThread, NULL);
}

void pg_sdobMpvTimeposThreadStop() {
  dbgprintf(DBG_DEBUG, "%s\n", "pg_sdobMpvTimeposThreadStop()");
  // Shutdown MPV FIFO Thread
  int shutdown_cnt = 0;
  if (pg_sdobMpvTimeposThreadRunning) {
    pg_sdobMpvTimeposThreadKill = 1;
    while (pg_sdobMpvTimeposThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
  }
  dbgprintf(DBG_DEBUG, "SkydiveOrBust MPV TimePos and Events Thread Shutdown %d\n", shutdown_cnt);
}






/*
int pg_skydiveorbust_parse_filename_default(gslc_tsGui *pGui, struct pg_sdob_video_data *newVideo) {

  // attempt to regex match filename to event info
  regmatch_t matches[4];
  int m = regexec(&r_svr_wholefilename, newVideo->video_file, 2, matches, 0);
  if (m) {
    // not a match, restore default working time
    dbgprintf(DBG_DEBUG, "video filename '%s' does not match default svr pattern\n", newVideo->video_file);
    return m;
  }

  // Store the meet name
  memset(svr_parsed_meet,0,sizeof(svr_parsed_meet));
  strncpy(svr_parsed_meet, newVideo->video_file, MIN(63,matches[1].rm_eo));
  dbgprintf(DBG_INFO|DBG_SVR, "SVR: meet = %s\n", svr_parsed_meet);

  char* subline = newVideo->video_file + matches[1].rm_eo;
  m = regexec(&r_svr_omniskore, subline, 4, matches, 0);
  if (m) {
    return m;
  }

  char *svr_eventname = strndup(subline+matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
  char *svr_rnd = strndup(subline+matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
  char *svr_team = strndup(subline+matches[3].rm_so, matches[3].rm_eo - matches[3].rm_so);

  char *eventStr = NULL;
  int ptrCnt = 0;
  char *compStr;

  // Parse Foldername for comp info
  // printf("Local Folder: %s\n", sdob_judgement->local_folder);
  char delim[] = "/";
  char * ptrFolder = strtok(newVideo->local_folder, delim);
  compStr = strdup(ptrFolder);
  while ((ptrFolder = strtok(NULL,delim)) != NULL) {
    if (ptrCnt > 0) { free(eventStr); }
    *eventStr = *compStr;
    compStr = strdup(ptrFolder);
    ptrCnt++;
  }
  free(ptrFolder);
  
  PG_SDOB_FREE_VIDEO_DATA(newVideo);
  
  if (ptrCnt > 1) {
    pg_sdobUpdateEvent(pGui, eventStr);
    free(eventStr);
  } else {
    pg_sdobUpdateEvent(pGui, svr_parsed_meet);
  }

  if (ptrCnt > 0) {
    pg_sdobUpdateComp(pGui, compStr);
  } else {
    pg_sdobUpdateComp(pGui, "");
  }
  free(compStr);
  dbgprintf(DBG_SVR|DBG_INFO, "SVR: event=%s, team=%s, round=%s\n", svr_eventname, svr_team, svr_rnd);

  pg_sdobUpdateRound(pGui, svr_rnd);
  pg_sdobUpdateTeam(pGui, svr_team);

  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);

  free(svr_eventname);
  free(svr_rnd);
  free(svr_team);
  return 0;
}
*/

int pg_skydiveorbust_parse_filename_tammy(gslc_tsGui *pGui, struct pg_sdob_video_data *newVideo) {

  if (newVideo->video_file[0] == '\0') {
     // printf("no filename to load\n");
     return 1;
  }
  // attempt to regex match filename to event info
  regmatch_t matches[5];
  int m = regexec(&r_svr_tammy, newVideo->video_file, 5, matches, 0);
  if (m) {
    // not a match, restore default working time
    dbgprintf(DBG_DEBUG, "video filename '%s' does not match tammy svr pattern\n", newVideo->video_file);
    return m;
  }

  char *svr_event = strndup(newVideo->video_file+matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
  char *svr_comp = strndup(newVideo->video_file+matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
  char *svr_rnd = strndup(newVideo->video_file+matches[3].rm_so + 1, matches[3].rm_eo - matches[3].rm_so - 1);
  char *svr_team = strndup(newVideo->video_file+matches[4].rm_so, matches[4].rm_eo - matches[4].rm_so);
  dbgprintf(DBG_SVR|DBG_INFO, "SVRZ: event=%s, comp=%s, team=%s, round=%s\n", svr_event, svr_comp, svr_team, svr_rnd);

  pg_sdobUpdateRound(pGui, svr_rnd);
  pg_sdobUpdateTeam(pGui, svr_team);
  pg_sdobUpdateEvent(pGui, svr_event);
  pg_sdobUpdateComp(pGui, svr_comp);

  // printf("SVR: event=%s, comp=%s, team=%s, round=%s\n", svr_event, svr_comp, svr_team, svr_rnd);

  free(svr_event);
  free(svr_comp);
  free(svr_rnd);
  free(svr_team);
  return 0;
}

/*
int pg_skydiveorbust_parse_filename_omniskore(gslc_tsGui *pGui, struct pg_sdob_video_data *newVideo) {

  if (newVideo->video_file[0] == '\0') {
     // printf("no filename to load\n");
     return 1;
  }
  // attempt to regex match filename to event info
  regmatch_t matches[5];
  int m = regexec(&r_svr_omniskore, newVideo->video_file, 5, matches, 0);
  if (m) {
    // not a match, restore default working time
    dbgprintf(DBG_DEBUG, "video filename '%s' does not match omniskore svr pattern\n", newVideo->video_file);
    return m;
  }

  char *svr_event = strndup(newVideo->video_file+matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
  char *svr_comp = strndup(newVideo->video_file+matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
  char *svr_team = strndup(newVideo->video_file+matches[3].rm_so, matches[3].rm_eo - matches[3].rm_so);
  char *svr_rnd = strndup(newVideo->video_file+matches[4].rm_so, matches[4].rm_eo - matches[4].rm_so);
  dbgprintf(DBG_SVR|DBG_INFO, "SVRZ: event=%s, comp=%s, team=%s, round=%s\n", svr_event, svr_comp, svr_team, svr_rnd);

  if (isNumeric(svr_event) != 0 || isNumeric(svr_comp) != 0) {
    dbgprintf(DBG_DEBUG, "video filename '%s' does not match omniskore numeric filename parts.\n", newVideo->video_file);
    
    free(svr_event);
    free(svr_comp);
    free(svr_rnd);
    free(svr_team);
    return 1;
  }

  pg_sdobUpdateRound(pGui, svr_rnd);
  pg_sdobUpdateTeam(pGui, svr_team);
  pg_sdobUpdateEvent(pGui, svr_event);
  pg_sdobUpdateComp(pGui, svr_comp);

  free(svr_event);
  free(svr_comp);
  free(svr_rnd);
  free(svr_team);
  return 0;
}
*/

/*
void * syncSlave(void *input) {
  // CURL Submit Scorecard to Server
  if (curl_sdob_slave_video(
      ((struct pg_sdob_device_slave_args *)input)->body,
      ((struct pg_sdob_device_slave_args *)input)->bodyLen,
      ((struct pg_sdob_device_slave_args *)input)->hostName
  ) != 0) {
    dbgprintf(DBG_DEBUG, "Unable to send video to: %s\n", ((struct pg_sdob_device_slave_args *)input)->hostName);
  };
  free(((struct pg_sdob_device_slave_args *)input)->hostName);
  free(((struct pg_sdob_device_slave_args *)input)->body);
  free((struct pg_sdob_device_slave_args *)input);
  pthread_exit(NULL);
}
*/
static void pg_skydiveorbust_clear_internal(gslc_tsGui *pGui) {
  pg_sdob_scorecard_clear(pGui);

  PG_SDOB_CLEAR_VIDEO_DATA(sdob_judgement->video);
  pg_sdobUpdateRound(pGui, (char*)" ");
  pg_sdobUpdateTeam(pGui, (char*)" ");
  pg_sdobUpdateEvent(pGui, (char*)" ");
  // pg_sdobUpdateComp(pGui, (char*)" ");
}

static void pg_skydiveorbust_parsefilename_internal(gslc_tsGui *pGui) {
  // if (pg_skydiveorbust_parse_filename_default(pGui, sdob_judgement->video) == 0) {
  //   dbgprintf(DBG_DEBUG, "Using Default Schema for filename: '%s'\n", sdob_judgement->video);
  // } else if (pg_skydiveorbust_parse_filename_omniskore(pGui, sdob_judgement->video) == 0) {
  //   dbgprintf(DBG_DEBUG, "Using Omniskore Schema for filename: '%s'\n", sdob_judgement->video);
  // } else
  if (pg_skydiveorbust_parse_filename_tammy(pGui, sdob_judgement->video) == 0) {
    dbgprintf(DBG_DEBUG, "Using Tammys Schema for filename: '%s'\n", sdob_judgement->video);
  }
}

/*
static void pg_skydiveorbust_notify_slaves(gslc_tsGui *pGui) {

  char* s = NULL;

  json_t *root = json_object();

  json_object_set_new(root, "hoster", json_integer(0));
  json_object_set_new(root, "slug", json_string(sdob_judgement->eventStr));

  json_object_set_new(root, "comp", json_string(sdob_judgement->compStr));
  json_object_set_new(root, "compId", json_string(sdob_judgement->comp));
  json_object_set_new(root, "event", json_string(sdob_judgement->eventStr));
  json_object_set_new(root, "eventId", json_string(sdob_judgement->event));
  json_object_set_new(root, "team", json_string(sdob_judgement->team));
  json_object_set_new(root, "rnd", json_string(sdob_judgement->round));

  json_object_set_new(root, "folder", json_string(sdob_judgement->video->local_folder));
  json_object_set_new(root, "file", json_string(sdob_judgement->video->video_file));
  json_object_set_new(root, "url", json_string(sdob_judgement->video->url));

  json_object_set_new(root, "es", json_string(sdob_judgement->ruleSet));

  // Dump JSON and decref
  s = json_dumps(root, 0);
  json_decref(root);

  char hostName[1024];
  hostName[1023] = '\0';
  if (gethostname(hostName, 1023) != -1) {
    strlcat(hostName, ".local", 1023);
    
    FILE * fpHosts;
    char * hostLine = NULL;
    size_t hostLen = 0;
    ssize_t readHost;

    int n = 10;
    int i = 0;
    struct pg_sdob_device_slave_args **dataSets = (struct pg_sdob_device_slave_args**)malloc(n * sizeof(struct pg_sdob_device_slave_args*));
    
    fpHosts = fopen("/home/pi/.config/sdobox/child_hosts", "r");
    if (fpHosts) {
      while ((readHost = getline(&hostLine, &hostLen, fpHosts)) != -1) {
        stripReturnCarriage(&hostLine);
        dbgprintf(DBG_DEBUG, "Sending To Slave: %s\n", hostLine);
        if (strcmp(hostName, hostLine) == 0) { continue; }

        struct pg_sdob_device_slave_args *childData = (struct pg_sdob_device_slave_args *)malloc(sizeof(struct pg_sdob_device_slave_args));
        childData->hostName = strdup(hostLine);
        childData->body = strdup(s);
        childData->bodyLen = strlen(s);
        dataSets[i] = childData;

        pthread_t tid;
        pthread_create(&tid, NULL, &syncSlave, dataSets[i]);
        i++;

      }
      fclose(fpHosts);
      if (hostLine) { free(hostLine); }
    }
  }
  free(s);

}
*/
//
// Called only from the skydiveorbust page's thread as the result of a queue action.
//
static void pg_skydiveorbust_loadvideo_internal(gslc_tsGui *pGui, struct pg_sdob_video_data *newVideo) {
  // Clear scorecard and reset interface to defaults
//  pg_sdob_clear(pGui);

  if (sdob_judgement->video->local_folder != NULL) { strlcpy(sdob_judgement->video->local_folder, newVideo->local_folder, 256); }
  if (sdob_judgement->video->video_file != NULL) { strlcpy(sdob_judgement->video->video_file, newVideo->video_file, 256); }
  if (sdob_judgement->video->url != NULL) { strlcpy(sdob_judgement->video->url, newVideo->url, 512); }

  mpv_loadfile(sdob_judgement->video->local_folder, sdob_judgement->video->video_file, "replace", "");
  mpv_fullscreen(1);
  pg_sdob_pl_sliderForceUpdate = 1;

  pg_sdobUpdateVideoDescOne(pGui, sdob_judgement->video->video_file);
}

//
// Called from anywhere (_videolist.c and main.c) to request a change
// to a new video file.  This creates the queue action responded to by
// the above function.
//
// --- ADD TO QUEUE DIRECTLY ---
//void pg_skydiveorbust_loadvideo(gslc_tsGui *pGui, struct pg_sdob_video_data *newVideo) {
//  struct queue_head *item = new_qhead();
//  item->action = E_Q_ACTION_LOADVIDEO;
//  item->data = newVideo;
//  item->u1.ptr = pGui;
//  assert(pg_sdobQueue);
//  queue_put(item, pg_sdobQueue, &pg_sdobQueueLen);
//}


void pg_sdob_scorecard_insert_mark(gslc_tsGui *pGui, int selected, double time, int mark) {
  pg_sdobInsertMark(selected, time, mark);
  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);

  // Display Point in Box
  int mSize = sdob_judgement->marks->size;
  // Update Slider Max Positions
  int sliderMax = (int)(mSize / pg_sdob_slot_max);
  if (sliderMax != pg_sdob_scroll_max) {
    pg_sdob_scroll_max = sliderMax;
  }
  // Increase line after screen is full
  if (mSize > pg_sdob_slot_max * pg_sdob_line_max && mSize % pg_sdob_slot_max == 1) {
    pg_sdobSliderSetCurPos(pGui, pg_sdob_slot_scroll + 1);
  }

  // Update Scorecard Ticks
  pg_sdobScoringMarks(pGui);

  pg_sdobUpdatePlayerSlider(pGui);
}

void pg_sdob_scorecard_update_mark(gslc_tsGui *pGui, int selected, int mark) {
  sdob_judgement->marks->arrScorecardPoints[selected] = mark;
  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);

  // Update Scorecard Ticks
  pg_sdobScoringMarks(pGui);
}


void pg_sdob_scorecard_delete_mark(gslc_tsGui *pGui, int selected) {
  pg_sdobDeleteMark(selected);
  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);

  // Update Scorecard Ticks
  pg_sdobScoringMarks(pGui);
}


void pg_sdob_scorecard_score_selected(gslc_tsGui *pGui, int selected, double amt) {
  // Updated Selected Score
  pg_sdobMoveMark(selected, amt);

  // Make sure the mark is viewable
  int markHidden = pg_sdobScoringMarkHidden(sdob_judgement->marks->selected);
  if (markHidden < 0) {
    pg_sdobSliderSetCurPos(pGui, (int)(sdob_judgement->marks->selected % pg_sdob_line_max));
  } else if (markHidden > 0) {
    pg_sdobSliderSetCurPos(pGui, (int)(sdob_judgement->marks->selected % pg_sdob_line_max) + 1);
  }

  // Update Scorecard Ticks
  pg_sdobScoringMarks(pGui);
}

void pg_sdob_scorecard_score_sopst(gslc_tsGui *pGui, double time, double prestartTime) {
  dbgprintf(DBG_DEBUG, "Set Prestart Time at %f for %f\n", time, prestartTime);
  // Set Start of Working Time
  pg_sdobSOPSTSet(time, prestartTime);
  pg_sdob_pl_sliderForceUpdate = 1;

  if (sdob_devicehost->isHost == 1) {
    /*
    fork();
    int cmdLen = snprintf(NULL, 0, "/opt/sdobox/scripts/sdob/prestart-cmd.sh -t %f -wt %f &", time, prestartTime) + 1;
    char *cmd = (char*)malloc(cmdLen);
    snprintf(cmd, cmdLen, "/opt/sdobox/scripts/sdob/prestart-cmd.sh -t %f -wt %f &", time, prestartTime);
    system(cmd);
    free(cmd);
    */
  }


}

void pg_sdob_scorecard_score_sowt(gslc_tsGui *pGui, double time, double workingTime) {
  dbgprintf(DBG_DEBUG, "Set Working Time at %f for %f\n", time, workingTime);
  // Reset Ticks to Judge Marks
  // CLEAR_PLAYER_TICKS
  // if (sdob_judgement->sowt == -1.0) {
  //  pg_sdob_player_sliderTicks(pGui, NULL, 0);
  //  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
  // }

  // Set Start of Working Time
  pg_sdobSOWTSet(time, workingTime);

  pg_sdob_pl_sliderForceUpdate = 1;
}

void pg_sdob_clear(gslc_tsGui *pGui) {
  pg_sdobUpdateTeam(pGui, "");
  pg_sdobUpdateRound(pGui, "");
  
  // Reset Scorecard Scroller
  pg_sdobSliderResetCurPos(pGui);
  if (!libmpvCache->player->is_playing) {
    pg_sdobUpdatePlayerSlider(pGui);
  }
  if (!libmpvCache->player->is_loaded) {
    pg_sdobUpdateVideoDescOne(pGui, "Click To Load Video");
  }

}

void pg_sdob_scorecard_clean(gslc_tsGui *pGui) {
  PG_SDOB_CLEAR_JUDGEMENT(sdob_judgement);
  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);

  // Reset SOWT and Video Length
  pg_sdobSOWTReset();
  // CLEAR_PLAYER_TICKS
  pg_sdob_player_sliderTicks(pGui, NULL, 0);
}

void pg_sdob_scorecard_clear(gslc_tsGui *pGui) {
  dbgprintf(DBG_DEBUG, "%s", "Clearing Scorecard\n");
  PG_SDOB_CLEAR_JUDGEMENT_META(sdob_judgement);
  PG_SDOB_CLEAR_JUDGEMENT(sdob_judgement);
  // pg_sdobUpdateScoringSettings(pGui, "fs");
  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);

  // Reset SOWT and Video Length
  pg_sdobSOWTReset();

  // Return to Chapter Marks
  //-/ pg_sdob_player_video_chapterMarks(pGui);

  // CLEAR_PLAYER_TICKS
  pg_sdob_player_sliderTicks(pGui, NULL, 0);
}


void pg_sdob_player_chaptersRefresh(gslc_tsGui *pGui) {
  // pg_sdob_player_setduration();
  //-/ pg_sdob_player_video_chapters();
  //-/ pg_sdob_player_video_chapterMarks(pGui);

  gslc_ElemSetRedraw(pGui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
}


void pg_sdob_player_sliderForceUpdate() {
  pg_sdob_pl_sliderForceUpdate = 1;
}


// GUI Thread
int pg_skydiveorbust_action_execute(struct queue_head *item) {
  int64_t sClock;

  // Fetch Items to deal with in queue
  if (item) {
    switch (item->action) {
      // Buttons, Knob, Keyboard actions
      case E_Q_SCORECARD_KEY:
        // if (item->key != NULL) { keyboard_proxy(item->key); }
      break;

      // Insert Mark
      case E_Q_SCORECARD_INSERT_MARK:
        pg_sdob_scorecard_insert_mark(&m_gui, item->selected, item->time, item->mark);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
        pg_sdobSubmitAction();
      break;


      // Update Mark
      case E_Q_SCORECARD_UPDATE_MARK:
        pg_sdob_scorecard_update_mark(&m_gui, item->selected, item->mark);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
        pg_sdobSubmitAction();
      break;

      // Delete Mark
      case E_Q_SCORECARD_DELETE_MARK:
        pg_sdob_scorecard_delete_mark(&m_gui, item->selected);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
        pg_sdobSubmitAction();
      break;

      case E_Q_SCORECARD_MOVE_SCORE_SELECTED:
        pg_sdob_scorecard_score_selected(&m_gui, item->selected, item->amt);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
      break;

      case E_Q_SCORECARD_SCORING_SOPST:
        pg_sdob_scorecard_score_sopst(&m_gui, item->time, sdob_judgement->prestartTime);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
        pg_sdobSubmitAction();
      break;

      case E_Q_SCORECARD_SCORING_SOWT:
        pg_sdob_scorecard_score_sowt(&m_gui, item->time, sdob_judgement->workingTime);

        if (sdob_current_rounds && sdob_num_current_rounds > 1
            && (!strcmp(sdob_judgement->team, "") || !strcmp(sdob_judgement->round, ""))
        ) {
          mpv_pause();
          touchscreenPageOpen(&m_gui, E_PG_SDOB_ROUNDLIST);
        }
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
        pg_sdobSubmitAction();
      break;

      case E_Q_SCORECARD_SUBMIT_SCORECARD:
        // Submit scorecard to syslog
        // touchscreenPopupMsgBox(&m_gui, "Submitting Score!", "Saving your score.");
        pg_sdobSubmitScorecard();
        // touchscreenPopupMsgBoxClose(&m_gui);
      break;
      case E_Q_SDOB_CLEAR:
        pg_sdob_clear(&m_gui);
      case E_Q_SCORECARD_CLEAR:
        pg_sdob_scorecard_clear(&m_gui);
        pg_sdobSliderSetCurPos(&m_gui, 0);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
      break;

      case E_Q_SCORECARD_CLEAN:
        pg_sdob_scorecard_clean(&m_gui);
        pg_sdobSliderSetCurPos(&m_gui, 0);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_PL_SLIDER], GSLC_REDRAW_FULL);
        gslc_ElemSetRedraw(&m_gui, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
        pg_sdobSubmitAction();
      break;

      case E_Q_PLAYER_SLIDER_CHAPTERS:
        pg_sdob_player_chaptersRefresh(&m_gui);
      break;

      case E_Q_PLAYER_CHAPTER_CHANGED:
        // pg_sdob_player_setChapterCur();
      break;

      case E_Q_PLAYER_VIDEO_INFO:
        //-/ pg_sdob_player_videoInfo(&m_gui);
      break;

      // Writes commands to open FD Socket
      case E_Q_ACTION_MPV_COMMAND:
        libmpv_zmq_cmd(item->cmd);
      break;



      case E_Q_PLAYER_VIDEO_STOP:
        mpv_stop();
      break;

      case E_Q_ACTION_VIDEO_RATE:
        pg_sdobUpdateVideoRate(&m_gui, mpv_speed(item->amt));
      break;

      case E_Q_ACTION_VIDEO_RATE_ADJUST:
        pg_sdobUpdateVideoRate(&m_gui, mpv_speed_adjust(item->amt));
      break;

      case E_Q_ACTION_VIDEO_RATE_USER:
        pg_sdobUpdateUserDefinedVideoRate(&m_gui, libmpvCache->player->pbrate);
      break;

      case E_Q_ACTION_LOADVIDEO:
        pg_skydiveorbust_clear_internal((gslc_tsGui*)item->u1.ptr);
        pg_skydiveorbust_loadvideo_internal
          ((gslc_tsGui*)item->u1.ptr, (struct pg_sdob_video_data*)item->data);
        PG_SDOB_FREE_VIDEO_DATA((struct pg_sdob_video_data*)item->data);
        free((struct pg_sdob_video_data*)item->data);
        gslc_ElemSetRedraw((gslc_tsGui*)item->u1.ptr, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
      break;

      case E_Q_ACTION_LOADURL:
        // printf("Action: %s\n", ((struct pg_sdob_video_data*)item->data)->url);
        if (((struct pg_sdob_video_data*)item->data)->url != NULL) {
          strlcpy(sdob_judgement->video->url, ((struct pg_sdob_video_data*)item->data)->url, 512);
          mpv_loadurl(sdob_judgement->video->url, "replace", "");
        }

        PG_SDOB_FREE_VIDEO_DATA((struct pg_sdob_video_data*)item->data);
        free((struct pg_sdob_video_data*)item->data);
        gslc_ElemSetRedraw((gslc_tsGui*)item->u1.ptr, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
      break;

      case E_Q_ACTION_PARSE_VIDEO_FILENAME:
        pg_skydiveorbust_parsefilename_internal((gslc_tsGui*)item->u1.ptr);
        gslc_ElemSetRedraw((gslc_tsGui*)item->u1.ptr, pg_sdobEl[E_SDOB_EL_BOX], GSLC_REDRAW_FULL);
      break;

      case E_Q_ACTION_NOTIFY_SLAVES:
        // pg_skydiveorbust_notify_slaves((gslc_tsGui*)item->u1.ptr);
      break;

      case E_Q_PLAYER_VIDEO_PAUSE:
        mpv_pause();
      break;
      case E_Q_PLAYER_VIDEO_PLAY:
        mpv_play();
      break;
      case E_Q_PLAYER_VIDEO_TOGGLE:
        mpv_playpause_toggle();
      break;
      case E_Q_PLAYER_VIDEO_SETPAUSE:
        if (item->mark == 1) { mpv_play();
        } else { mpv_pause(); }
      break;
      case E_Q_PLAYER_VIDEO_CHECKPAUSE:
        mpv_check_pause();
      break;
      case E_Q_PLAYER_VIDEO_BACKWARD:
        mpv_seek(-2);
      break;
      case E_Q_PLAYER_VIDEO_FORWARD:
        mpv_seek(2);
      break;
      case E_Q_PLAYER_VIDEO_FORWARD_STEP:
        // sClock = s_clock();
        // if ((sClock - pg_sdob_framestep_debounce_delay) > 750) {
          libmpv_zmq_cmd(strdup("frame-step"));
        //   pg_sdob_framestep_debounce_delay = sClock;
        // }
      break;

      case E_Q_PLAYER_SLIDER_UPDATE:
        pg_sdob_player_sliderForceUpdate();
      break;

      case E_Q_SDOB_CLEAR_SELECTING:
        // Stop Changing Video Rate
        pg_sdobPlayerVideoRateChanging(&m_gui, 2);

        // Stop Clearing Scorecard Selection
        if (sdob_judgement->marks->selected > 0) {
          pg_sdobScoringSelectionClear(&m_gui);
        }
      break;

    default:
      dbgprintf(DBG_ERROR, "Undetected item action: %d\n", item->action);
        abort();
      break;
    }

    if (item->cb != NULL) {
      item->cb(item->cbarg);
    }

    free(item);
    return 1;
  }
  return 0;
}

// from main thread loop, return 1 if any work is done
int pg_skydiveorbust_thread(gslc_tsGui *pGui) {
  ////////////////////////
  // Player Slider Moved
  if (pg_sdob_player_move > -1
    // && (s_clock() - pg_sdob_player_move_debounce) > 750
  ) {
    // Execute Slider Move
    if (sdob_judgement->sowt == -1.0 || !pg_sdob_timeline_zoom_workingtime) {
      mpv_seek_arg(pg_sdob_player_move, "absolute-percent+exact");
    } else {
      mpv_seek_arg((sdob_judgement->sowt + (sdob_judgement->workingTime * (pg_sdob_player_move * .01))), "percent");
    }
    // pg_sdob_player_move_debounce = s_clock();
    pg_sdob_player_move = -1;
    return 1;
  }
  return 0;
}

/*
  // SDOB Video Host Event
  // Incoming Request from sdobox.socket or API socket threads
  // Using reference counter to try to keep threads cleaner
  if (libUlfiusSDOBNewVideoInfo->cnt > 0 && pg_sdob_new_video_cnt != libUlfiusSDOBNewVideoInfo->cnt) {
    // Update counter to sync with current
    libUlfiusSDOBNewVideoInfo->cnt = pg_sdob_new_video_cnt;

    dbgprintf(DBG_DEBUG, "%s", "New Video Requested From Socket\n");

    // if (m_tPageCur != E_PG_SKYDIVEORBUST) {
    //   touchscreenPageOpen(&m_gui, E_PG_SKYDIVEORBUST);
    // }
    
    // pg_sdob_clear(&m_gui);
    // pg_sdob_scorecard_clear(&m_gui);
    if (sdob_judgement->marks->size > 0) {
      dbgprintf(DBG_DEBUG, "%s", "Currently Scoring, Rejecting New Video Request.\n");
      return 1;
    }

    // Setup Environment for new Video API
    if (strcmp(libUlfiusSDOBNewVideoInfo->host, "0") == 0) {
      sdob_devicehost->isHost = 0;
    } else {
      sdob_devicehost->isHost = 1;
    }
    pg_sdobUpdateHostDeviceInfo(&m_gui);
    
    
    if (sdob_devicehost->isHost == 1 && libUlfiusSDOBNewVideoInfo->url[0] != '\0') {
      mpv_loadfile(NULL, libUlfiusSDOBNewVideoInfo->url, "replace", "");
      mpv_fullscreen(1);
    } else if (sdob_devicehost->isHost == 1 &&
                libUlfiusSDOBNewVideoInfo->local_folder[0] != '\0' &&
                libUlfiusSDOBNewVideoInfo->video_file[0] != '\0'
    ) {
      mpv_loadfile(libUlfiusSDOBNewVideoInfo->local_folder, libUlfiusSDOBNewVideoInfo->video_file, "replace", "");
      mpv_fullscreen(1);
    }
    

    if (libUlfiusSDOBNewVideoInfo->team[0] != '\0') {
      pg_sdobUpdateTeam(&m_gui, libUlfiusSDOBNewVideoInfo->team);
    }
    if (libUlfiusSDOBNewVideoInfo->rnd[0] != '\0') {
      pg_sdobUpdateRound(&m_gui, libUlfiusSDOBNewVideoInfo->rnd);
    }

    if (libUlfiusSDOBNewVideoInfo->video_file[0] != '\0') {
      pg_sdobUpdateVideoDescOne(&m_gui, libUlfiusSDOBNewVideoInfo->video_file);
    } else if (libUlfiusSDOBNewVideoInfo->eventStr[0] != '\0') {
      pg_sdobUpdateVideoDescOne(&m_gui, libUlfiusSDOBNewVideoInfo->eventStr);
    } else {
      pg_sdobUpdateVideoDescOne(&m_gui, (char*)" ");
    }
    if (libUlfiusSDOBNewVideoInfo->compStr[0] != '\0') {
      pg_sdobUpdateVideoDescTwo(&m_gui, libUlfiusSDOBNewVideoInfo->compStr);
      // pg_sdobUpdateComp(&m_gui, libUlfiusSDOBNewVideoInfo->compStr);
    }
    if (libUlfiusSDOBNewVideoInfo->es[0] != '\0') {
      pg_sdobUpdateScoringSettings(&m_gui, libUlfiusSDOBNewVideoInfo->es);
    }


    // pg_sdobUpdateEventFromLocalFolder(&m_gui, libUlfiusSDOBNewVideoInfo->meetStr);
    // pg_sdobUpdateComp(&m_gui, libUlfiusSDOBNewVideoInfo->compId, libUlfiusSDOBNewVideoInfo->comp);
    // pg_sdobUpdateVideoDescTwo(&m_gui, libUlfiusSDOBNewVideoInfo->desc);
    // pg_sdobUpdateTeam(&m_gui, libUlfiusSDOBNewVideoInfo->team);
    // pg_sdobUpdateRound(&m_gui, libUlfiusSDOBNewVideoInfo->rnd);
    

    // if (sdob_devicehost->isHost == 1) {
    //   mpv_loadfile(libUlfiusSDOBNewVideoInfo->folder, libUlfiusSDOBNewVideoInfo->file, "replace", "fullscreen=yes");
    // }

  }
  */

  /***gslc_Update(&m_gui);*/
//   return 0;
// }





// ------------------------
// MPV Output SDOB Thread Thread
// ------------------------
void * pg_sdobThread(void *input)
{
  if (pg_sdobThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting SDOB Scoring Queue Thread, Already Started");
    return NULL;
  }
  pg_sdobThreadRunning = 1;

  if (pg_sdobThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not SDOB Scoring Queue Thread, Stop Flag Set");
    pg_sdobThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_DEBUG, "%s\n", "Starting SDOB Scoring Queue Thread");

  void *sdobsub = zmq_socket(libzhelpers_context(), ZMQ_SUB);
  zmq_connect(sdobsub, "inproc://sdobworker");
  const char *filter = "";
  int rc = zmq_setsockopt (sdobsub, ZMQ_SUBSCRIBE, filter, strlen(filter));
  if (rc != 0) {
    dbgprintf(DBG_ERROR, "%s\n", "SDOB Scoring Queue Thread Cannot Set Socket Options");
  }
  zmq_pollitem_t items [] = {
    { sdobsub, 0, ZMQ_POLLIN, 0 }
  };
  
  while (!pg_sdobThreadKill) {
    zmq_poll (items, 1, -1);
    if (!m_bQuit) {  
      if (items[0].revents & ZMQ_POLLIN) {
        struct queue_head *head;
        int size = zmq_recv (sdobsub, &head, sizeof(&head), 0);
        if (size > -1) {
          pg_skydiveorbust_action_execute(head);
        }
      }
    }
  }

  zmq_close(sdobsub);
  dbgprintf(DBG_DEBUG, "%s\n", "Closing SDOB Scoring Queue Thread");
  pg_sdobThreadRunning = 0;
  pthread_exit(NULL);
}


int pg_sdobThreadStart() {
  dbgprintf(DBG_DEBUG, "%s\n", "pg_sdobThreadStart()");
  if (pg_sdobThreadRunning) { return 0; }

  pg_sdob_api_newVideoCnt = 0;

  // printf("SkydiveOrBust MPV TimePos Thread Spinup: %d\n", pg_sdobThreadRunning);
  pg_sdobThreadKill = 0;
  pthread_t tid;
  return pthread_create(&tid, NULL, &pg_sdobThread, NULL);
}

void pg_sdobThreadStop() {
  dbgprintf(DBG_DEBUG, "%s\n", "pg_sdobThreadStop()");
  // Shutdown MPV FIFO Thread

  int shutdown_cnt = 0;
  if (pg_sdobThreadRunning) {
    pg_sdobThreadKill = 1;
    while (pg_sdobThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
  }
  dbgprintf(DBG_DEBUG, "pg_sdobThread Shutdown %d\n", shutdown_cnt);
}





int pg_sdob_MpgEventCbId = -1;
void pg_sdobMpvEventCb(char* event) {
  // printf("Yay Event: %s\n", event);
  /*
  if (strcmp(event, "file-loaded") == 0) {
    mpv_any_u* retPath;
    if ((mpvSocketSinglet("path", &retPath)) != -1) {
      printf("File Path: %s\n", (char *)retPath->ptr);
      free(retPath->ptr);
      MPV_ANY_U_FREE(retPath);
    }
  }
  */
}


/////////////////////////////////////////////
// Initialization
//

// GUI init
void pg_skydiveorbust_init(gslc_tsGui *pGui) {
  dbgprintf(DBG_DEBUG, "%s\n", "Page SkydiveOrBust Init");

  // Clear function pointer, indicate it's been run
  cbInit[E_PG_SKYDIVEORBUST] = NULL;

  // Dynamically Allocate Page Elements
  pg_sdobElTotal = E_SDOB_EL_MAX + 150;
  pg_sdobElem = (gslc_tsElem *)malloc(pg_sdobElTotal * sizeof(gslc_tsElem));
  // pg_sdobElemRef = (gslc_tsElemRef*)calloc(pg_sdobElTotal, sizeof(gslc_tsElemRef));
  pg_sdobEl = (gslc_tsElemRef **)malloc(pg_sdobElTotal * sizeof(gslc_tsElemRef*));

  //////////////////////////////
  // Host Device Initalizer
  sdob_devicehost = PG_SDOB_DEVICE_HOST();

  //////////////////////////////
  // Queue Initializer
  // Connect to SDOB Worker Queue
  pg_sdobWorker = zmq_socket(libzhelpers_context(), ZMQ_PUB);
  int rc = zmq_bind(pg_sdobWorker, "inproc://sdobworker");
  if (rc != 0) {
    printf("Cannot Start Inhome Proc: %d\n", rc);
  }

  pg_sdobQueue = ALLOC_QUEUE_ROOT();
  pg_sdobQueueLen = 0;
  pg_sdobMpvSocketThreadKill = 0; // Stopping SDOB Thread
  pg_sdobMpvSocketThreadRunning = 0; // Running flag for SDOB Thread
  pg_sdobThreadKill = 0; // Stopping SDOB Thread
  pg_sdobThreadRunning = 0; // Running flag for SDOB Thread
  



  ///////////////////
  // MPV Initializer
  // mpv_init(pGui);
  pg_sdob_framestep_debounce_delay = 0;

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

  sdob_current_rounds = NULL;
  sdob_num_current_rounds = 0;

  // GUI Init
  gslc_tsRect rScores = {0,110,440,130};
  pg_sdobGuiInit(pGui, rScores);

  // Judge Initials
  pg_sdobUpdateJudgeInitials(pGui, "");

  // Team
  pg_sdobUpdateTeam(pGui, "");

  // Round
  pg_sdobUpdateRound(pGui, "");

  // Video Desc
  pg_sdobUpdateVideoDescTwo(pGui, "");

  pg_sdobUpdateCount(pGui, pg_sdobEl[E_SDOB_EL_SC_COUNT]);



  //////////////
  // Player Info
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
  pg_sdobUpdateVideoRate(pGui, libmpvCache->player->pbrate);
  pg_sdobUpdateUserDefinedVideoRate(pGui, pg_sdob_player_playback_slow);

  // pg_skydiveorbust_loadvideo(pGui, "/home/pi/Videos", "XP.mpg");


  pg_sdob_new_video_cnt = 0;

  pg_sdobUpdateHostDeviceInfo(pGui);
  pg_sdobUpdateScoringSettings(pGui, "fs");


  ////////////////////////////
  // Start SDOB Thread
  // dbgprintf(DBG_DEBUG, "%s\n", "Page SkydiveOrBust Stopping MPV TimePos Thread");
  pg_sdobMpvTimeposThreadStart(); // Processes Timestamps in a human way
  pg_sdobMpvEventsThreadStart(); // Fetch events from video server :5555
  // dbgprintf(DBG_DEBUG, "%s\n", "Page SkydiveOrBust Starting Thread");
  pg_sdobThreadStart(); // Processes QUEUE Events


  // pg_sdob_MpgEventCbId = libMpvCallbackAppend(&pg_sdobMpvEventCb);

  pg_sdob_clear(pGui);

  // Remove MPV Player Volume
  // printf("Send Mute\n");
  mpv_volume_mute();

  //////////////////////////////
  // Finish up

}


// GUI Open
void pg_skydiveorbust_open(gslc_tsGui *pGui) {

  dbgprintf(DBG_DEBUG, "%s\n", "Page SkydiveOrBust Setting Button Functions");
  pg_skydiveorbustButtonSetFuncs();
  
  sdob_devicehost->isHost = 0;

  // Fetch Info
  int rc;
  libmpvCache->player->duration = libmpv_zmq_get_prop_double("duration");

  char *propFilename = libmpv_zmq_get_prop_string("filename");
  if (propFilename != NULL) {
    strlcpy(libmpvCache->player->file, propFilename, 512);
    free(propFilename);
  }
  if (libmpvCache->player->file != NULL) {
    libmpvCache->player->is_loaded = 1;
  } else {
    libmpvCache->player->is_loaded = 0;
  }

  pg_sdobSubmitAction();
  // Reset Scorecard Slider to Top
  // pg_sdobSliderResetCurPos(pGui);

  // pg_sdobSqlSetup();
  // pg_sdobSql_markInsert(1, 1, "34.4", "35.0");

  dbgprintf(DBG_DEBUG, "%s\n", "Page SkydiveOrBust Started");
}


// GUI Close
void pg_skydiveorbust_close(gslc_tsGui *pGui) {
  
}

void pg_skydiveorbust_judgement_free(void) {
  // Free Judgement Info
  PG_SDOB_FREE_VIDEO_DATA(sdob_judgement->video);
  free(sdob_judgement->video);

  free(sdob_judgement->ruleSet);
  free(sdob_judgement->judge);
  free(sdob_judgement->event);
  free(sdob_judgement->eventStr);
  free(sdob_judgement->comp);
  free(sdob_judgement->compStr);
  free(sdob_judgement->team);
  free(sdob_judgement->teamStr);
  free(sdob_judgement->round);
  free(sdob_judgement->roundStr);
  free(sdob_judgement->scoreStr);

  free(sdob_judgement->marks->arrScorecardPoints);
  free(sdob_judgement->marks->arrScorecardTimes);
  free(sdob_judgement->marks->arrScorecardTicks);

  free(sdob_judgement->marks);
  free(sdob_judgement);
}

// GUI Destroy
void pg_skydiveorbust_destroy(gslc_tsGui *pGui) {
  printf("Destroying SDOB\n");
////////////////////////////
  // Stop SDOB Thread
  // dbgprintf(DBG_DEBUG, "%s\n", "Page SkydiveOrBust Stopping Thread");
  pg_sdobThreadStop();
  // dbgprintf(DBG_DEBUG, "%s\n", "Page SkydiveOrBust Stopping MPV TimePos Thread");
  pg_sdobMpvTimeposThreadStop();
  pg_sdobMpvEventsThreadStop();

  pg_skydiveorbust_judgement_free();
  
  destroy_sdob_video_rounds();

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

  zmq_close(pg_sdobWorker);
  // if (pg_sdob_MpgEventCbId > -1) {
  //   libMpvCallbackRemove(pg_sdob_MpgEventCbId);
  // }
  // free(sdob_devicehost);
  // printf("Page SkydiveOrBust Destroyed\n");

  // mpv_volume_on();
}


void __attribute__ ((constructor)) pg_skydiveorbust_constructor(void) {
  cbInit[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_init;
  cbOpen[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_open;
  cbThread[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_thread;
  cbClose[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_close;
  cbDestroy[E_PG_SKYDIVEORBUST] = &pg_skydiveorbust_destroy;
  init_svr_regexp();
}

void __attribute__ ((destructor)) pg_skydiveorbust_destructor(void) {
  destroy_svr_regexp();
}

