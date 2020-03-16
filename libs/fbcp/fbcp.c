
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
// #include <pthread.h>

#include <bcm_host.h>

#include <wiringPi.h> // Gordons Wiring Pi
#include "fbcp.h"
#include "dbg/dbg.h"


int fbcpData_fbfd = 0;
struct fb_var_screeninfo fbcpData_vinfo;
struct fb_fix_screeninfo fbcpData_finfo;
// static pthread_mutex_t fbcpAccessLock = PTHREAD_MUTEX_INITIALIZER;

void fbcp_copyFrame() {
  // pthread_mutex_trylock(&fbcpAccessLock);
  vc_dispmanx_snapshot(fbcpData_display, fbcpData_screen_resource, DISPMANX_NO_ROTATE);

  // Rotate Image 180 when HDMI is upside down
  if (fbcpData_display_info.transform == 2) {
    vc_dispmanx_resource_read_data(fbcpData_screen_resource, &fbcpData_rect1, fbcpData_fbImagePtr, fbcpData_fbPitch);
    // Rotate Image 180
    int32_t j = 0;
    for (j = 0; j < fbcpData_fbHeight; ++j) {
      int32_t fbYoffset = (fbcpData_fbHeight - j - 1) * fbcpData_fbPitch;
      int32_t i = 0;
      for (i = 0; i < fbcpData_fbWidth; ++i) {
        int32_t fbXoffset = (fbcpData_fbWidth - i - 1) * fbcpData_fbBytesPerPixel;

        uint8_t *tsPixelPtr = fbcpData_tsImagePtr + (i * fbcpData_fbBytesPerPixel) + (j * fbcpData_fbPitch);
        uint8_t *fbPixelPtr = fbcpData_fbImagePtr + fbXoffset + fbYoffset;
        memcpy(tsPixelPtr, fbPixelPtr, fbcpData_fbBytesPerPixel);
      }
    }
    memcpy(fbcpData_fbp, fbcpData_tsImagePtr, fbcpData_fbSize);
  } else {
    // No Rotation
    vc_dispmanx_resource_read_data(fbcpData_screen_resource, &fbcpData_rect1, fbcpData_fbp, fbcpData_fbPitch);
  }
  // pthread_mutex_unlock(&fbcpAccessLock);
}


void fbcp_vsyncCb(DISPMANX_UPDATE_HANDLE_T u, void* arg) {
  vc_dispmanx_vsync_callback(fbcpData_display, NULL, NULL);
  fbcp_copyFrame();

  if (fbcpThreadRunning) {
    vc_dispmanx_vsync_callback(fbcpData_display, fbcp_vsyncCb, NULL);
  }
}


int fbcp_toggle() {
  if (fbcpThreadRunning) {
    fbcp_stop();
    return 0;
  } else {
    return fbcp_start();
  }
}



int fbcp_init() {
  if (fbcpInit) { return fbcpInit; }
  fbcpStartSkip = 0;
  bcm_host_init();
  return 1;
}


int fbcp_alloc() {
  // VC_IMAGE_TRANSFORM_T transform;
  uint32_t image_prt;
  int ret;

  fbcpData_display = vc_dispmanx_display_open(0);
  if (!fbcpData_display) {
    dbgprintf(DBG_ERROR, "Unable to open primary display\n");
    return 0;
  }
  ret = vc_dispmanx_display_get_info(fbcpData_display, &fbcpData_display_info);
  if (ret) {
    dbgprintf(DBG_ERROR, "Unable to get primary display information\n");
    return 0;
  }
  dbgprintf(DBG_INFO, "Primary display is %d x %d\n", fbcpData_display_info.width, fbcpData_display_info.height);


  fbcpData_fbfd = open("/dev/fb1", O_RDWR);
  if (fbcpData_fbfd == -1) {
    dbgprintf(DBG_ERROR, "Unable to open secondary display\n");
    return 0;
  }
  if (ioctl(fbcpData_fbfd, FBIOGET_FSCREENINFO, &fbcpData_finfo)) {
    dbgprintf(DBG_ERROR, "Unable to get secondary display information\n");
    return 0;
  }
  if (ioctl(fbcpData_fbfd, FBIOGET_VSCREENINFO, &fbcpData_vinfo)) {
    dbgprintf(DBG_ERROR, "Unable to get secondary display information\n");
    return 0;
  }


  fbcpData_fbWidth = fbcpData_vinfo.xres;
  fbcpData_fbHeight = fbcpData_vinfo.yres;
  dbgprintf(DBG_INFO, "Second display is %d x %d %dbps\n", fbcpData_fbWidth, fbcpData_fbHeight, fbcpData_vinfo.bits_per_pixel);

  fbcpData_screen_resource = vc_dispmanx_resource_create(VC_IMAGE_RGB565, fbcpData_fbWidth, fbcpData_fbHeight, &image_prt);
  if (!fbcpData_screen_resource) {
    dbgprintf(DBG_ERROR, "Unable to create screen buffer\n");
    close(fbcpData_fbfd);
    vc_dispmanx_display_close(fbcpData_display);
    return 0;
  }

  fbcpData_fbp = (char*) mmap(0, fbcpData_finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbcpData_fbfd, 0);
  if (fbcpData_fbp <= 0) {
    dbgprintf(DBG_ERROR, "Unable to create memory mapping\n");
    close(fbcpData_fbfd);
    ret = vc_dispmanx_resource_delete(fbcpData_screen_resource);
    vc_dispmanx_display_close(fbcpData_display);
    return 0;
  }

  vc_dispmanx_rect_set(&fbcpData_rect1, 0, 0, fbcpData_fbWidth, fbcpData_fbHeight);

  fbcpData_fbBytesPerPixel = fbcpData_vinfo.bits_per_pixel / 8;
  fbcpData_fbPitch = fbcpData_fbWidth * fbcpData_fbBytesPerPixel;
  // Check calculated display_info image size equal fixed buffer memory size
  if ((fbcpData_fbPitch * fbcpData_fbHeight) != fbcpData_finfo.smem_len) {
    dbgprintf(DBG_ERROR, "display_info image size does not match fixed buffer smem_len\n");
    close(fbcpData_fbfd);
    ret = vc_dispmanx_resource_delete(fbcpData_screen_resource);
    vc_dispmanx_display_close(fbcpData_display);
    return 0;
  }

  fbcpData_fbSize = fbcpData_finfo.smem_len;
  fbcpData_fbImagePtr = malloc(fbcpData_fbSize);
  if (fbcpData_fbImagePtr == NULL)
  {
    dbgprintf(DBG_ERROR, "unable to allocated image buffer\n");
    close(fbcpData_fbfd);
    ret = vc_dispmanx_resource_delete(fbcpData_screen_resource);
    vc_dispmanx_display_close(fbcpData_display);
    return 0;
  }
  fbcpData_tsImagePtr = malloc(fbcpData_fbSize);
  if (fbcpData_tsImagePtr == NULL)
  {
    dbgprintf(DBG_ERROR, "unable to allocated image buffer\n");
    close(fbcpData_fbfd);
    ret = vc_dispmanx_resource_delete(fbcpData_screen_resource);
    vc_dispmanx_display_close(fbcpData_display);
    return 0;
  }

  return 1;
}

