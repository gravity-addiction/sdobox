
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "libs/dbg/dbg.h"
#include "libs/json/json.h"
#include "libs/shared.h"
#include "usb-drives.h"

struct libUsbDrivesCounter *ALLOC_LIBUSBDRIVES_COUNTER()
{
  struct libUsbDrivesCounter *root = (struct libUsbDrivesCounter*)malloc(sizeof(struct libUsbDrivesCounter));
  root->max = 5;
  root->cur = 0;
  root->cnt = 0;
  return root;
}

struct libUsbDrivesHardware *ALLOC_LIBUSBDRIVES_HARDWARE()
{
  struct libUsbDrivesHardware *root = (struct libUsbDrivesHardware*)malloc(sizeof(struct libUsbDrivesHardware));
  root->partitionCur = -1;
  root->partitionMax = 0;
  return root;
}

struct libUsbDrivesInfo *ALLOC_LIBUSBDRIVES_INFO()
{
  struct libUsbDrivesInfo *root = malloc(sizeof(struct libUsbDrivesInfo));
  return root;
}

void libUsbDrives_cleanPartition(struct libUsbDrivesInfo *partition) {
  if (partition == NULL) { return; }

  if (partition->name != NULL) { free(partition->name); }
  if (partition->label != NULL) { free(partition->label); }
  if (partition->mountpoint != NULL) { free(partition->mountpoint); }
  if (partition->fstype != NULL) { free(partition->fstype); }
  if (partition->drivesize != NULL) { free(partition->drivesize); }
}

void libUsbDrives_cleanPartitions(struct libUsbDrivesInfo **partitionList, int partitionCount) {
  for (int p = 0; p < partitionCount; p++) {
    if (partitionList[p] != NULL) {
      libUsbDrives_cleanPartition(partitionList[p]);
    }
  }
}

void libUsbDrives_cleanDriveList(struct libUsbDrivesHardware **driveList, struct libUsbDrivesCounter *driveCounter) {
  for (int l = 0; l < driveCounter->cur; ++l) {
    if (driveList[l]->name != NULL) { free(driveList[l]->name); }
    if (driveList[l]->drivesize != NULL) { free(driveList[l]->drivesize); }

    for (int p = 0; p < driveList[l]->partitionMax; ++p) {
      libUsbDrives_cleanPartition(driveList[l]->partitions[p]);
      free(driveList[l]->partitions[p]);
    }
    free(driveList[l]->partitions);
    free(driveList[l]);
  }
  driveCounter->cur = 0;
}


int libUsbDrives_findHardware(char *name, struct libUsbDrivesHardware *driveList) {
  return -1;
}


