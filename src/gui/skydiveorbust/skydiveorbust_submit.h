#ifndef _PAGES_SDOB_SUBMIT_H_
#define _PAGES_SDOB_SUBMIT_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

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
  E_SDOB_SUBMIT_EL_TXT_SUBMITTING,
  E_SDOB_SUBMIT_EL_TXT_TMP,

  E_SDOB_SUBMIT_EL_BTN_TMPKB,

  E_SDOB_SUBMIT_EL_MAX
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

int pg_sdob_submit_is_submitting;

// Double press clear button
int pg_sdob_submit_clearCheck;

void pg_sdobSubmitAction();

int pg_sdobSubmitButtonRotaryCW();
int pg_sdobSubmitButtonRotaryCCW();
int pg_sdobSubmitButtonLeftPressed();
int pg_sdobSubmitButtonRightPressed();
int pg_sdobSubmitButtonRotaryPressed();
int pg_sdobSubmitButtonLeftHeld();
int pg_sdobSubmitButtonRightHeld();
int pg_sdobSubmitButtonRotaryHeld();
int pg_sdobSubmitButtonDoubleHeld();

void pg_sdobSubmitButtonSetFuncs();

void pg_sdobSubmit_init(gslc_tsGui *pGui);
void pg_sdobSubmit_open(gslc_tsGui *pGui);
void pg_sdobSubmit_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_sdobSubmit_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_SDOB_SUBMIT_H_
