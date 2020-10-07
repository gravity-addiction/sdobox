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
  
  E_DUBBING_EL_MAX
};

#define MAX_ELEM_PG_DUBBING      E_DUBBING_EL_MAX
#define MAX_ELEM_PG_DUBBING_RAM  MAX_ELEM_PG_DUBBING


gslc_tsElem pg_dubbingElem[MAX_ELEM_PG_DUBBING];
gslc_tsElemRef pg_dubbingElemRef[MAX_ELEM_PG_DUBBING_RAM];

gslc_tsElemRef* pg_dubbingEl[E_DUBBING_EL_MAX];

int pg_dubbingSocketThreadKill;
int pg_dubbingSocketThreadRunning;

int pg_dubbing_fd;
int pg_dubbing_buf_size;
char* pg_dubbing_socket_path;


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