int libUsbDrives_parse_lsblk(char *libSdobSocket_buf, struct libUsbDrivesHardware **driveList, struct libUsbDrivesCounter *driveCounter) {
  char* sdob_data;
  int sdob_data_len = ta_json_parse(libSdobSocket_buf, "blockdevices", &sdob_data);
  if (sdob_data_len > 0) {
    int r;
    int tI = 0;
    jsmn_parser p;
    jsmntok_t t[128]; // We expect no more than 128 tokens

    jsmn_init(&p);
    r = jsmn_parse(&p, sdob_data, strlen(sdob_data), t, sizeof(t) / sizeof(t[tI]));
    if (r < 0) {
      dbgprintf(DBG_DEBUG, "Failed to parse JSON: %d\n", r);
      return 1;
    }

    // Assume the top-level element is an object
    if (r < 1 || t[tI].type != JSMN_ARRAY) {
      dbgprintf(DBG_DEBUG, "%s\n", "Array expected");
      return 2;
    }

    dbgprintf(DBG_DEBUG, "USB Devices: %d\n", t[0].size);

    int deviceLen = t[0].size;
    if (deviceLen > driveCounter->max) { deviceLen = driveCounter->max; }

    for (int device = 0; device < deviceLen; ++device) { // Loop Hardware Devices
      tI++;
      jsmntok_t *deviceInfo = &t[tI];
      int deviceInfoLen = deviceInfo->size;
      driveList[device] = ALLOC_LIBUSBDRIVES_HARDWARE();

      /*
      // Malloc or cleanu partitions space
      if (driveList[device]->partitionMax == 0) {
        driveList[device]->partitionMax = 5;
        driveList[device]->partitions = (struct libUsbDrivesInfo**)malloc(sizeof(struct libUsbDrivesInfo*) * driveList[device]->partitionMax);

        for (int pd = 0; pd < driveList[device]->partitionMax; pd++) {
          driveList[device]->partitions[pd] = (struct libUsbDrivesInfo*)malloc(sizeof(struct libUsbDrivesInfo));
        }
      } else {
        // Cleanup
        // libUsbDrives_cleanPartitions(driveList[device]->partitions, driveList[device]->partitionCur);
      }
      driveList[device]->partitionCur = 0;
      */

      for (int deviceInfoI = 0; deviceInfoI < deviceInfoLen; deviceInfoI++) { // Loop Properties Of Hardware
        tI++;
        jsmntok_t *deviceInfoT = &t[tI];

        if (deviceInfoT->size == 1) { // Key token, has single child
          // Capture Overall Device Info
          tI++;
          jsmntok_t *deviceInfoTVal = &t[tI]; // Capture Key Value
          int vKeyLen = (deviceInfoT->end - deviceInfoT->start) + 1;
          char *vKey = malloc(vKeyLen + 1);
          snprintf(vKey, vKeyLen, "%.*s", deviceInfoT->end - deviceInfoT->start, sdob_data + deviceInfoT->start);

          int vValLen = (deviceInfoTVal->end - deviceInfoTVal->start) + 1;
          char *vVal = malloc(vValLen + 1);
          snprintf(vVal, vValLen, "%.*s", deviceInfoTVal->end - deviceInfoTVal->start, sdob_data + deviceInfoTVal->start);

          if (strcmp(vKey, "name") == 0 && vValLen) {
            driveList[device]->name = vVal;
          } else if (strcmp(vKey, "size") == 0 && vValLen) {
            driveList[device]->drivesize = vVal;
          } else {
            free(vVal);
          }
          free(vKey);
          // printf("[%d] %.*s - %.*s\n", deviceInfoT->type, deviceInfoT->end - deviceInfoT->start, sdob_data + deviceInfoT->start, deviceInfoTVal->end - deviceInfoTVal->start, sdob_data + deviceInfoTVal->start);
        } else {
          dbgprintf(DBG_DEBUG, "Unknown Token Size [%d] %d\n", deviceInfoT->type, deviceInfoT->size);
        }
        
        if (jsoneq(sdob_data, deviceInfoT, "children") == 0) {
          jsmntok_t *deviceChildren = &t[tI];
          // printf("Found Children! %d\n", deviceChildren->size);

          int deviceChildrenLen = deviceChildren->size;
          if (deviceChildrenLen > 0) {
            
            driveList[device]->partitionMax = deviceChildrenLen;
            driveList[device]->partitions = (struct libUsbDrivesInfo**)malloc(sizeof(struct libUsbDrivesInfo*) * deviceChildrenLen);
            
            for (int deviceChildrenI = 0; deviceChildrenI < deviceChildrenLen; deviceChildrenI++) {
              tI++;
              jsmntok_t *deviceChildInfo = &t[tI];
              int deviceChildInfoLen = deviceChildInfo->size;
              driveList[device]->partitions[deviceChildrenI] = ALLOC_LIBUSBDRIVES_INFO();

              for (int deviceChildInfoI = 0; deviceChildInfoI < deviceChildInfoLen; deviceChildInfoI++) {
                tI++;
                jsmntok_t *deviceChildInfoT = &t[tI];
                if (deviceChildInfoT->size == 1) {
                  // Capture Individual Partion Info
                  tI++;
                  jsmntok_t *deviceChildInfoTVal = &t[tI];

                  int vcKeyLen = (deviceChildInfoT->end - deviceChildInfoT->start) + 1;
                  char *vcKey = malloc(vcKeyLen + 1);
                  snprintf(vcKey, vcKeyLen, "%.*s", deviceChildInfoT->end - deviceChildInfoT->start, sdob_data + deviceChildInfoT->start);

                  int vcValLen = (deviceChildInfoTVal->end - deviceChildInfoTVal->start) + 1;
                  char *vcVal = malloc(vcValLen + 1);
                  snprintf(vcVal, vcValLen, "%.*s", deviceChildInfoTVal->end - deviceChildInfoTVal->start, sdob_data + deviceChildInfoTVal->start);


                  if (strcmp(vcKey, "name") == 0 && vcValLen > 0) {
                    driveList[device]->partitions[deviceChildrenI]->name = vcVal;
                  } else if (strcmp(vcKey, "label") == 0 && vcValLen > 0) {
                    driveList[device]->partitions[deviceChildrenI]->label = vcVal;
                  } else if (strcmp(vcKey, "mountpoint") == 0 && vcValLen > 0) {
                    driveList[device]->partitions[deviceChildrenI]->mountpoint = vcVal;
                  } else if (strcmp(vcKey, "fstype") == 0 && vcValLen > 0) {
                    driveList[device]->partitions[deviceChildrenI]->fstype = vcVal;
                  } else if (strcmp(vcKey, "size") == 0 && vcValLen > 0) {
                    driveList[device]->partitions[deviceChildrenI]->drivesize = vcVal;
                  } else {
                    free(vcVal);
                  }
                  free(vcKey);
                  // printf("[%d] %.*s - %.*s\n", deviceChildInfoT->type, deviceChildInfoT->end - deviceChildInfoT->start, sdob_data + deviceChildInfoT->start, deviceChildInfoTVal->end - deviceChildInfoTVal->start, sdob_data + deviceChildInfoTVal->start);
                } else {
                  dbgprintf(DBG_DEBUG, "Unknown Token Size %d\n", deviceChildInfoT->size);
                }
              }

            }
          }
        }
      }
    }
    driveCounter->cur = deviceLen;
    driveCounter->cnt++;
  }
  free(sdob_data);
  return 0;
}

