#ifndef _PAGES_SPOTIFY_H_
#define _PAGES_SPOTIFY_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_SPOTIFY_EL_BOX,
  E_SPOTIFY_EL_BTN_X,
  E_SPOTIFY_EL_CLOCK,
  E_SPOTIFY_EL_VOLUME,
  E_SPOTIFY_EL_VOLUME_DISPLAY,

  E_SPOTIFY_EL_HELLO_WORLD,
  E_CSPOTIFY_EL_BTN_HELLO_WORLD,
  
  E_SPOTIFY_EL_MAX
};

#define MAX_ELEM_PG_SPOTIFY      E_SPOTIFY_EL_MAX + 1
#define MAX_ELEM_PG_SPOTIFY_RAM  MAX_ELEM_PG_SPOTIFY

gslc_tsElem m_asPgSpotifyElem[MAX_ELEM_PG_SPOTIFY_RAM];
gslc_tsElemRef m_asPgSpotifyElemRef[MAX_ELEM_PG_SPOTIFY];

gslc_tsElemRef* pg_spotifyEl[E_SPOTIFY_EL_MAX];

gslc_tsXSlider pg_spotify_sliderVolume;
uint16_t pg_spotify_iVolume;
char* pg_spotify_cVolume;

struct vlist_config *pg_spotify_listConfig;
struct fileStruct **pg_spotify_list;
struct fileStruct **pg_spotify_listFolders;
char* pg_spotify_currentFolderPath;
gslc_tsXSlider pg_spotify_listSlider;

void pg_spotifyButtonRotaryCW();
void pg_spotifyButtonRotaryCCW();
void pg_spotifyButtonLeftPressed();
void pg_spotifyButtonRightPressed();
void pg_spotifyButtonRotaryPressed();
void pg_spotifyButtonLeftHeld();
void pg_spotifyButtonRightHeld();
void pg_spotifyButtonRotaryHeld();
void pg_spotifyButtonDoubleHeld();

void pg_spotify_init(gslc_tsGui *pGui);
void pg_spotify_open(gslc_tsGui *pGui);
void pg_spotify_destroy(gslc_tsGui *pGui);
void pg_spotify(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_SPOTIFY_H_
