
#include <stdio.h>
#include <stdlib.h>
#include <xdo.h>

#include "libs/shared.h"
#include "xdotool-wrapper.h"


void xdotool_wrapper_wakeUp() {
    xdo_t * x = xdo_new(":0.0");
    xdo_send_keysequence_window(x, CURRENTWINDOW, "Shift_L", 0);
    xdo_free(x);
}