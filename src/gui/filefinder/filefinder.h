#ifndef _PAGES_FILEFINDER_H_
#define _PAGES_FILEFINDER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_FILEFINDER_CLEAN,

  E_FILEFINDER_EL_BTN_A,
  E_FILEFINDER_EL_BTN_B,
  E_FILEFINDER_EL_BTN_C,
  E_FILEFINDER_EL_BTN_D,
  E_FILEFINDER_EL_BTN_E,
  E_FILEFINDER_EL_BTN_F,
  E_FILEFINDER_EL_BTN_G,
  E_FILEFINDER_EL_BTN_H,


  E_FILEFINDER_EL_BOX,
  E_FILEFINDER_EL_OPEN_STARTX,
  E_FILEFINDER_EL_OPEN_SLIDESHOW,
  E_FILEFINDER_EL_OPEN_SKYDIVEORBUST,
  E_FILEFINDER_EL_TXT_TMP,
  E_FILEFINDER_EL_BTN_TMP,
  E_FILEFINDER_EL_BTN_TMPKB,

  E_FILEFINDER_EL_PLAYLIST,
  E_FILEFINDER_EL_PLAYLIST_UL,

  E_FILEFINDER_EL_MAX
};

#define MAX_ELEM_PG_FILEFINDER      E_FILEFINDER_EL_MAX + 11
#define MAX_ELEM_PG_FILEFINDER_RAM  MAX_ELEM_PG_FILEFINDER

gslc_tsElem m_asPgFileFinderElem[MAX_ELEM_PG_FILEFINDER_RAM];
gslc_tsElemRef m_asPgFileFinderElemRef[MAX_ELEM_PG_FILEFINDER];

gslc_tsElemRef* pg_fileFinderEl[E_FILEFINDER_EL_MAX];

struct vlist_config *pg_fileFinder_listConfig;
struct fileStruct **pg_fileFinder_list;
struct fileStruct **pg_fileFinder_listFolders;
char* pg_fileFinder_currentFolderPath;
gslc_tsXSlider pg_fileFinder_listSlider;

void pg_fileFinderButtonRotaryCW();
void pg_fileFinderButtonRotaryCCW();
void pg_fileFinderButtonLeftPressed();
void pg_fileFinderButtonRightPressed();
void pg_fileFinderButtonRotaryPressed();
void pg_fileFinderButtonLeftHeld();
void pg_fileFinderButtonRightHeld();
void pg_fileFinderButtonRotaryHeld();
void pg_fileFinderButtonDoubleHeld();
void pg_fileFinderButtonSetFuncs();

void pg_fileFinder_loadFolder(gslc_tsGui *pGui, char* folderPath);

void pg_fileFinder_init(gslc_tsGui *pGui);
void pg_fileFinder_open(gslc_tsGui *pGui);
void pg_fileFinder_destroy(gslc_tsGui *pGui);
void pg_fileFinder(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_FILEFINDER_H_
