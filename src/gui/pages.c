#include "pages.h"
#include "libs/shared.h"
#include <time.h> // system time clocks
#include <unistd.h>

#include "gui/msgbox/msgbox.h"
#include "libs/dbg/dbg.h"


void touchscreenPageStackAdd(int ePage) {
  if (m_page_stackLen >= m_page_stackMax) {
    m_page_stackMax += 16;
    int *tStack;
    tStack=realloc(m_page_stack, m_page_stackMax * sizeof(int));
    if(tStack == NULL) {
      dbgprintf(DBG_ERROR, "Cannot Realloc Page Stack");
      return;
    }
    m_page_stack = tStack;
  }

  (m_page_stack)[m_page_stackLen] = ePage;
  m_page_stackLen++;
}

int touchscreenPageStackCur() {
  if (m_page_stackLen > 0) {
    return (m_page_stack)[m_page_stackLen - 1];
  } else { return -1; }
}

int touchscreenPageStackPop() {
  if (m_page_stackLen > 0) {
    m_page_stackLen--;
    return (m_page_stack)[m_page_stackLen];
  } else { return -1; }
}


void touchscreenPageStackReset() {
  CLEAR(m_page_stack, m_page_stackLen);
  m_page_stackLen = 0;
}


void touchscreenPageGoBack(gslc_tsGui *pGui) {
  touchscreenPageClose(pGui, touchscreenPageStackCur());
  touchscreenPageStackPop();
  touchscreenPageSetCur(pGui, touchscreenPageStackCur());
}






// update actual page
void touchscreenPageSetCur(gslc_tsGui *pGui, int ePage) {
  if (cbInit[ePage]) { cbInit[ePage](pGui); }

  // gslc_SetBkgndColor(pGui, GSLC_COL_BLACK);
  gslc_SetPageBase(pGui, ePage);
  gslc_SetPageCur(pGui, ePage);

  if (cbOpen[ePage]) {
    printf("Open Page\n");
    cbOpen[ePage](pGui);
  }
}

// Open Page
void touchscreenPageOpen(gslc_tsGui *pGui, int ePage) {
  if (ePage < 0) { return; }

  touchscreenPageStackAdd(ePage);
  touchscreenPageSetCur(pGui, ePage);
  gslc_Update(pGui);
}


// Close Page
void touchscreenPageClose(gslc_tsGui *pGui, int ePage) {
  if (ePage < 0) { return; }
  if (cbClose[ePage] && cbClose[ePage] != NULL && cbInit[ePage] == NULL) {
    cbClose[ePage](pGui);
  }
  gslc_Update(pGui);
}


// Destory Page
void touchscreenPageDestroy(gslc_tsGui *pGui, int ePage) {
  if (ePage < 0) { return; }

  if (m_bQuit == 0 && touchscreenPageStackCur() == ePage) {
    touchscreenPageGoBack(pGui);
  }
  if (cbInit[ePage] == NULL && cbDestroy[ePage]) {
    cbDestroy[ePage](pGui);
  }
}



// Bulk Close All Pages
void touchScreenPageCloseAll(gslc_tsGui *pGui) {
  // Start at 1 as to not destory the main page

  for (size_t i = 1; i < MAX_PAGES; i++) {
    touchscreenPageClose(pGui, i);
  }
}


// Bulk Destory All Pages
void touchScreenPageDestroyAll(gslc_tsGui *pGui) {
  // Start at 1 as to not destory the main page
  for (size_t i = 1; i < MAX_PAGES; i++) {
    touchscreenPageDestroy(pGui, i);
  }
}


void touchscreenPopupMsgBox(gslc_tsGui *pGui, const char* title, const char* fmt, ...) {
  if (cbInit[E_PG_MSGBOX]) { cbInit[E_PG_MSGBOX](pGui); }

  gslc_PopupShow(pGui, E_PG_MSGBOX, true);

  m_page_popup = m_page_current;
  m_page_current = E_PG_MSGBOX;

  if (cbOpen[E_PG_MSGBOX]) {
    cbOpen[E_PG_MSGBOX](pGui);
  }

  pg_msgbox_setTitle(pGui, title);

  va_list ap;
  va_start(ap, fmt);

  const size_t MAXBUF = 431;
  char buf[MAXBUF];
  buf[MAXBUF-1] = '\0';
  int x = vsnprintf(buf, MAXBUF-1, fmt, ap);
  if (x > 0) {
    char newBuf[x];
    strlcpy(newBuf, buf, x);
    pg_msgbox_setMsg(pGui, newBuf);
  }
  va_end(ap);

}

void touchscreenPopupMsgBoxClose(gslc_tsGui *pGui) {
  if (cbClose[E_PG_MSGBOX]) {
    cbClose[E_PG_MSGBOX](pGui);
  }

  gslc_PopupHide(pGui);
  m_page_current = m_page_popup;
  m_page_popup = -1;
}



// Setup Constructor
void __attribute__ ((constructor)) pg_setup(void) {
  m_page_stackLen = 0;
  m_page_stackMax = 16;
  m_page_stack = malloc(m_page_stackMax * sizeof(int));
}