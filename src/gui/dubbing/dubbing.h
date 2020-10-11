#ifndef _DUBBING_H_
#define _DUBBING_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

//////////////////
// GUI Page
enum {
  E_DUBBING_EL_CLOSE,
  E_DUBBING_EL_SETSLATE,
  E_DUBBING_EL_STOPVIDEO,
  E_DUBBING_EL_SETEXIT,
  E_DUBBING_EL_PLAYPAUSEVIDEO,
  
  E_DUBBING_EL_USBDRIVE_A,
  E_DUBBING_EL_USBDRIVE_AA,
  E_DUBBING_EL_USBDRIVE_AB,
  E_DUBBING_EL_USBDRIVE_AC,

  E_DUBBING_EL_USBDRIVE_B,
  E_DUBBING_EL_USBDRIVE_BA,
  E_DUBBING_EL_USBDRIVE_BB,
  E_DUBBING_EL_USBDRIVE_BC,

  E_DUBBING_EL_USBDRIVE_C,
  E_DUBBING_EL_USBDRIVE_CA,
  E_DUBBING_EL_USBDRIVE_CB,
  E_DUBBING_EL_USBDRIVE_CC,

  E_DUBBING_EL_USBDRIVE_D,
  E_DUBBING_EL_USBDRIVE_DA,
  E_DUBBING_EL_USBDRIVE_DB,
  E_DUBBING_EL_USBDRIVE_DC,


  E_DUBBING_EL_MAX
};

#define MAX_ELEM_PG_DUBBING      E_DUBBING_EL_MAX
#define MAX_ELEM_PG_DUBBING_RAM  MAX_ELEM_PG_DUBBING

gslc_tsElem pg_dubbingElem[MAX_ELEM_PG_DUBBING];
gslc_tsElemRef pg_dubbingElemRef[MAX_ELEM_PG_DUBBING_RAM];

gslc_tsElemRef* pg_dubbingEl[E_DUBBING_EL_MAX];

double pg_dubbing_videoSlate;
double pg_dubbing_videoExit;
int pg_dubbing_libUsbCnt;
int pg_dubbing_libMpvCnt;
int pg_dubbing_driveI_A;
int pg_dubbing_driveI_B;
int pg_dubbing_driveI_C;
int pg_dubbing_driveI_D;

void pg_dubbingButtonRotaryCW();
void pg_dubbingButtonRotaryCCW();
void pg_dubbingButtonLeftPressed();
void pg_dubbingButtonRightPressed();
void pg_dubbingButtonRotaryPressed();
void pg_dubbingButtonLeftHeld();
void pg_dubbingButtonRightHeld();
void pg_dubbingButtonSetFuncs();
void pg_dubbingButtonUnsetFuncs();

void pg_dubbing_init(gslc_tsGui *pGui);
void pg_dubbing_open(gslc_tsGui *pGui);
int pg_dubbing_thread(gslc_tsGui *pGui);
void pg_dubbing_destroy(gslc_tsGui *pGui);

void __attribute__ ((constructor)) pg_dubbing_constructor(void);
void __attribute__ ((destructor)) pg_dubbing_destructor(void);

#endif // _DUBBING_H_