#ifndef _PAGES_H_
#define _PAGES_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum {
  E_PG_MAIN,
  E_PG_STARTUP,
  E_PG_CONFIGURE,
  E_PG_KEYBOARD,
  E_PG_MSGBOX,
  E_PG_SYSTEM,
  E_PG_WIFI,
  E_PG_WIFI_LIST,
  E_PG_WIFI_LIST_SAVED,
  E_PG_FILEFINDER,
  E_PG_SLIDESHOW,
  E_PG_SKYDIVEORBUST,
  E_PG_SDOB_SUBMIT,
  E_PG_SDOB_VIDEOLIST,
  E_PG_SDOB_ROUNDLIST,
  E_PG_COOKBOOK,
  E_PG_COOKBOOK_HELLO_WORLD,
  E_PG_SPOTIFY,
  E_PG_DUBBING,
  E_PG_USBDRIVE,
  MAX_PAGES
};

#include "GUIslice.h"
#include "libs/GUIslice-wrapper/GUIslice-wrapper.h"

int m_page_current;
int m_page_show; // = -1;
int m_page_previous; // = -1;
int m_page_popup;

int *m_page_stack;
int m_page_stackLen;
int m_page_stackMax;

void (*cbInit[MAX_PAGES])(gslc_tsGui *pGui);
void (*cbOpen[MAX_PAGES])(gslc_tsGui *pGui);
int (*cbThread[MAX_PAGES])(gslc_tsGui *pGui);
void (*cbClose[MAX_PAGES])(gslc_tsGui *pGui);
void (*cbDestroy[MAX_PAGES])(gslc_tsGui *pGui);


// Page Stack management
void touchscreenPageStackAdd(int ePage);
int touchscreenPageStackCur();
int touchscreenPageStackPop();
void touchscreenPageStackReset();
void touchscreenPageGoBack(gslc_tsGui *pGui);

// Initialize Page
void touchscreenPageInit(gslc_tsGui *pGui, int ePage);
void touchscreenPageSetCur(gslc_tsGui *pGui, int ePage);
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
