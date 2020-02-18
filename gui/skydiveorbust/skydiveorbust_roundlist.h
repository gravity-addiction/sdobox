#ifndef _PAGES_SDOB_ROUND_H_
#define _PAGES_SDOB_ROUND_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_SDOB_ROUND_EL_BOX,
  E_SDOB_ROUND_EL_BTN_0,
  E_SDOB_ROUND_EL_BTN_1,
  E_SDOB_ROUND_EL_BTN_2,
  E_SDOB_ROUND_EL_BTN_3,
  E_SDOB_ROUND_EL_BTN_4,

  E_SDOB_ROUND_EL_MAX
};

gslc_tsElem m_asPgSdobRoundElem[E_SDOB_ROUND_EL_MAX];
gslc_tsElemRef m_asPgSdobRoundElemRef[E_SDOB_ROUND_EL_MAX];

gslc_tsElemRef* pg_sdobRoundEl[E_SDOB_ROUND_EL_MAX];

void pg_sdobRoundButtonSetFuncs();

void pg_sdobRound_init(gslc_tsGui *pGui);
void pg_sdobRound_open(gslc_tsGui *pGui);
void pg_sdobRound_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_sdobRound_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_SDOB_ROUND_H_
