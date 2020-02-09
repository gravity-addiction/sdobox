#ifndef _TOUCHAPP_SDOB_H_
#define _TOUCHAPP_SDOB_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdbool.h>
#include "gui/pages.h"
#include "skydiveorbust_scorecard.h"
#include "skydiveorbust_player.h"


//////////////////
// GUI Page
enum {
  E_SDOB_CLEAN,
  E_SDOB_EL_SC_SLIDER,
  E_SDOB_EL_SC_SLIDER_UP,
  E_SDOB_EL_SC_SLIDER_DOWN,
  E_SDOB_EL_BOX,
  E_SDOB_EL_SC_COUNT,
  E_SDOB_EL_JUDGE_INITIALS,
  E_SDOB_EL_TEAM_DESC,
  E_SDOB_EL_ROUND_DESC,
  E_SDOB_EL_MEET_DESC,
  E_SDOB_EL_VIDEO_DESC,
  E_SDOB_EL_BTN_SUBMIT,

  E_SDOB_EL_PL_PAUSE,
  E_SDOB_EL_PL_USER_RATE,
  E_SDOB_EL_PL_BACK,
  E_SDOB_EL_PL_PLAY,
  E_SDOB_EL_PL_FORWARD,
  E_SDOB_EL_PL_SLIDER,
  E_SDOB_EL_PL_POS,
  E_SDOB_EL_PL_RATE,

  E_SDOB_EL_MAX
};

#define MAX_ELEM_PG_SDOB      E_SDOB_EL_MAX + 100
#define MAX_ELEM_PG_SDOB_RAM  MAX_ELEM_PG_SDOB

gslc_tsElem pg_sdobElem[MAX_ELEM_PG_SDOB];
gslc_tsElemRef pg_sdobElemRef[MAX_ELEM_PG_SDOB_RAM];

gslc_tsElemRef* pg_sdobEl[E_SDOB_EL_MAX];


/////////////////
// Scorecard
enum {
  E_SCORES_BUST,
  E_SCORES_POINT,
  E_SCORES_OMISSION,
  E_SCORES_SOWT,
  MAX_SCORE_MARKS
};
struct pg_sdob_scorecard_marks {
  int          max;
  int          last;
  int          tickCnt;
  double       *arrScorecardTicks;
  double       *arrScorecardTimes;
  int          *arrScorecardId;

  int selected;
  int size;
  int *arrScorecardPoints;
  int *arrScorecardMilli;
};

struct pg_sdob_judgement_data {
  double sowt;
  double workingTime;
  double score;
  double scoreMax;

  char*  judge; // judge initials
  char*  video_file; // video filename in meet folder
  char*  meet; // meet folder
  char*  team;
  char*  teamStr;
  char*  round;
  char*  roundStr;
  char*  scoreStr;

  struct pg_sdob_scorecard_marks* marks; // skydiveorbust_gui-scorecard.c
};
struct pg_sdob_judgement_data *sdob_judgement;

struct pg_sdob_scorecard_marks * PG_SDOB_SCORECARD_INIT_MARKS();
void PG_SDOB_SCORECARD_CLEAR_MARKS(struct pg_sdob_scorecard_marks *sc);
struct pg_sdob_judgement_data * PG_SDOB_INIT_JUDGEMENT();
void PG_SDOB_CLEAR_JUDGEMENT(struct pg_sdob_judgement_data *judgement);


// Scoring
gslc_tsXSlider pg_sdob_sc_slider;

// int       m_scorecard_is_submitting;
// double    sc_sowt;

// int       i_slot_start; // first slot to show on scorecard, all other slots created from this index
int pg_sdob_line_max; // maximum number of lines to display on scorecard, each line is points and results
int pg_sdob_slot_max; // maximum number of slots per line
int pg_sdob_scroll_max; // Initial number of positions on scrollcard scrollbar;
int pg_sdob_slot_scroll; // position of vertical scrollbar
int pg_sdob_score_count; // Number of points to display
int pg_sdob_scorecard_max; // Initial array resize for storeing marks, currently no realloc functionality
int pg_sdob_slider_debounce_delay;

