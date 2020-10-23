
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "libs/shared.h"
#include "clock.h"

void clock_getTime(char **timeStr) {
  // Set time per loop
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  // size_t timeSz = snprintf(NULL, 0, "%02d/%02d/%d %02d:%02d:%02d", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec) + 1;
  // *timeStr = (char *)malloc(timeSz * sizeof(char));
  snprintf(*timeStr, 72, "%02d/%02d/%d %02d:%02d:%02d", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
  
}