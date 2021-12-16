#ifndef _PAGES_SDOB_VIDEOLIST_H_
#define _PAGES_SDOB_VIDEOLIST_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_SDOB_VIDEOLIST_EL_BOX,
  E_SDOB_VIDEOLIST_EL_BTN_CANCEL,
  E_SDOB_VIDEOLIST_EL_BTN_CLEAR,
  E_SDOB_VIDEOLIST_EL_BTN_CHANGE,
  E_SDOB_VIDEOLIST_EL_BTN_FOLDER,
  E_SDOB_VIDEOLIST_EL_BTN_REFRESH,

  E_SDOB_VIDEOLIST_EL_TXT_TMP,
  E_SDOB_VIDEOLIST_EL_BTN_TMPKB,

  E_SDOB_VIDEOLIST_EL_MAX
};

#define MAX_ELEM_SDOB_VIDEOLIST      E_SDOB_VIDEOLIST_EL_MAX + 11

gslc_tsElem m_asPgSdobVideolistElem[MAX_ELEM_SDOB_VIDEOLIST];
gslc_tsElemRef m_asPgSdobVideolistElemRef[MAX_ELEM_SDOB_VIDEOLIST];

gslc_tsElemRef* pg_sdobVideolistEl[E_SDOB_VIDEOLIST_EL_MAX];

struct vlist_config *pg_sdobVideo_listConfig;
struct fileStruct **pg_sdobVideo_list;
gslc_tsXSlider pg_sdobVideo_listSlider;

void pg_sdobVideoList_loadFolder(gslc_tsGui *pGui, char* folderPath);


int pg_sdobVideoListButtonRotaryCW();
int pg_sdobVideoListButtonRotaryCCW();
int pg_sdobVideoListButtonLeftPressed();
int pg_sdobVideoListButtonRightPressed();
int pg_sdobVideoListButtonRotaryPressed();
int pg_sdobVideoListButtonLeftHeld();
int pg_sdobVideoListButtonRightHeld();
int pg_sdobVideoListButtonRotaryHeld();
int pg_sdobVideoListButtonDoubleHeld();

void pg_sdobVideoListButtonSetFuncs();

void pg_sdobVideoList_init(gslc_tsGui *pGui);
void pg_sdobVideoList_open(gslc_tsGui *pGui);
void pg_sdobVideoList_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_sdobVideoList_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_SDOB_VIDEOLIST_H_
