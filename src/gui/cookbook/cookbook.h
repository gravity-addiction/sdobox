#ifndef _PAGES_COOKBOOK_H_
#define _PAGES_COOKBOOK_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_COOKBOOK_EL_BOX,
  E_COOKBOOK_EL_BTN_X,
  E_COOKBOOK_EL_CLOCK,

  E_COOKBOOK_EL_HELLO_WORLD,
  E_COOKBOOK_EL_BTN_HELLO_WORLD,
  
  E_COOKBOOK_EL_MAX
};

#define MAX_ELEM_PG_COOKBOOK      E_COOKBOOK_EL_MAX + 1
#define MAX_ELEM_PG_COOKBOOK_RAM  MAX_ELEM_PG_COOKBOOK

gslc_tsElem m_asPgCookbookElem[MAX_ELEM_PG_COOKBOOK_RAM];
gslc_tsElemRef m_asPgCookbookElemRef[MAX_ELEM_PG_COOKBOOK];

gslc_tsElemRef* pg_cookbookEl[E_COOKBOOK_EL_MAX];

struct vlist_config *pg_cookbook_listConfig;
struct fileStruct **pg_cookbook_list;
struct fileStruct **pg_cookbook_listFolders;
char* pg_cookbook_currentFolderPath;
gslc_tsXSlider pg_cookbook_listSlider;

int pg_cookbookButtonRotaryCW();
int pg_cookbookButtonRotaryCCW();
int pg_cookbookButtonLeftPressed();
int pg_cookbookButtonRightPressed();
int pg_cookbookButtonRotaryPressed();
int pg_cookbookButtonLeftHeld();
int pg_cookbookButtonRightHeld();
int pg_cookbookButtonRotaryHeld();
int pg_cookbookButtonDoubleHeld();

void pg_cookbook_init(gslc_tsGui *pGui);
void pg_cookbook_open(gslc_tsGui *pGui);
void pg_cookbook_destroy(gslc_tsGui *pGui);
void pg_cookbook(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_COOKBOOK_H_
