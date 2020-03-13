#ifndef _PAGES_MSGBOX_H_
#define _PAGES_MSGBOX_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_MSGBOX_EL_BOX,
  E_MSGBOX_EL_BOX_HEADER,
  E_MSGBOX_EL_BOX_MSG,
  E_MSGBOX_EL_BOX_OK,

  E_MSGBOX_EL_MAX
};

#define MAX_ELEM_PG_MSGBOX      E_MSGBOX_EL_MAX + 11
#define MAX_ELEM_PG_MSGBOX_RAM  MAX_ELEM_PG_MSGBOX

gslc_tsElem m_asPgMsgboxElem[MAX_ELEM_PG_MSGBOX_RAM];
gslc_tsElemRef m_asPgMsgboxElemRef[MAX_ELEM_PG_MSGBOX];

gslc_tsElemRef* pg_msgboxEl[E_MSGBOX_EL_MAX];

#define PG_MSGBOX_ROWS 10
#define PG_MSGBOX_COLS 43
gslc_tsXTextbox pg_msgboxTextbox;

void pg_msgbox_setTitle(gslc_tsGui *pGui, const char* title);
void pg_msgbox_setMsg(gslc_tsGui *pGui, char* msg);
bool pg_msgbox_cbBtn_Ok(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool pg_msgbox_cbDrawBox(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);

void pg_msgboxGuiInit(gslc_tsGui *pGui);

void pg_msgbox_init(gslc_tsGui *pGui);
void pg_msgbox_open(gslc_tsGui *pGui);
void pg_msgbox_close(gslc_tsGui *pGui);
void pg_msgbox_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_msgbox(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_MSGBOX_H_
