#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "dbg/dbg.h"

static dbgmask_t configured_mask = DBG_ERROR;

int dbg_enabled_p(dbgmask_t m) {
  return (m & configured_mask) != 0;
}

void init_dbg() {
  char* e = getenv("DBGMASK");
  if (e)
    configured_mask = strtoul(e, NULL, 0);
}

static void vasigprintf(char* fmt, va_list ap) {
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

void sigprintf(char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vasigprintf(fmt, ap);
  va_end(ap);
}

void dbgprintf(dbgmask_t m, char* fmt, ...) {
  if (dbg_enabled_p(m)) {
    va_list ap;
    va_start(ap, fmt);
    vasigprintf(fmt, ap);
    va_end(ap);
  }
}
