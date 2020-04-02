#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "libs/GUIslice-wrapper/GUIslice-wrapper.h"
#include "vlisting.h"

///////////////////////////////
// Slider Config Management

struct vlist_config * VLIST_INIT_CONFIG(int per, int max) {
  struct vlist_config *config = (struct vlist_config*)malloc(sizeof(struct vlist_config));

  config->max = max;
  config->len = 0;
  config->cur = -1;
  config->per = per;
  config->scrollMax = 1;
  config->scroll = 0;
  config->refs = (gslc_tsElemRef**)malloc(config->per * sizeof(struct gslc_tsElemRef*));

  return config;
}

void VLIST_UPDATE_CONFIG(struct vlist_config *config) {
  config->scrollMax = ceil(config->len / config->per);
  if (config->scrollMax < 1) { config->scrollMax = 1; }
}

// Clear Judgement Data and Scorecard Marks
void VLIST_CLEAR_CONFIG(struct vlist_config *config)
{
  config->len = 0;
  config->cur = -1;
  config->scrollMax = 1;
  config->scroll = 0;
}


///////////////////////////////
// Slider Config Updates

// Slider Down Clicked
void vlist_sliderDown(struct vlist_config *config) {
  config->scroll += 1;
  if (config->scroll > config->scrollMax) {
    config->scroll = config->scrollMax - 1;
  }
  if (config->scroll < 0) {
    config->scroll = 0;
  }
}

// Slider Up Clicked
void vlist_sliderUp(struct vlist_config *config) {
  config->scroll -= 1;
  if (config->scroll < 0) {
    config->scroll = 0;
  }
}

// Setting Current Scroll Position
void vlist_sliderSetPos(gslc_tsGui *pGui, struct vlist_config *config, int i) {
  if (i < 0) { i = config->scrollMax; }
  else if (i > config->scrollMax) { i = 0; }
  config->scroll = i;
}

void vlist_sliderSetPosFromCur(gslc_tsGui *pGui, struct vlist_config *config) {
  int sR = config->cur % config->per;
  int i = (config->cur - sR) / config->per;
  vlist_sliderSetPos(pGui, config, i);

  int i_slot_old = gslc_ElemXSliderGetPos(pGui, config->sliderEl);
  if (i_slot_old != config->scroll) {
    vlist_sliderChangeCurPos(pGui, config, (config->scroll - i_slot_old));
  }
}

// List item clicked, make it current selected
int vlist_clickBtn(struct vlist_config *config, int i) {
  int newI = i + (config->scroll * config->per);
  if (newI >= config->len) { return 0; }
  config->cur = newI;
  return 1;
}

void vlist_next(struct vlist_config *config) {
  int newI = config->cur + 1;
  if (newI >= config->len) { newI = 0; }
  config->cur = newI;
}

void vlist_prev(struct vlist_config *config) {
  int newI = config->cur - 1;
  if (newI < 0) { newI = config->len - 1; }
  if (newI < 0) { newI = 0; }
  config->cur = newI;
}




///////////////////////////////
// Slider Display Updates

void vlist_sliderUpdate(gslc_tsGui *pGui, struct vlist_config *config) {
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, config->sliderEl);
  gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);
  pSlider->nPosMax = config->scrollMax;
}

// Updated Scorecard Slider Position
void vlist_sliderChangeCurPos(gslc_tsGui *pGui, struct vlist_config *config, int amt) {
  // Save Current Slider POS as i_slot_old
  int i_slot_old = gslc_ElemXSliderGetPos(pGui, config->sliderEl);
  // Set Slider Pos
  vlist_sliderSetPos(pGui, config, (i_slot_old + amt));
  gslc_ElemXSliderSetPos(pGui, config->sliderEl, config->scroll);

  // Up scroller indicator
  if (config->scroll == 0) {
    gslc_ElemSetTxtCol(pGui, config->sliderUpEl, GSLC_COL_GRAY_LT1);
    gslc_ElemSetCol(pGui, config->sliderUpEl, GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  } else {
    gslc_ElemSetTxtCol(pGui, config->sliderUpEl, GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, config->sliderUpEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }

  // Down Scroller indicator
  if (config->scroll == config->scrollMax) {
    gslc_ElemSetTxtCol(pGui, config->sliderDownEl, GSLC_COL_GRAY_LT1);
    gslc_ElemSetCol(pGui, config->sliderDownEl, GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
  } else {
    gslc_ElemSetTxtCol(pGui, config->sliderDownEl, GSLC_COL_GREEN);
    gslc_ElemSetCol(pGui, config->sliderDownEl, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  }
}

// Reset Cur Slider pos
void vlist_sliderResetCurPos(gslc_tsGui *pGui, struct vlist_config *config) {
  int i_slot_old = gslc_ElemXSliderGetPos(pGui, config->sliderEl);
  vlist_sliderChangeCurPos(pGui, config, i_slot_old * -1);
}


void vlist_sliderClear(gslc_tsGui *pGui, struct vlist_config *config) {
  for (int i = 0; i < config->per; ++i) {
    gslc_ElemSetTxtStr(pGui, config->refs[i], (char*)" ");
    gslc_ElemSetFillEn(pGui, config->refs[i], false);
  }
}

void vlist_sliderMessage(gslc_tsGui *pGui, struct vlist_config *config, char* msg) {
  vlist_sliderClear(pGui, config);
  gslc_ElemSetTxtStr(pGui, config->refs[0], msg);
}

void vlist_sliderDraw(gslc_tsGui *pGui, struct vlist_config *config, char **list, int maxLen) {
  int pgAdd = config->scroll * config->per;

  for (int i = 0; i < config->per; ++i) {
    int iPg = i + pgAdd;

    if (iPg < config->len) {
      char str[maxLen];
      strlcpy(str, list[iPg], maxLen);
      gslc_ElemSetTxtStr(pGui, config->refs[i], str);
    } else {
      gslc_ElemSetTxtStr(pGui, config->refs[i], (char*)" ");
    }

    if (iPg == config->cur) {
      gslc_ElemSetFillEn(pGui, config->refs[i], true);
    } else {
      gslc_ElemSetFillEn(pGui, config->refs[i], false);
    }
  }
}