int pg_sdob_scorecard_slotid[30];
gslc_tsElemRef *pg_sdob_scorecard_elemsNum[30];
gslc_tsElemRef *pg_sdob_scorecard_elemsMark[30];


bool pg_sdobScorecardDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);


////////////////////
// Files & Folders
struct pg_sdob_videolist_files {
  int          max; // Max files in list
  int          cur; // Currently select file
  int          size; // Current size of file list
  int          len; // Max File name length
  char         **list; // array of pointer for file names
};
struct pg_sdob_videolist_files *sdob_files;
struct pg_sdob_videolist_files * PG_SDOB_INIT_VIDEOLIST_FILES();

struct pg_sdob_videolist_folders {
  int          max; // Max folders in list
  int          cur; // Currently select folder
  int          size; // Current size of folder list
  int          len; // Max Foldername Length
  char         **list; // array of pointers for folder names
};
struct pg_sdob_videolist_folders *sdob_folders;
struct pg_sdob_videolist_folders * PG_SDOB_INIT_VIDEOLIST_FOLDERS();


////////////////////
// MPV Player
struct pg_sdob_player_data {
  int paused;
  double position;
  double duration;
  double pbrate;
  double pbrateUser;
  char *positionStr;
  char *pbrateStr;
  char *pbrateUserStr;
};
struct pg_sdob_player_data *sdob_player;

struct pg_sdob_player_chapters {
  int len;
  int max;
  int cur;
  double *ptr;
};
struct pg_sdob_player_chapters *sdob_chapters;

struct pg_sdob_player_ticks {
  int len;
  int max;
  int lock;
  double *ptr;
};
struct pg_sdob_player_ticks *sdob_player_ticks;

int pg_sdob_player_mpv_fd;

gslc_tsXSlider pg_sdob_pl_slider;
int pg_sdob_pl_sliderForceUpdate;
double pg_sdob_player_playback_slow_default; // Set by configuration, defaults to .75 (75%)
double pg_sdob_player_playback_slow; // Used playback setting
int pg_sdob_change_video_rate;

int pg_sdob_player_move_debounce;
double pg_sdob_player_move;
int pg_sdob_player_move_timepos;
int pg_sdob_player_move_timepos_lock;
int pg_sdob_player_mpv_fd_timer;

struct pg_sdob_player_data * PG_SDOB_INIT_PLAYER();
struct pg_sdob_player_chapters * PG_SDOB_INIT_PLAYER_CHAPTERS();
struct pg_sdob_player_ticks * PG_SDOB_INIT_PLAYER_TICKS();
struct pg_sdob_video_data * PG_SDOB_INIT_VIDEO();

void pg_sdobPlayerVideoRateChanging(gslc_tsGui *pGui, int changable);


////////////////
// Thread
int pg_sdobMpvTimeposThreadKill;
int pg_sdobMpvTimeposThreadRunning;

int pg_sdobMpvSocketThreadKill;
int pg_sdobMpvSocketThreadRunning;

int pg_sdobThreadKill;
int pg_sdobThreadRunning;











/////////////////////
// Queue
enum {
  E_Q_SCORECARD_NONE,
  E_Q_SCORECARD_KEY,
  E_Q_SCORECARD_ADD_MARK,
  E_Q_SCORECARD_INSERT_MARK,
  E_Q_SCORECARD_UPDATE_MARK,
  E_Q_SCORECARD_DELETE_MARK,
  E_Q_SCORECARD_MOVE_SCORE_SELECTED,
  E_Q_SCORECARD_SCORING_SOWT,
  E_Q_SCORECARD_SUBMIT_SCORECARD,
  E_Q_SCORECARD_CLEAR,

