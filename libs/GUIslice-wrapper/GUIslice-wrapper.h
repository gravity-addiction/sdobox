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
#define IMG_BLACK_FULLSCREEN      "images/black480x320.bmp"
#define IMG_GREEN_FULLSCREEN      "images/green480x320.bmp"
#define IMG_TRANSPARENT_FULLSCREEN      "images/fushcia480x320.bmp"
#define IMG_BTNEXIT32X32          "images/btn-exit32x32.bmp"
#define IMG_BTNEXIT_SEL32X32_SEL  "images/btn-exit_sel32x32.bmp"

// 64px X 64px images
#define IMG_BTN_UP_ARROW          "images/up_arrow.bmp"
#define IMG_BTN_UP_ARROW_SEL      "images/up_arrow_sel.bmp"
#define IMG_BTN_RIGHT_ARROW       "images/right_arrow.bmp"
#define IMG_BTN_RIGHT_ARROW_SEL   "images/right_arrow_sel.bmp"
#define IMG_BTN_DOWN_ARROW        "images/down_arrow.bmp"
#define IMG_BTN_DOWN_ARROW_SEL    "images/down_arrow_sel.bmp"
#define IMG_BTN_LEFT_ARROW        "images/left_arrow.bmp"
#define IMG_BTN_LEFT_ARROW_SEL    "images/left_arrow_sel.bmp"
#define IMG_BTN_BACK_ARROW        "images/back_arrow.bmp"
#define IMG_BTN_BACK_ARROW_SEL    "images/back_arrow_sel.bmp"
#define IMG_BTN_SELECT            "images/select.bmp"
#define IMG_BTN_SELECT_SEL        "images/select_sel.bmp"
#define IMG_BTN_HOME              "images/home.bmp"
#define IMG_BTN_HOME_SEL          "images/home_sel.bmp"
#define IMG_BTN_STOP              "images/stop.bmp"
#define IMG_BTN_STOP_SEL          "images/stop_sel.bmp"
#define IMG_BTN_PLAY              "images/play.bmp"
#define IMG_BTN_PLAY_SEL          "images/play_sel.bmp"
#define IMG_BTN_PAUSE             "images/pause.bmp"
#define IMG_BTN_PAUSE_SEL         "images/pause_sel.bmp"
#define IMG_BTN_BACK_SINGLE       "images/back_single.bmp"
#define IMG_BTN_BACK_SINGLE_SEL   "images/back_single_sel.bmp"
#define IMG_BTN_BACK              "images/back.bmp"
#define IMG_BTN_BACK_SEL          "images/back_sel.bmp"
#define IMG_BTN_FORWARD_SINGLE    "images/forward_single.bmp"
#define IMG_BTN_FORWARD_SINGLE_SEL  "images/forward_single_sel.bmp"
#define IMG_BTN_FORWARD           "images/forward.bmp"
#define IMG_BTN_FORWARD_SEL       "images/forward_sel.bmp"
#define IMG_BTN_PAGE_UP           "images/page_up.bmp"
#define IMG_BTN_PAGE_UP_SEL       "images/page_up_sel.bmp"
#define IMG_BTN_PAGE_DOWN         "images/page_down.bmp"
#define IMG_BTN_PAGE_DOWN_SEL     "images/page_down_sel.bmp"
#define IMG_BTN_VOLUME_UP         "images/volume_up.bmp"
#define IMG_BTN_VOLUME_UP_SEL     "images/volume_up_sel.bmp"
#define IMG_BTN_VOLUME_DOWN       "images/volume_down.bmp"
#define IMG_BTN_VOLUME_DOWN_SEL   "images/volume_down_sel.bmp"
#define IMG_BTN_VOLUME_MUTE       "images/volume_mute.bmp"
#define IMG_BTN_VOLUME_MUTE_SEL   "images/volume_mute_sel.bmp"

// 48px X 48px images
#define IMG_BTN_VOLUME_ONLY       "images/volume_only.bmp"
#define IMG_BTN_VOLUME_ONLY_SEL   "images/volume_only_sel.bmp"

#define MAX_ELEM_PG_DEFAULT        100 // Default
#define MAX_ELEM_PG_DEFAULT_RAM    MAX_ELEM_PG_DEFAULT // Default

// Third Party Libs
#include "shared.h"
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

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_WRAPPER_H_