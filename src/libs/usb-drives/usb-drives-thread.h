#ifndef LIBUSBDRIVES_H_
#define LIBUSBDRIVES_H_

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
#endif // LIBUSBDRIVES_H_