#ifndef _LIBUSB_DRIVES_THREAD_H_
#define _LIBUSB_DRIVES_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Queue up processing of usb drives
int libUsbDrivesThreadKill;
int libUsbDrivesThreadRunning;

struct queue_root *libUsbDrives_Queue;
size_t libUsbDrives_QueueLen;

int libUsbDrivesThreadStart();
void libUsbDrivesThreadStop();


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _LIBUSB_DRIVES_THREAD_H_