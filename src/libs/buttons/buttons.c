
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "GUIslice.h"
#include "libs/buttons/buttons.h"
#include "libs/dbg/dbg.h"
#include "libs/queue/queue.h"
#include "libs/thpool/thpool.h"

threadpool buttons_thpool = NULL;

void lib_buttons_searchGPIO(const int whitelistPinsSize, const int *whitelistedPins, int pinCache[]) {
  for (int p = 0; p < whitelistPinsSize; p++) {
    dbgprintf(DBG_INFO, "Check Pin: %d\n", whitelistedPins[p]);
    pinCache[p] = digitalRead(whitelistedPins[p]);
  }
}

int lib_buttons_findGPIO(int i_timeout, int isleep) { // timeout in milliseconds to search, i_sleep wait between checks
  const int whitelistPinsSize = 17;
  // Removed 7 due to ups boards
  const int whitelistedPins[] = { 2, 3, 4, 8, 9, 15, 16, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
  int pinLast[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
  int pinCache[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
  int pinFound = -2; // -2 for nothing found, -1 will flag timeout
  int i_now = millis();
  int ii_now = millis();
  int i_diff = 0;
  int i_sleep = isleep * 1000;

  lib_buttons_searchGPIO(whitelistPinsSize, whitelistedPins, pinCache);
  for (int p = 0; p < whitelistPinsSize; p++) {
    pinLast[p] = pinCache[p];
  }

  while (pinFound < -1) {
    usleep(i_sleep);
    lib_buttons_searchGPIO(whitelistPinsSize, whitelistedPins, pinCache);
    for (int p = 0; p < whitelistPinsSize; p++) {
      if (pinLast[p] != pinCache[p]) {
        pinFound = whitelistedPins[p];
        break;
      }
    }

    // Check for Timeout
    ii_now = millis();
    i_diff = ii_now - i_now;
    if ( i_diff > i_timeout) { 
      pinFound = -1;
    }
  }

  return pinFound;
}


void lib_buttons_waitRelease(int pin) {
  int pinState = digitalRead(pin);
  while (pinState == 0) {
    usleep(50000);
    pinState = digitalRead(pin);
  }
}



int lib_buttons_saveConfig(char* config_path, int leftBtn, int rightBtn, int rotaryBtn, int rotaryA, int rotaryB) {
  // Check config path folder exists
  char* dir = strdup(config_path);
  char* dirn = dirname(dir);
  if (dirn != NULL) {
    mkdir(dirn, 0700);

    // Writing to Config File
    FILE *fptr;
    fptr = fopen(config_path, "w");
    if (fptr == NULL) {
      return 0;
    }

    fprintf(fptr, "rotary_pin_a = %d;\nrotary_pin_b = %d;\nrotary_pin_btn = %d;\nright_pin_btn = %d;\nleft_pin_btn = %d;\ndebounce_delay = 175;\nbtn_hold_delay = 800;\n", rotaryA, rotaryB, rotaryBtn, rightBtn, leftBtn);
    fclose(fptr);

    return 1;
  }

  return 0;
}

int lib_buttons_configure(char* config_path) {
  if (wiringPiSetup () == -1) {
    dbgprintf(DBG_DEBUG, "Failed Initializing Pi Wiring\n");
    return 0;
  }

  int leftBtn, rightBtn, rotaryBtn, rotaryA, rotaryB = -1;

  dbgprintf(DBG_DEBUG, "Press Left Button\n");
  leftBtn = lib_buttons_findGPIO(10000, 100);
  dbgprintf(DBG_DEBUG, "Left Button Found! %d\n", leftBtn);
  dbgprintf(DBG_DEBUG, "Press Right Button\n");
  lib_buttons_waitRelease(leftBtn);
  rightBtn = lib_buttons_findGPIO(10000, 100);
  dbgprintf(DBG_DEBUG, "Right Button Found! %d\n", rightBtn);
  dbgprintf(DBG_DEBUG, "Press Directly Down On Rotary Button\n");
  lib_buttons_waitRelease(rightBtn);
  rotaryBtn = lib_buttons_findGPIO(10000, 100);
  dbgprintf(DBG_DEBUG, "Rotary Button Found! %d\n", rotaryBtn);
  dbgprintf(DBG_DEBUG, "Spin Rotary Knob Clockwise Until Detected\n");
  lib_buttons_waitRelease(rotaryBtn);
  rotaryA = lib_buttons_findGPIO(10000, 50);
  rotaryB = lib_buttons_findGPIO(10000, 50);

  int i_now = millis();
  int ii_now = millis();
  int i_diff = 0;
  while (rotaryA == rotaryB) {
    rotaryB = lib_buttons_findGPIO(10000, 50);
    // Check for Timeout
    ii_now = millis();
    i_diff = ii_now - i_now;
    if ( i_diff > 20000) { 
      rotaryB = -1;
    }
  }
  if (rotaryB == -1) {
    dbgprintf(DBG_DEBUG, "Rotary Knob NOT Found, 20second Timeout!\n");
  } else {
    dbgprintf(DBG_DEBUG, "Rotary Knob Found! %d %d\n", rotaryA, rotaryB);
  }

  return lib_buttons_saveConfig(config_path, leftBtn, rightBtn, rotaryBtn, rotaryA, rotaryB);
}



int lib_buttons_thread() {
  if (lib_buttonsDisabled) { return 0; } // Check Buttons Disabled

  int i_now = millis();

  // Check Left Press and Held
  // int lPin = digitalRead(LEFT_PIN_BTN);
  // int rPin = digitalRead(RIGHT_PIN_BTN);
  // int kPin = digitalRead(ROTARY_PIN_BTN);

  if (lib_buttonsLastInterruptTime[E_BUTTON_LEFT_UP] > 0) {
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] = 0;
    lib_buttonsLastInterruptAction[E_BUTTON_DOUBLE_HELD] = 0;
  }
  if (lib_buttonsLastInterruptTime[E_BUTTON_RIGHT_UP] > 0) {
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] = 0;
    lib_buttonsLastInterruptAction[E_BUTTON_DOUBLE_HELD] = 0;
  }

  // Check Left Held
  if (
    lib_buttonsLastInterruptTime[E_BUTTON_LEFT_DOWN] > 0
    && lib_buttonsLastInterruptTime[E_BUTTON_LEFT_DOWN] < (i_now - lib_buttonsBtnHoldDelay)
    && lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] == 0
    && cbBtns[E_BUTTON_LEFT_HELD]
  ) {
    ++lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD];
    dbgprintf(DBG_INFO, "%s\n", "Execute Left Held");
    // Execute CB Left Held Function
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] += cbBtns[E_BUTTON_LEFT_HELD]();
    // thpool_add_work(buttons_thpool, cbBtns[E_BUTTON_LEFT_HELD], NULL);
  }

  // Check Right Held
  if (
    lib_buttonsLastInterruptTime[E_BUTTON_RIGHT_DOWN] > 0
    && lib_buttonsLastInterruptTime[E_BUTTON_RIGHT_DOWN] < (i_now - lib_buttonsBtnHoldDelay)
    && lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] == 0
    && cbBtns[E_BUTTON_RIGHT_HELD]
  ) {
    ++lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD];
    dbgprintf(DBG_INFO, "%s\n", "Execute Right Held");
    // Execute CB
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] += cbBtns[E_BUTTON_RIGHT_HELD]();
    // thpool_add_work(buttons_thpool, cbBtns[E_BUTTON_RIGHT_HELD], NULL);
  }

  // Check Rotary Held
  if (
    lib_buttonsLastInterruptTime[E_BUTTON_ROTARY_DOWN] > 0
    && lib_buttonsLastInterruptTime[E_BUTTON_ROTARY_DOWN] < (i_now - lib_buttonsBtnHoldDelay)
    && lib_buttonsLastInterruptAction[E_BUTTON_ROTARY_HELD] == 0
    && cbBtns[E_BUTTON_ROTARY_HELD]
  ) {
    ++lib_buttonsLastInterruptAction[E_BUTTON_ROTARY_HELD];
    dbgprintf(DBG_INFO, "%s\n", "Execute Rotary Held");
    // Execute CB
    lib_buttonsLastInterruptAction[E_BUTTON_ROTARY_HELD] += cbBtns[E_BUTTON_ROTARY_HELD]();
    
    // thpool_add_work(buttons_thpool, cbBtns[E_BUTTON_ROTARY_HELD], NULL);
  }

  // Dbl Button Press Held Found
  if (
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] > 0
    && lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] > 0
    && lib_buttonsLastInterruptAction[E_BUTTON_DOUBLE_HELD] == 0
    && cbBtns[E_BUTTON_DOUBLE_HELD]
  ) {
    // ++lib_buttonsLastInterruptAction[E_BUTTON_DOUBLE_HELD];
    dbgprintf(DBG_INFO, "%s\n", "Execute Double Held");
    lib_buttonsLastInterruptAction[E_BUTTON_DOUBLE_HELD] = cbBtns[E_BUTTON_DOUBLE_HELD]();
    // thpool_add_work(buttons_thpool, cbBtns[E_BUTTON_DOUBLE_HELD], NULL);
  }

  return 1;
}



