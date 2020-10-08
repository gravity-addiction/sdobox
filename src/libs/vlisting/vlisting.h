#ifndef _VLISTING_H_
#define _VLISTING_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct vlist_config {
  int max; // maximum elements that fit in memory
  int len; // total elements
  int cur; // current index selected
  int per; // number to display per page
  int scrollMax; // scrollbar current max
  int scroll; // scrollbar current position
  gslc_tsXSlider *slider; // slider guislice element
  gslc_tsElemRef *sliderEl; // slider Element
  gslc_tsElemRef *sliderUpEl; // slider Up Button Element
  gslc_tsElemRef *sliderDownEl; // slider Down Button Element
  gslc_tsElemRef **refs; // array of list elements
};

///////////////////////////////
// Slider Config Management
struct vlist_config * VLIST_INIT_CONFIG(int per, int max);
void VLIST_UPDATE_CONFIG(struct vlist_config *config);
void VLIST_CLEAR_CONFIG(struct vlist_config *config);

///////////////////////////////
// Slider Config Updates
void vlist_sliderDown(struct vlist_config *config);
void vlist_sliderUp(struct vlist_config *config);
void vlist_sliderSetPos(gslc_tsGui *pGui, struct vlist_config *config, int i);
void vlist_sliderSetPosFromCur(gslc_tsGui *pGui, struct vlist_config *config);
// returns 0 when click is invalid
int vlist_clickBtn(struct vlist_config *config, int i);

void vlist_next(struct vlist_config *config);
void vlist_prev(struct vlist_config *config);

///////////////////////////////
// Slider Display Updates
void vlist_sliderUpdate(gslc_tsGui *pGui, struct vlist_config *config);
void vlist_sliderChangeCurPos(gslc_tsGui *pGui, struct vlist_config *config, int amt);
void vlist_sliderResetCurPos(gslc_tsGui *pGui, struct vlist_config *config);
void vlist_sliderClear(gslc_tsGui *pGui, struct vlist_config *config);
void vlist_sliderMessage(gslc_tsGui *pGui, struct vlist_config *config, char* msg);
void vlist_sliderDraw(gslc_tsGui *pGui, struct vlist_config *config, char **list, int maxLen);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _VLISTING_H_