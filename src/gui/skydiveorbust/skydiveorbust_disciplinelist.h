#ifndef _PAGES_SDOB_DISCIPLINELIST_H_
#define _PAGES_SDOB_DISCIPLINELIST_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_SDOB_DISCIPLINELIST_EL_BOX,
  E_SDOB_DISCIPLINELIST_EL_BTN_CANCEL,
  E_SDOB_DISCIPLINELIST_EL_BTN_CLEAR,
  E_SDOB_DISCIPLINELIST_EL_BTN_CHANGE,
  E_SDOB_DISCIPLINELIST_EL_BTN_FOLDER,
  E_SDOB_DISCIPLINELIST_EL_BTN_REFRESH,

  E_SDOB_DISCIPLINELIST_EL_TXT_TMP,
  E_SDOB_DISCIPLINELIST_EL_BTN_TMPKB,

  E_SDOB_DISCIPLINELIST_EL_MAX
};

#define MAX_ELEM_SDOB_DISCIPLINELIST      E_SDOB_DISCIPLINELIST_EL_MAX + 11

gslc_tsElem m_asPgSdobDisciplinelistElem[MAX_ELEM_SDOB_DISCIPLINELIST];
gslc_tsElemRef m_asPgSdobDisciplinelistElemRef[MAX_ELEM_SDOB_DISCIPLINELIST];

gslc_tsElemRef* pg_sdobDisciplinelistEl[E_SDOB_DISCIPLINELIST_EL_MAX];

struct pg_sdobDisciplineStruct {
  char *name;
  char *ident;
};

struct vlist_config *pg_sdobDiscipline_listConfig;
struct pg_sdobDisciplineStruct* pg_sdobDiscipline_list;
gslc_tsXSlider pg_sdobDiscipline_listSlider;

void pg_sdobDisciplineList_loadDisciplines(gslc_tsGui *pGui);

int pg_sdobDisciplineListButtonRotaryCW();
int pg_sdobDisciplineListButtonRotaryCCW();
int pg_sdobDisciplineListButtonLeftPressed();
int pg_sdobDisciplineListButtonRightPressed();
int pg_sdobDisciplineListButtonRotaryPressed();
int pg_sdobDisciplineListButtonLeftHeld();
int pg_sdobDisciplineListButtonRightHeld();
int pg_sdobDisciplineListButtonRotaryHeld();
int pg_sdobDisciplineListButtonDoubleHeld();

void pg_sdobDisciplineListButtonSetFuncs();

void pg_sdobDisciplineList_init(gslc_tsGui *pGui);
void pg_sdobDisciplineList_open(gslc_tsGui *pGui);
void pg_sdobDisciplineList_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_sdobDisciplineList_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_SDOB_DISCIPLINELIST_H_