int lib_buttons_check(void* input) {
  while(!lib_buttonsThreadKill) {
    lib_buttons_thread();
    usleep(1000000);
  }
  return 1;
}





// Button Initialized
void lib_buttons_init() {
  // placeholders for the rotary knob, determines which direction the knob was turned
  // lib_buttonsBtnDebounceDelay = 0;
  // lib_buttonsBtnHoldDelay = 1500;

  lib_buttonsThreadKill = 0; // Stopping mpvRpcSocket
  lib_buttonsThreadRunning = 0; // Running flag for Mpv RPC

  for (size_t b = 0; b < MAX_BUTTON_STATES; b++) {
    lib_buttonsLastInterruptTime[b] = 0;
  }
  for (size_t a = 0; a < MAX_BUTTONS; a++) {
    lib_buttonsLastInterruptAction[a] = 0;
  }

  // Initialize All Button Interrupts, split into threads for queue
  if (wiringPiSetup () == -1) {
    // debug_print("%s\n", "Failed Initializing Pi Wiring");
  }

  if ( ROTARY_PIN_A > -1 && ROTARY_PIN_B > -1 && wiringPiISR (ROTARY_PIN_A, INT_EDGE_BOTH, &lib_buttonsRotaryInterrupt) < 0 ) {
    // debug_print("Unable to setup Rotary Pin A ISR: %s\n", strerror(errno));
  }
  if ( ROTARY_PIN_A > -1 && ROTARY_PIN_B > -1 && wiringPiISR (ROTARY_PIN_B, INT_EDGE_BOTH, &lib_buttonsRotaryInterrupt) < 0 ) {
    // debug_print("Unable to setup Rotary Pin B ISR: %s\n", strerror(errno));
  }

  if (ROTARY_PIN_BTN > -1) {
    if (wiringPiISR (ROTARY_PIN_BTN, INT_EDGE_BOTH, &lib_buttonsRotaryBtnInterrupt) < 0 ) {
      // debug_print("Unable to setup Left Btn ISR: %s\n", strerror(errno));
    } else {
      if (digitalRead(ROTARY_PIN_BTN) == 1) {
        dbgprintf(DBG_INFO, "%s\n", "Set Rotary Up");
        lib_buttonsLastInterruptTime[E_BUTTON_ROTARY_UP] = millis();
      } else {
        dbgprintf(DBG_INFO, "%s\n", "Set Rotary Down");
        lib_buttonsLastInterruptTime[E_BUTTON_ROTARY_DOWN] = millis();
      }
    }
  }

  if (RIGHT_PIN_BTN > -1) {
    if (wiringPiISR (RIGHT_PIN_BTN, INT_EDGE_BOTH, &lib_buttonsRightBtnInterrupt) < 0 ) {
      // debug_print("Unable to setup Left Btn ISR: %s\n", strerror(errno));
    } else {
      if (digitalRead(RIGHT_PIN_BTN) == 1) {
        dbgprintf(DBG_INFO, "%s\n", "Set Right Up");
        lib_buttonsLastInterruptTime[E_BUTTON_RIGHT_UP] = millis();
      } else {
        dbgprintf(DBG_INFO, "%s\n", "Set Right Down");
        lib_buttonsLastInterruptTime[E_BUTTON_RIGHT_DOWN] = millis();
      }
    }
  }
  if (LEFT_PIN_BTN > -1) {
    if (wiringPiISR (LEFT_PIN_BTN, INT_EDGE_BOTH, &lib_buttonsLeftBtnInterrupt) < 0 ) {
      // debug_print("Unable to setup Left Btn ISR: %s\n", strerror(errno));
    } else {
      if (digitalRead(LEFT_PIN_BTN) == 1) {
        dbgprintf(DBG_INFO, "%s\n", "Set Left Up");
        lib_buttonsLastInterruptTime[E_BUTTON_LEFT_UP] = millis();
      } else {
        dbgprintf(DBG_INFO, "%s\n", "Set Left Down");
        lib_buttonsLastInterruptTime[E_BUTTON_LEFT_DOWN] = millis();
      }
    }
  }

  buttons_thpool = thpool_init(8);
  
  thpool_add_work(buttons_thpool, &lib_buttons_check, NULL);
}
/*
int lib_buttonsThreadStart() {
  return piThreadCreate(buttonsEventThread);
}

void lib_buttonsThreadStop() {
  if (lib_buttonsThreadRunning) {
    lib_buttonsThreadKill = 1;
    int shutdown_cnt = 0;
    while (lib_buttonsThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    // debug_print("Buttons Thread Shutdown %d\n", shutdown_cnt);
  }
}
*/