int fbcp_start() {
  if (fbcpThreadRunning) {
    // debug_print("%s\n", "Not Starting FBCP Thread, Already Started");
    return 0;
  }
  fbcpThreadRunning = 1;

  if ((fbcpInit = fbcp_init()) == 0) {
    goto cleanup;
  }

  if ((fbcpInit = fbcp_alloc()) == 0) {
    goto cleanup;
  }

  // Startup vsync callback
  vc_dispmanx_vsync_callback(fbcpData_display, NULL, NULL);
  vc_dispmanx_vsync_callback(fbcpData_display, fbcp_vsyncCb, NULL);

  return 1;
 cleanup:
   fbcpThreadRunning = 0;
   return 0;
}


void fbcp_stop() {
  fbcpThreadRunning = 0;

  vc_dispmanx_vsync_callback(fbcpData_display, NULL, NULL);
/*
  free(fbcpData_fbImagePtr);
  fbcpData_fbImagePtr = NULL;
  free(fbcpData_tsImagePtr);
  fbcpData_tsImagePtr = NULL;

  munmap(fbcpData_fbp, fbcpData_finfo.smem_len);
  close(fbcpData_fbfd);
  vc_dispmanx_resource_delete(fbcpData_screen_resource);
  vc_dispmanx_display_close(fbcpData_display);
*/
}






PI_THREAD (fbcpThread)
{
  if (fbcpThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting FBCP Thread, Already Started");
    return NULL;
  }
  fbcpThreadRunning = 1;

  if ((fbcpInit = fbcp_init()) == 0) {
    goto cleanup;
  }

  if ((fbcpInit = fbcp_alloc()) == 0) {
    goto cleanup;
  }

  if (fbcpThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting FBCP, Stop Flag Set");
    fbcpThreadRunning = 0;
    return NULL;
  }

  dbgprintf(DBG_DEBUG, "%s\n", "Starting FBCP Thread");

  while (!fbcpThreadKill) {
    fbcp_copyFrame();
    if (!fbcpThreadKill) { usleep(16666); }
  }

 cleanup:
  dbgprintf(DBG_DEBUG, "%s\n", "Closing FBCP Thread");
  fbcpThreadRunning = 0;
  return NULL;
}


int fbcpThreadStart() {
  dbgprintf(DBG_DEBUG, "%s\n", "fbcpThreadStart()");
  if (fbcpThreadRunning) { return 0; }

  fbcpThreadKill = 0;
  return piThreadCreate(fbcpThread);
}

void fbcpThreadStop() {
  dbgprintf(DBG_DEBUG, "%s\n", "fbcpThreadStop()");
  // Shutdown MPV FIFO Thread
  if (fbcpThreadRunning) {
    fbcpThreadKill = 1;
    int shutdown_cnt = 0;
    while (fbcpThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    dbgprintf(DBG_DEBUG, "FBCP Thread Shutdown %d\n", shutdown_cnt);
  }
}

/*
fbcp.c License
-------

Copyright (c) 2013 Tasanakorn Phaipool
Copyright (c) 2020 Gary Taylor <gravity.addiction@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/