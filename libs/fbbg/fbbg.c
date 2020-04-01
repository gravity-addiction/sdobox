#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fbbg.h"
#include "libs/dbg/dbg.h"

//-------------------------------------------------------------------------

void
fbbg_init(
    FBBG_BACKGROUND_LAYER_T *bg,
    uint16_t colour,
    int32_t layer)
{
    int result = 0;
    VC_IMAGE_TYPE_T type = VC_IMAGE_RGBA16;
    uint32_t vc_image_ptr;

    bg->resource = vc_dispmanx_resource_create(type, 1, 1, &vc_image_ptr);
    // assert(bg->resource != 0);
    if (bg->resource == 0) {
      return;
    }

    //---------------------------------------------------------------------

    VC_RECT_T dst_rect;
    vc_dispmanx_rect_set(&dst_rect, 0, 0, 1, 1);

    bg->layer = layer;

    result = vc_dispmanx_resource_write_data(bg->resource,
                                             type,
                                             sizeof(colour),
                                             &colour,
                                             &dst_rect);
    // assert(result == 0);
}

//-------------------------------------------------------------------------

void
fbbg_add(
    FBBG_BACKGROUND_LAYER_T *bg,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update)
{
    VC_DISPMANX_ALPHA_T alpha =
    {
        DISPMANX_FLAGS_ALPHA_FROM_SOURCE,
        255,
        0
    };

    //---------------------------------------------------------------------

    VC_RECT_T src_rect;
    vc_dispmanx_rect_set(&src_rect, 0, 0, 1, 1);

    VC_RECT_T dst_rect;
    vc_dispmanx_rect_set(&dst_rect, 0, 0, 0, 0);

    bg->element =
        vc_dispmanx_element_add(update,
                                display,
                                bg->layer,
                                &dst_rect,
                                bg->resource,
                                &src_rect,
                                DISPMANX_PROTECTION_NONE,
                                &alpha,
                                NULL,
                                DISPMANX_NO_ROTATE);
    // assert(bg->element != 0);
}

//-------------------------------------------------------------------------

void
fbbg_destroy(
    FBBG_BACKGROUND_LAYER_T *bg)
{
    int result = 0;

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    // assert(update != 0);
    if (result == 0) {
      dbgprintf(DBG_ERROR, "FBBG Destroy: Unable to update start\nError: %d", result);
      return;
    }

    result = vc_dispmanx_element_remove(update, bg->element);
    // assert(result == 0);
    if (result != 0) {
      dbgprintf(DBG_ERROR, "FBBG Destroy: Unable to remove element\nError: %d", result);
      return;
    }

    result = vc_dispmanx_update_submit_sync(update);
    // assert(result == 0);
    if (result != 0) {
      dbgprintf(DBG_ERROR, "FBBG Destroy: Unable Submit Sync\nError: %d", result);
      return;
    }

    result = vc_dispmanx_resource_delete(bg->resource);
    if (result != 0) {
      dbgprintf(DBG_ERROR, "FBBG Destroy: Unable Delete Resource\nError: %d", result);
      return;
    }
}

//-------------------------------------------------------------------------

int fbbg_toggle() {
  if (fbbgRunning) {
    fbbg_stop();
    return 0;
  } else {
    return fbbg_start();
  }
}

//-------------------------------------------------------------------------

int fbbg_start()
{
  if (fbbgRunning) { return 0; }

  uint16_t background = 0x000F;
  uint32_t displayNumber = 0;
  uint32_t displayLayer = 0;
  bcm_host_init();

  fbbg_display = vc_dispmanx_display_open(displayNumber);
  // assert(fbbg_display != 0);
  if (fbbg_display == 0) {
    dbgprintf(DBG_ERROR, "FBBG Start: Unable to Open Display\nError: %d", fbbg_display);
    return 0;
  }


  DISPMANX_MODEINFO_T info;
  int result = vc_dispmanx_display_get_info(fbbg_display, &info);
  // assert(result == 0);
  if (result != 0) {
    dbgprintf(DBG_ERROR, "FBBG Start: Unable to Get Display Info\nError: %d", result);
    return 0;
  }

  fbbg_init(&fbbg_backgroundLayer, background, displayLayer);

  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
  // assert(update != 0);
  if (update == 0) {
    dbgprintf(DBG_ERROR, "FBBG Start: Unable to Update Start\nError: %d", result);
    return 0;
  }

  fbbg_add(&fbbg_backgroundLayer, fbbg_display, update);

  result = vc_dispmanx_update_submit_sync(update);
  // assert(result == 0);
  if (result != 0) {
    dbgprintf(DBG_ERROR, "FBBG Start: Unable to Submit Sync\nError: %d", result);
    return 0;
  }

  fbbgRunning = 1;
  return 1;
}

//-------------------------------------------------------------------------

void fbbg_stop() {
  if (!fbbgRunning) { return; }

  fbbg_destroy(&fbbg_backgroundLayer);

  int result = vc_dispmanx_display_close(fbbg_display);
  // assert(result == 0);
  if (result != 0) {
    dbgprintf(DBG_ERROR, "FBBG Stop: Unable to Close Display\nError: %d", result);
  }

  fbbgRunning = 0;
}



//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2013 Andrew Duncan
// Copyright (c) 2020 Gary Taylor
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------
