#ifndef _USBDRIVE_H_
#define _USBDRIVE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "libs/usb-drives/usb-drives.h"
#include "gui/pages.h"

//////////////////
// GUI Page
enum {
  E_USBDRIVE_EL_CLOSE,
  E_USBDRIVE_EL_EJECT,
  E_USBDRIVE_EL_EJECT_TXTA,
  E_USBDRIVE_EL_POWEROFF,
  E_USBDRIVE_EL_ACTION,

  E_USBDRIVE_EL_BOX,

  E_USBDRIVE_EL_MAX
};

#define MAX_ELEM_PG_USBDRIVE      E_USBDRIVE_EL_MAX + 11
#define MAX_ELEM_PG_USBDRIVE_RAM  MAX_ELEM_PG_USBDRIVE

gslc_tsElem pg_usbdriveElem[MAX_ELEM_PG_USBDRIVE];
gslc_tsElemRef pg_usbdriveElemRef[MAX_ELEM_PG_USBDRIVE_RAM];

gslc_tsElemRef* pg_usbdriveEl[E_USBDRIVE_EL_MAX];

void pg_usbdriveButtonRotaryCW();
void pg_usbdriveButtonRotaryCCW();
void pg_usbdriveButtonLeftPressed();
void pg_usbdriveButtonRightPressed();
void pg_usbdriveButtonRotaryPressed();
void pg_usbdriveButtonLeftHeld();
void pg_usbdriveButtonRightHeld();
void pg_usbdriveButtonSetFuncs();
void pg_usbdriveButtonUnsetFuncs();

void pg_usbdrive_init(gslc_tsGui *pGui);
void pg_usbdrive_open(gslc_tsGui *pGui);
int pg_usbdrive_thread(gslc_tsGui *pGui);
void pg_usbdrive_destroy(gslc_tsGui *pGui);

void __attribute__ ((constructor)) pg_usbdrive_constructor(void);
void __attribute__ ((destructor)) pg_usbdrive_destructor(void);

int libUsbDrivesI;

void pg_usbdrive_loadDrive(int driveI);
struct vlist_config *pg_usbDrive_listConfig;
gslc_tsXSlider pg_usbDrive_listSlider;

#ifdef __cplusplus
}
#endif

#endif // _USBDRIVE_H_