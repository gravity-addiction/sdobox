#ifndef DBG_H
#define DBG_H
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// DBG_ERROR is enabled by default
typedef unsigned long dbgmask_t;

#define DBG_ERROR     (1UL<<0)
#define DBG_DEBUG     (1UL<<1)
#define DBG_INFO      (1UL<<2)

#define DBG_MPV_WRITE (1UL<<3)
#define DBG_MPV_READ  (1UL<<4)
#define DBG_MPV_EVENT (1UL<<5)
#define DBG_QUEUE     (1UL<<6)
#define DBG_SLIDER    (1UL<<7)
#define DBG_VIDEOLIST (1UL<<8)
#define DBG_SVR       (1UL<<9)
#define DBG_WPA       (1UL<<10)

void init_dbg();

int16_t gui_debug_out(char x);

void dbgprintf(dbgmask_t fl, const char* fmt, ...);
void sigprintf(const char* fmt, ...); /* same as fprintf(stderr, fmt, ...), but safe from signal handler */

// Returns 1 if the given mask overlaps with the configured mask,
// 0 otherwise.
int dbg_enabled_p(dbgmask_t);

// From GUISlice, returns 0 for no output, 1 for errors, 2 for errors & debug info
int get_debug_err();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // QUEUE_H
