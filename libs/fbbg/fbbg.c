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
    VC_IMAGE_TYPE_T type = VC_IMAGE_RGBA16;
    uint32_t vc_image_ptr;

    bg->resource = vc_dispmanx_resource_create(type, 1, 1, &vc_image_ptr);

    if (bg->resource == 0) {
      return;
    }

    //---------------------------------------------------------------------

    VC_RECT_T dst_rect;
    vc_dispmanx_rect_set(&dst_rect, 0, 0, 1, 1);

    bg->layer = layer;

    vc_dispmanx_resource_write_data(bg->resource,
                                             type,
                                             sizeof(colour),
                                             &colour,
                                             &dst_rect);

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

}

//-------------------------------------------------------------------------

void
fbbg_destroy(
    FBBG_BACKGROUND_LAYER_T *bg)
{
  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
  vc_dispmanx_element_remove(update, bg->element);
  vc_dispmanx_update_submit_sync(update);
  vc_dispmanx_resource_delete(bg->resource);
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
  if (fbbgRunning == 1) { return 0; }

  uint16_t background = 0x000F;
  uint32_t displayNumber = 0;
  uint32_t displayLayer = 0;
  bcm_host_init();

  fbbg_display = vc_dispmanx_display_open(displayNumber);

  DISPMANX_MODEINFO_T info;
  vc_dispmanx_display_get_info(fbbg_display, &info);

  fbbg_init(&fbbg_backgroundLayer, background, displayLayer);

  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);

  fbbg_add(&fbbg_backgroundLayer, fbbg_display, update);

  vc_dispmanx_update_submit_sync(update);

  fbbgRunning = 1;
  return 1;
}

//-------------------------------------------------------------------------

void fbbg_stop() {
  if (fbbgRunning != 1) { return; }
  fbbg_destroy(&fbbg_backgroundLayer);

  vc_dispmanx_display_close(fbbg_display);

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