  E_Q_PLAYER_SLIDER_CHAPTERS,
  E_Q_PLAYER_VIDEO_INFO,
  E_Q_PLAYER_VIDEO_PAUSE,
  E_Q_PLAYER_VIDEO_UNPAUSE,
  E_Q_PLAYER_VIDEO_SETPAUSE,
  E_Q_PLAYER_SLIDER_UPDATE,
  E_Q_PLAYER_CHAPTER_CHANGED,

  E_Q_ACTION_PLAY_PAUSE,
  E_Q_ACTION_VIDEO_RATE,
  E_Q_ACTION_VIDEO_RATE_ADJUST,
  E_Q_ACTION_VIDEO_RATE_USER,
  E_Q_ACTION_MPV_COMMAND,
  E_Q_MAX_QUEUE_TYPES
};

struct queue_root *pg_sdobQueue;
size_t pg_sdobQueueLen;



void pg_sdobSliderSetCurPos(gslc_tsGui *pGui, int slot_scroll);
void pg_sdobSliderChangeCurPos(gslc_tsGui *pGui, int amt, bool redraw);
void pg_sdobSliderResetCurPos(gslc_tsGui *pGui);
void pg_sdobUpdateCount(gslc_tsGui *pGui, gslc_tsElemRef *pElem);

void pg_sdobUpdateVideoRate(gslc_tsGui *pGui, double playback_rate);
void pg_sdobUpdateUserDefinedVideoRate(gslc_tsGui *pGui, double playback_rate);
void pg_sdobUpdateJudgeInitials(gslc_tsGui *pGui, char *str);
void pg_sdobUpdateMeet(gslc_tsGui *pGui, char *str);
void pg_sdobUpdateVideoDesc(gslc_tsGui *pGui, char *str);
void pg_sdobUpdateTeam(gslc_tsGui *pGui, char *str);
void pg_sdobUpdateRound(gslc_tsGui *pGui, char *str);






void pg_skydiveorbustGuiInit(gslc_tsGui *pGui, gslc_tsRect pRect);
void pg_skydiveorbustGuiRedraw(gslc_tsGui *pGui);


void pg_skydiveorbustButtonRotaryCW();
void pg_skydiveorbustButtonRotaryCCW();
void pg_skydiveorbustButtonLeftPressed();
void pg_skydiveorbustButtonRightPressed();
void pg_skydiveorbustButtonRotaryPressed();
void pg_skydiveorbustButtonLeftHeld();
void pg_skydiveorbustButtonRightHeld();
void pg_skydiveorbustButtonRotaryHeld();
void pg_skydiveorbustButtonSetFuncs();
void pg_skydiveorbustButtonUnsetFuncs();

void pg_sdob_mpv_timepos_thread();
int pg_sdobThreadStart();
void pg_sdobThreadStop();

void pg_skydiveorbust_init(gslc_tsGui *pGui);
void pg_skydiveorbust_open(gslc_tsGui *pGui);


void pg_sdob_scorecard_insert_mark(gslc_tsGui *pGui, int selected, double time, int mark);
void pg_sdob_scorecard_update_mark(gslc_tsGui *pGui, int selected, int mark);
void pg_sdob_scorecard_delete_mark(gslc_tsGui *pGui, int selected);

void pg_sdob_scorecard_score_selected(gslc_tsGui *pGui, int selected, double amt);
void pg_sdob_scorecard_score_sowt(gslc_tsGui *pGui, double time);
void pg_sdob_scorecard_clear(gslc_tsGui *pGui);
void pg_sdob_player_chaptersRefresh(gslc_tsGui *pGui);
void pg_sdob_player_sliderForceUpdate();

int pg_skydiveorbust_thread(gslc_tsGui *pGui);
void pg_skydiveorbust_destroy(gslc_tsGui *pGui);



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _TOUCHAPP_SDOB_H_