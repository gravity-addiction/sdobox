#ifndef _PAGES_SDOB_SUBMIT_H_
#define _PAGES_SDOB_SUBMIT_H_

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


void pg_sdobVideoListButtonRotaryCW();
void pg_sdobVideoListButtonRotaryCCW();
void pg_sdobVideoListButtonLeftPressed();
void pg_sdobVideoListButtonRightPressed();
void pg_sdobVideoListButtonRotaryPressed();
void pg_sdobVideoListButtonLeftHeld();
void pg_sdobVideoListButtonRightHeld();
void pg_sdobVideoListButtonRotaryHeld();
void pg_sdobVideoListButtonDoubleHeld();
void pg_sdobVideoListButtonSetFuncs();

void pg_sdobVideoList_init(gslc_tsGui *pGui);
void pg_sdobVideoList_open(gslc_tsGui *pGui);
void pg_sdobVideoList_destroy(gslc_tsGui *pGui);
void __attribute__ ((constructor)) pg_sdobVideoList_setup(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_SDOB_SUBMIT_H_
