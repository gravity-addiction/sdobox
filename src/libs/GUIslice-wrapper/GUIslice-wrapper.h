#ifndef _GUISLICE_WRAPPER_H_
#define _GUISLICE_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// Include any extended elements
#define MAX_PATH  255
#define MAX_STR   100
#define GSLC_LOCAL_STR_LEN  100

// ------------------------------------------------
// Defines for fonts
// ------------------------------------------------
#define FONT_NATO_MONO1 "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"

// ------------------------------------------------
// Defines for images
// ------------------------------------------------
// default images
#define IMG_BLACK_FULLSCREEN      "/opt/sdobox/images/black480x320.bmp"
#define IMG_GREEN_FULLSCREEN      "/opt/sdobox/images/green480x320.bmp"
#define IMG_TRANSPARENT_FULLSCREEN      "/opt/sdobox/images/fushcia480x320.bmp"
#define IMG_BTNEXIT32X32          "/opt/sdobox/images/btn-exit32x32.bmp"
#define IMG_BTNEXIT_SEL32X32_SEL  "/opt/sdobox/images/btn-exit_sel32x32.bmp"

// Screens 320px x 480px
#define IMG_SCREEN_BLANK         "/opt/sdobox/images/screen/blank.bmp"
#define IMG_SCREEN_MAIN_1         "/opt/sdobox/images/screen/main.bmp"
#define IMG_SCREEN_COOKBOOK       "/opt/sdobox/images/screen/cookbook.bmp"
#define IMG_SCREEN_SPOTIFY       "/opt/sdobox/images/screen/spotify.bmp"
#define IMG_SCREEN_SYSTEM       "/opt/sdobox/images/screen/system.bmp"

// 64px X 64px images
#define IMG_BTN_UP_ARROW          "/opt/sdobox/images/up_arrow.bmp"
#define IMG_BTN_UP_ARROW_SEL      "/opt/sdobox/images/up_arrow_sel.bmp"
#define IMG_BTN_RIGHT_ARROW       "/opt/sdobox/images/right_arrow.bmp"
#define IMG_BTN_RIGHT_ARROW_SEL   "/opt/sdobox/images/right_arrow_sel.bmp"
#define IMG_BTN_DOWN_ARROW        "/opt/sdobox/images/down_arrow.bmp"
#define IMG_BTN_DOWN_ARROW_SEL    "/opt/sdobox/images/down_arrow_sel.bmp"
#define IMG_BTN_LEFT_ARROW        "/opt/sdobox/images/left_arrow.bmp"
#define IMG_BTN_LEFT_ARROW_SEL    "/opt/sdobox/images/left_arrow_sel.bmp"
#define IMG_BTN_BACK_ARROW        "/opt/sdobox/images/back_arrow.bmp"
#define IMG_BTN_BACK_ARROW_SEL    "/opt/sdobox/images/back_arrow_sel.bmp"
#define IMG_BTN_SELECT            "/opt/sdobox/images/select.bmp"
#define IMG_BTN_SELECT_SEL        "/opt/sdobox/images/select_sel.bmp"
#define IMG_BTN_HOME              "/opt/sdobox/images/home.bmp"
#define IMG_BTN_HOME_SEL          "/opt/sdobox/images/home_sel.bmp"
#define IMG_BTN_STOP              "/opt/sdobox/images/stop.bmp"
#define IMG_BTN_STOP_SEL          "/opt/sdobox/images/stop_sel.bmp"
#define IMG_BTN_PLAY              "/opt/sdobox/images/play.bmp"
#define IMG_BTN_PLAY_SEL          "/opt/sdobox/images/play_sel.bmp"
#define IMG_BTN_PAUSE             "/opt/sdobox/images/pause.bmp"
#define IMG_BTN_PAUSE_SEL         "/opt/sdobox/images/pause_sel.bmp"
#define IMG_BTN_BACK_SINGLE       "/opt/sdobox/images/back_single.bmp"
#define IMG_BTN_BACK_SINGLE_SEL   "/opt/sdobox/images/back_single_sel.bmp"
#define IMG_BTN_BACK              "/opt/sdobox/images/back.bmp"
#define IMG_BTN_BACK_SEL          "/opt/sdobox/images/back_sel.bmp"
#define IMG_BTN_FORWARD_SINGLE    "/opt/sdobox/images/forward_single.bmp"
#define IMG_BTN_FORWARD_SINGLE_SEL  "/opt/sdobox/images/forward_single_sel.bmp"
#define IMG_BTN_FORWARD           "/opt/sdobox/images/forward.bmp"
#define IMG_BTN_FORWARD_SEL       "/opt/sdobox/images/forward_sel.bmp"
#define IMG_BTN_PAGE_UP           "/opt/sdobox/images/page_up.bmp"
#define IMG_BTN_PAGE_UP_SEL       "/opt/sdobox/images/page_up_sel.bmp"
#define IMG_BTN_PAGE_DOWN         "/opt/sdobox/images/page_down.bmp"
#define IMG_BTN_PAGE_DOWN_SEL     "/opt/sdobox/images/page_down_sel.bmp"
#define IMG_BTN_VOLUME_UP         "/opt/sdobox/images/volume_up.bmp"
#define IMG_BTN_VOLUME_UP_SEL     "/opt/sdobox/images/volume_up_sel.bmp"
#define IMG_BTN_VOLUME_DOWN       "/opt/sdobox/images/volume_down.bmp"
#define IMG_BTN_VOLUME_DOWN_SEL   "/opt/sdobox/images/volume_down_sel.bmp"
#define IMG_BTN_VOLUME_MUTE       "/opt/sdobox/images/volume_mute.bmp"
#define IMG_BTN_VOLUME_MUTE_SEL   "/opt/sdobox/images/volume_mute_sel.bmp"

