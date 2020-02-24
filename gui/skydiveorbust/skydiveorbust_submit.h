#ifndef _PAGES_SDOB_SUBMIT_H_
#define _PAGES_SDOB_SUBMIT_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

#define E_SDOB_SUBMIT_MAX_JUDGES 10

enum {
  E_SDOB_SUBMIT_EL_BOX,
  E_SDOB_SUBMIT_EL_BTN_CANCEL,
  E_SDOB_SUBMIT_EL_BTN_CLEAR,
  E_SDOB_SUBMIT_EL_BTN_SUBMIT,
  E_SDOB_SUBMIT_EL_TXT_VIDEOFILE,
  E_SDOB_SUBMIT_EL_TXT_CHAPTER,
  E_SDOB_SUBMIT_EL_TXT_SCORE,
  E_SDOB_SUBMIT_EL_TXT_TEAMNUM,
  E_SDOB_SUBMIT_EL_TXT_ROUNDNUM,
  E_SDOB_SUBMIT_EL_TXT_JUDGES,
  E_SDOB_SUBMIT_EL_BTN_JUDGE0,

  E_SDOB_SUBMIT_EL_MAX = E_SDOB_SUBMIT_EL_BTN_JUDGE0 + E_SDOB_SUBMIT_MAX_JUDGES
};

gslc_tsElem m_asPgSdobSubmitElem[E_SDOB_SUBMIT_EL_MAX];
gslc_tsElemRef m_asPgSdobSubmitElemRef[E_SDOB_SUBMIT_EL_MAX];

gslc_tsElemRef* pg_sdobSubmitEl[E_SDOB_SUBMIT_EL_MAX];

struct pg_sdob_submit_info {
  char chapterStr[32];
  char teamStr[32];
  char roundStr[32];
  char scoreStr[32];
};
struct pg_sdob_submit_info sdob_submit_info;

// Double press clear button
int pg_sdob_submit_clearCheck;

void pg_sdobSubmitButtonRotaryCW();
void pg_sdobSubmitButtonRotaryCCW();
void pg_sdobSubmitButtonLeftPressed();
void pg_sdobSubmitButtonRightPressed();
void pg_sdobSubmitButtonRotaryPressed();
void pg_sdobSubmitButtonLeftHeld();
void pg_sdobSubmitButtonRightHeld();
void pg_sdobSubmitButtonRotaryHeld();
void pg_sdobSubmitButtonDoubleHeld();
void pg_sdobSubmitButtonSetFuncs();

void pg_sdobSubmit_init(gslc_tsGui *pGui);
void pg_sdobSubmit_open(gslc_tsGui *pGui);
void pg_sdobSubmit_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_sdobSubmit_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_SDOB_SUBMIT_H_
