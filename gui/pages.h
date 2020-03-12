#ifndef _PAGES_H_
#define _PAGES_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum {
  E_PG_MAIN,
  E_PG_KEYBOARD,
  E_PG_MSGBOX,
  E_PG_SYSTEM,
  E_PG_WIFI,
  E_PG_WIFI_LIST,
  E_PG_WIFI_LIST_SAVED,
  E_PG_SLIDESHOW,
  E_PG_SKYDIVEORBUST,
  E_PG_SDOB_SUBMIT,
  E_PG_SDOB_VIDEOLIST,
  E_PG_SDOB_ROUNDLIST,
  MAX_PAGES
};

#include "GUIslice.h"
#include "GUIslice-wrapper/GUIslice-wrapper.h"

int m_page_current;
int m_page_show; // = -1;
int m_page_previous; // = -1;
int m_page_popup;

void (*cbInit[MAX_PAGES])(gslc_tsGui *pGui);
void (*cbOpen[MAX_PAGES])(gslc_tsGui *pGui);
int (*cbThread[MAX_PAGES])(gslc_tsGui *pGui);
void (*cbClose[MAX_PAGES])(gslc_tsGui *pGui);
void (*cbDestroy[MAX_PAGES])(gslc_tsGui *pGui);


// Initialize Page
void touchscreenPageInit(gslc_tsGui *pGui, int ePage);
void touchscreenPageOpen(gslc_tsGui *pGui, int ePage);
void touchscreenPageClose(gslc_tsGui *pGui, int ePage);
void touchscreenPageDestroy(gslc_tsGui *pGui, int ePage);
void touchScreenPageCloseAll(gslc_tsGui *pGui);
void touchScreenPageDestroyAll(gslc_tsGui *pGui);

void touchscreenPopupMsgBox(gslc_tsGui *pGui, const char* title, const char* fmt, ...);
void touchscreenPopupMsgBoxClose(gslc_tsGui *pGui);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_H_
