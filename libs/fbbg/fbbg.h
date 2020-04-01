




#ifndef _LIB_RASPIDMX_BG_H_
#define _LIB_RASPIDMX_BG_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "bcm_host.h"

typedef struct
{
  int32_t layer;
  DISPMANX_RESOURCE_HANDLE_T resource;
  DISPMANX_ELEMENT_HANDLE_T element;
} FBBG_BACKGROUND_LAYER_T;
FBBG_BACKGROUND_LAYER_T fbbg_backgroundLayer;

DISPMANX_DISPLAY_HANDLE_T fbbg_display;

int fbbgRunning;

int fbbg_toggle();
int fbbg_start();
void fbbg_stop();

#endif // _LIB_RASPIDMX_BG_H_