// Set callback for button actions
void lib_buttonsSetCallbackFunc(int btn, int (*function)()) {
  cbBtns[btn] = function;
  lib_buttonsLastInterruptAction[btn] = 0;
  return;
}


// Overall Button Management Routine
int lib_buttonsManageBtnInterrupt(int pin, int timeUp, int timeDown, \
                int actionPressed, int actionReleased, int actionHeld
) {
  if (lib_buttonsDisabled) { return 0; } // Check Buttons Disabled

  unsigned int i_now = millis();

  int rc = 1;
  int p = digitalRead(pin);

  // printf("%d - %d - %d\n", i_now, lib_buttonsLastInterruptTime[timeUp], lib_buttonsLastInterruptTime[timeDown]);
  // printf("%d, Pin: %d - %d - Delay: %d, Relased: %d, Pressed: %d\n", i_now, pin, p, lib_buttonsBtnDebounceDelay, lib_buttonsLastInterruptAction[actionReleased], lib_buttonsLastInterruptAction[actionPressed]);
  // printf("Got p: %d ", p);



  if (
    p == 1
    && lib_buttonsLastInterruptTime[timeDown] > 0 // Ensure it was Down at some point
    && (lib_buttonsLastInterruptTime[timeDown] + 50) < i_now // Jitter
    && lib_buttonsLastInterruptTime[timeUp] < (i_now - lib_buttonsBtnDebounceDelay)
  ) {
    lib_buttonsLastInterruptTime[timeUp] = i_now;
    lib_buttonsLastInterruptTime[timeDown] = 0;
    dbgprintf(DBG_INFO, "%s\n", "Execute Release");
    if (cbBtns[actionReleased] && lib_buttonsLastInterruptAction[actionHeld] == 0) thpool_add_work(buttons_thpool, cbBtns[actionReleased], NULL);
  } else if (
    p == 0
    && lib_buttonsLastInterruptTime[timeUp] > 0 // Ensure it was Up at some point
    && (lib_buttonsLastInterruptTime[timeUp] + 50) < i_now // Jitter
    && lib_buttonsLastInterruptTime[timeDown] < (i_now - lib_buttonsBtnDebounceDelay)
  ) {
    lib_buttonsLastInterruptTime[timeDown] = i_now;
    lib_buttonsLastInterruptTime[timeUp] = 0;
    dbgprintf(DBG_INFO, "%s\n", "Execute Press");
    if (cbBtns[actionPressed]) thpool_add_work(buttons_thpool, cbBtns[actionPressed], NULL);
  }

 // cleanup:
  return rc;
}