// 48px X 48px images
#define IMG_BTN_VOLUME_ONLY       "/opt/sdobox/images/volume_only.bmp"
#define IMG_BTN_VOLUME_ONLY_SEL   "/opt/sdobox/images/volume_only_sel.bmp"

#define MAX_ELEM_PG_DEFAULT        100 // Default
#define MAX_ELEM_PG_DEFAULT_RAM    MAX_ELEM_PG_DEFAULT // Default

// Third Party Libs
#include "libs/shared.h"
#include "gui/pages.h"
#include <wiringPi.h> // Gordons Wiring Pi

#include "GUIslice-wrapper-enum.h"
#include "GUIslice.h"
#include "GUIslice_drv.h"
#include "elem/XSlider.h"
#include "elem/XTextbox.h"


int m_show_wifi;
int m_hide_wifi;
int m_show_keyboard;
int m_hide_keyboard;

// GUI Elements
gslc_tsGui m_gui;
gslc_tsDriver m_drv;
gslc_tsFont m_asFont[MAX_FONTS];
gslc_tsPage m_asPage[MAX_PAGES];

gslc_tsRect rFullscreen;

int m_gslcWrapperThreadStop; // Stopping Thread
int m_gslcWrapperThreadRunning; // Running Thread

int gslc_wrapper_initalized; // Flag for gslc_init()

void UserInitEnv();
int guislice_wrapper_init(gslc_tsGui *pGui);
int guislice_wrapper_quit(gslc_tsGui *pGui);

void guislice_wrapper_mirror_toggle(gslc_tsGui *pGui);
int guislice_wrapper_mirroring();

void guislice_wrapper_setClock(gslc_tsGui *pGui, gslc_tsElemRef *pElemRef, int forceUpdate);

void guislice_wrapper_setVolumeDisplay(gslc_tsGui *pGui, gslc_tsElemRef *pElemRef);
void guislice_wrapper_setVolume(gslc_tsGui *pGui, gslc_tsElemRef *pElemRef, int forceUpdate);
void guislice_wrapper_setVolumeAndDisplay(gslc_tsGui *pGui, gslc_tsElemRef *pElemRef, int forceUpdate, gslc_tsElemRef *pElemRefDisplay);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_WRAPPER_H_