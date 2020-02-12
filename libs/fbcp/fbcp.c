
#include <stdio.h>
#include <syslog.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include <bcm_host.h>

#include "fbcp.h"
#include <wiringPi.h> // Gordons Wiring Pi

// ------------------------
// FBCP Thread
// ------------------------
PI_THREAD (fbcpThread)
{
  if (fbcpThreadRunning) {
    // debug_print("%s\n", "Not Starting FBCP Thread, Already Started");
    return NULL;
  }
  fbcpThreadRunning = 1;

  if (fbcpThreadKill) {
    // debug_print("%s\n", "Not Starting FBCP Thread, Stop Flag Set");
    fbcpThreadRunning = 0;
    return NULL;
  }

  // debug_print("%s\n", "Starting FBCP Thread");

/*
License
-------


Copyright (c) 2013 Tasanakorn Phaipool

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

  DISPMANX_DISPLAY_HANDLE_T display;
  DISPMANX_MODEINFO_T display_info;
  DISPMANX_RESOURCE_HANDLE_T screen_resource;
  // VC_IMAGE_TRANSFORM_T transform;
  uint32_t image_prt;
  VC_RECT_T rect1;
  int ret;
  int fbfd = 0;
  char *fbp = 0;

  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;

  bcm_host_init();

  display = vc_dispmanx_display_open(0);
  if (!display) {
      syslog(LOG_ERR, "Unable to open primary display");
      return NULL;
  }
  ret = vc_dispmanx_display_get_info(display, &display_info);
  if (ret) {
      syslog(LOG_ERR, "Unable to get primary display information");
      return NULL;
  }
  syslog(LOG_INFO, "Primary display is %d x %d", display_info.width, display_info.height);


  fbfd = open("/dev/fb1", O_RDWR);
  if (fbfd == -1) {
      syslog(LOG_ERR, "Unable to open secondary display");
      return NULL;
  }
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
      syslog(LOG_ERR, "Unable to get secondary display information");
      return NULL;
  }
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
      syslog(LOG_ERR, "Unable to get secondary display information");
      return NULL;
  }

  syslog(LOG_INFO, "Second display is %d x %d %dbps\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

  screen_resource = vc_dispmanx_resource_create(VC_IMAGE_RGB565, vinfo.xres, vinfo.yres, &image_prt);
  if (!screen_resource) {
      syslog(LOG_ERR, "Unable to create screen buffer");
      close(fbfd);
      vc_dispmanx_display_close(display);
      return NULL;
  }

  fbp = (char*) mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  if (fbp <= 0) {
      syslog(LOG_ERR, "Unable to create memory mapping");
      close(fbfd);
      ret = vc_dispmanx_resource_delete(screen_resource);
      vc_dispmanx_display_close(display);
      return NULL;
  }

  vc_dispmanx_rect_set(&rect1, 0, 0, vinfo.xres, vinfo.yres);

  while (!fbcpThreadKill) {
      ret = vc_dispmanx_snapshot(display, screen_resource, 0);
      vc_dispmanx_resource_read_data(screen_resource, &rect1, fbp, vinfo.xres * vinfo.bits_per_pixel / 8);
      
      usleep(16666); // double desired framerate (1 / 60) * 1000000 
  }

  munmap(fbp, finfo.smem_len);
  close(fbfd);
  ret = vc_dispmanx_resource_delete(screen_resource);
  vc_dispmanx_display_close(display);


  // printf("%s\n", "Closing FBCP Thread");
  fbcpThreadRunning = 0;
  return NULL;
}

int fbcp_toggle() {
  if (fbcpThreadRunning) {
    fbcp_stop();
    return 0;
  } else {
    fbcp_start();
    return 1;
  }
}

int fbcp_start() {
  // printf("%s\n", "fbcpThreadStart()");
  if (fbcpThreadRunning) { return 0; }

  fbcpThreadKill = 0;
  return piThreadCreate(fbcpThread);
}

void fbcp_stop() {
  // printf("%s\n", "fbcpThreadStop()");
  if (fbcpThreadRunning && !fbcpThreadKill) {
    fbcpThreadKill = 1;
    int shutdown_cnt = 0;
    while (fbcpThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    fbcpThreadKill = 0;
    // printf("FBCP Thread Shutdown %d\n", shutdown_cnt);
  }
}
