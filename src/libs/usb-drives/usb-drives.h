#ifndef _USB_DRIVES_H_
#define _USB_DRIVES_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct libUsbDrivesCounter *ALLOC_LIBUSBDRIVES_COUNTER();
struct libUsbDrivesHardware *ALLOC_LIBUSBDRIVES_HARDWARE();
struct libUsbDrivesInfo *ALLOC_LIBUSBDRIVES_INFO();

struct libUsbDrivesInfo {
  char *name;
  char *label;
  char *mountpoint;
  char *fstype;
  char *drivesize;
};

struct libUsbDrivesHardware {
  char *name;
  char *drivesize;
  int partitionCur;
  int partitionMax;
  struct libUsbDrivesInfo **partitions;
};
struct libUsbDrivesHardware **libUsbDrivesList;

struct libUsbDrivesCounter {
  int max;
  int cur;
  int cnt;
};
struct libUsbDrivesCounter *libUsbDrivesCount;

void libUsbDrives_cleanPartition(struct libUsbDrivesInfo *partition);
void libUsbDrives_cleanPartitions(struct libUsbDrivesInfo **partitionList, int partitionMax);
void libUsbDrives_cleanDriveList(struct libUsbDrivesHardware **driveList, struct libUsbDrivesCounter *driveCounter);
int libUsbDrives_findHardware(char *name, struct libUsbDrivesHardware *driveList);
int libUsbDrives_parse_lsblk(char *libSdobSocket_buf, struct libUsbDrivesHardware **driveList, struct libUsbDrivesCounter *driveCounter);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _USB_DRIVES_H_