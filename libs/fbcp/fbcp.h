#ifndef _FBCP_H_
#define _FBCP_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <bcm_host.h>

DISPMANX_DISPLAY_HANDLE_T fbcpData_display;
DISPMANX_MODEINFO_T fbcpData_display_info;
DISPMANX_RESOURCE_HANDLE_T fbcpData_screen_resource;
VC_RECT_T fbcpData_rect1;
char *fbcpData_fbp;
int32_t fbcpData_fbWidth;
int32_t fbcpData_fbHeight;
int32_t fbcpData_fbBytesPerPixel;
int32_t fbcpData_fbPitch;
size_t fbcpData_fbSize;
void *fbcpData_fbImagePtr;
void *fbcpData_tsImagePtr;

// Thread
int fbcpInit;
int fbcpThreadKill;
int fbcpThreadRunning;
int fbcpStartSkip;


void fbcp_copyFrame();
void fbcp_vsyncCb(DISPMANX_UPDATE_HANDLE_T u, void* arg);
int fbcp_toggle();
int fbcp_init();
int fbcp_alloc();
int fbcp_start();
void fbcp_stop();
int fbcpThreadStart();
void fbcpThreadStop();


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _FBCP_H_

/*
fbcp.h License
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