/*
[
  {
    "name":"sda",
    "label":"Ubuntu 11.10 amd64",
    "fstype":"iso9660",
    "size":"1.9G",
    "mountpoint":null,
    "children": [
      {
        "name":"sda1",
        "label":"NEW VOLUME",
        "fstype":"vfat",
        "size":"1.9G",
        "mountpoint":null
      }
    ]
  },
  {
    "name":"mmcblk0",
    "label":null,
    "fstype":null,
    "size":"59.5G",
    "mountpoint":null,
    "children": [
      {
        "name":"mmcblk0p1",
        "label":"boot",
        "fstype":"vfat",
        "size":"256M",
        "mountpoint":"/boot"
      },
      {
        "name":"mmcblk0p2",
        "label":"rootfs",
        "fstype":"ext4",
        "size":"59.2G",
        "mountpoint":"/"
      }
    ]
  }
]
*/
/*
[0
  {1
    "2":"3",
    "4":"5",
    "6":"7",
    "8":"9",
    "10":11,
    "12": [13
      {14
        "15":"16",
        "17":"18",
        "19":"20",
        "21":"22",
        "23":24
      }
    ]
  },
  {25
    "26":"27",
    "28":29,
    "30":31,
    "32":"33",
    "34":35,
    "36": [37
      {38
        "39":"40",
        "41":"42",
        "43":"44",
        "45":"46",
        "47":"48"
      },
      {49
        "50":"51",
        "52":"53",
        "54":"55",
        "56":"57",
        "58":"59"
      }
    ]
  }
]
*/