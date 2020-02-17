#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "dbg/dbg.h"
#include "GUIslice/GUIslice.h"

static dbgmask_t configured_mask = DBG_ERROR;

int dbg_enabled_p(dbgmask_t m) {
  return (m & configured_mask) != 0;
}

int16_t gui_debug_out(char x) {
  write(STDERR_FILENO, &x, 1);
  return 0;
}

void init_dbg() {
  char* e = getenv("DBGMASK");
  if (e)
    configured_mask = strtoul(e, NULL, 0);

  gslc_InitDebug(gui_debug_out);
}

static void vasigprintf(const char* fmt, va_list ap) {
  // Do not use malloc from a signal handler
  const size_t MAXBUF = 256;
  char buf[MAXBUF];
  buf[MAXBUF-1] = '\0';
  int x = vsnprintf(buf, MAXBUF-1, fmt, ap);
  if (x > 0) {
    x = strlen(buf);
    write(STDERR_FILENO,buf,x);
  }
}

void sigprintf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vasigprintf(fmt, ap);
  va_end(ap);
}

void dbgprintf(dbgmask_t m, const char* fmt, ...) {
  if (dbg_enabled_p(m)) {
    va_list ap;
    va_start(ap, fmt);
    vasigprintf(fmt, ap);
    va_end(ap);
  }
}

int get_debug_err() {
  int result;
  if ((configured_mask & DBG_ERROR) == 0)
    result = 0;
  else if (configured_mask & (DBG_DEBUG|DBG_INFO))
    result = 2;
  else
    result = 1;
  // dbgprintf(DBG_ERROR, "get_debug_err(%s) ==> %d\n", fromwhere, result);
  return result;
}