// Rotary Management Routine
void lib_buttonsRotaryInterrupt(void) {
  if (lib_buttonsDisabled) { return; } // Check Buttons Disabled

  r_a = digitalRead(ROTARY_PIN_A);
  r_b = digitalRead(ROTARY_PIN_B);

  // Enforce something changed in the rotary knob
  if (lib_buttonsLastInterruptTime[E_BUTTON_ROTARY] != (r_a + r_b)) {
    lib_buttonsLastInterruptTime[E_BUTTON_ROTARY] = (r_a + r_b);

    r_seq_a <<= 1;
    r_seq_a |= r_a;

    r_seq_b <<= 1;
    r_seq_b |= r_b;

    // Mask the MSB four bits
    r_seq_a &= 0b00001111;
    r_seq_b &= 0b00001111;

    if (r_seq_a == 0b00000011 && r_seq_b == 0b00001001) {
      if (cbBtns[E_BUTTON_ROTARY_CCW]) cbBtns[E_BUTTON_ROTARY_CCW]();
    }

    // Compare the recorded sequence with the expected sequence
    if (r_seq_a == 0b00001001 && r_seq_b == 0b00000011) {
      if (cbBtns[E_BUTTON_ROTARY_CW]) cbBtns[E_BUTTON_ROTARY_CW]();
    }
  }

}


// Rotary Button Pressed
void lib_buttonsRotaryBtnInterrupt(void) {
  if (lib_buttonsDisabled) { return; } // Check Buttons Disabled
  lib_buttonsManageBtnInterrupt(ROTARY_PIN_BTN, E_BUTTON_ROTARY_UP, E_BUTTON_ROTARY_DOWN,
                  E_BUTTON_ROTARY_PRESSED, E_BUTTON_ROTARY_RELEASED, E_BUTTON_ROTARY_HELD);
}

// Left Button Pressed
void lib_buttonsLeftBtnInterrupt(void) {
  if (lib_buttonsDisabled) { return; } // Check Buttons Disabled
  lib_buttonsManageBtnInterrupt(LEFT_PIN_BTN, E_BUTTON_LEFT_UP, E_BUTTON_LEFT_DOWN,
                  E_BUTTON_LEFT_PRESSED, E_BUTTON_LEFT_RELEASED, E_BUTTON_LEFT_HELD);
}

// Right Button Pressed
void lib_buttonsRightBtnInterrupt(void) {
  if (lib_buttonsDisabled) { return; } // Check Buttons Disabled
  lib_buttonsManageBtnInterrupt(RIGHT_PIN_BTN, E_BUTTON_RIGHT_UP, E_BUTTON_RIGHT_DOWN,
                  E_BUTTON_RIGHT_PRESSED, E_BUTTON_RIGHT_RELEASED, E_BUTTON_RIGHT_HELD);
}

