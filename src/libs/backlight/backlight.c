
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "libs/shared.h"
#include "backlight.h"

void backlight_on() {
  digitalWrite(3, 1);
}
void backlight_off() {
  digitalWrite(3, 0);
}
void backlight_toggle() {
  if (digitalRead(3)) {
    backlight_off();
  } else {
    backlight_on();
  }
}
