#include "pages.h"
#include "shared.h"
#include <time.h> // system time clocks
#include <unistd.h>

void touchscreenPageOpen(gslc_tsGui *pGui, int ePage) {
  if (ePage < 0) { return; } 
  printf("Opening: %d\n", ePage);

  // Open Page
  m_page_previous = m_page_current;

  if (cbInit[ePage]) {
    printf("Page Init Found: %d\n", ePage);
    cbInit[ePage](pGui);
  }
  
  printf("Setting Page Current: %d\n", ePage);
  gslc_SetPageCur(pGui, ePage);
  m_page_current = ePage;

  if (cbOpen[m_page_current]) {
    printf("Page Open Found: %d\n", m_page_current);
    cbOpen[m_page_current](pGui);
  }
}


void touchscreenPageClose(gslc_tsGui *pGui, int ePage) {
  if (ePage < 0) { return; } 
  if (cbDestroy[ePage]) { cbDestroy[ePage](pGui); }
}

void touchScreenPageCloseAll(gslc_tsGui *pGui) {
  for (size_t i = 0; i < MAX_PAGES; i++) {
    if (cbDestroy[i] && cbInit[i] == NULL) {
      cbDestroy[i](pGui);
    }
  }
}
