#include "pages.h"
#include "shared.h"
#include <time.h> // system time clocks
#include <unistd.h>


// Initialize Page
void touchscreenPageInit(gslc_tsGui *pGui, int ePage) {
  if (cbInit[ePage]) { cbInit[ePage](pGui); }
}


// Open Page
void touchscreenPageOpen(gslc_tsGui *pGui, int ePage) {
  if (ePage < 0) { return; } 

  // Open Page
  m_page_previous = m_page_current;

  touchscreenPageInit(pGui, ePage);
  
  gslc_SetPageCur(pGui, ePage);
  m_page_current = ePage;

  if (cbOpen[m_page_current]) {
    cbOpen[m_page_current](pGui);
  }
}


// Close Page
void touchscreenPageClose(gslc_tsGui *pGui, int ePage) {
  if (ePage < 0) { return; } 
  if (cbClose[ePage] && cbClose[ePage] != NULL && cbInit[ePage] == NULL) {
    cbClose[ePage](pGui);
  }
}


// Destory Page
void touchscreenPageDestroy(gslc_tsGui *pGui, int ePage) {
  if (ePage < 0) { return; } 

  if (m_page_current == ePage) {
    touchscreenPageClose(pGui, ePage);
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
