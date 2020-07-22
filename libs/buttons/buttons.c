
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "GUIslice.h"
#include "libs/buttons/buttons.h"
#include "libs/queue/queue.h"



  
void lib_buttons_searchGPIO(const int whitelistPinsSize, const int *whitelistedPins, int pinCache[]) {
  for (int p = 0; p < whitelistPinsSize; p++) {
    pinCache[p] = digitalRead(whitelistedPins[p]);
  }
}

int lib_buttons_findGPIO(int i_timeout, int isleep) { // timeout in milliseconds to search, i_sleep wait between checks
  const int whitelistPinsSize = 18;
  const int whitelistedPins[] = { 2, 3, 4, 7, 8, 9, 15, 16, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
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

int lib_buttons_configure(char* config_path) {
  if (wiringPiSetup () == -1) {
    // debug_print("%s\n", "Failed Initializing Pi Wiring");
    return 0;
  }

  int leftBtn, rightBtn, rotaryBtn, rotaryA, rotaryB = -1;

  printf("Press Left Button\n");
  leftBtn = lib_buttons_findGPIO(10000, 100);
  printf("Left Button Found! %d\n", leftBtn);
  printf("Press Right Button\n");
  lib_buttons_waitRelease(leftBtn);
  rightBtn = lib_buttons_findGPIO(10000, 100);
  printf("Right Button Found! %d\n", rightBtn);
  printf("Press Directly Down On Rotary Button\n");
  lib_buttons_waitRelease(rightBtn);
  rotaryBtn = lib_buttons_findGPIO(10000, 100);
  printf("Rotary Button Found! %d\n", rotaryBtn);
  printf("Spin Rotary Knob Clockwise Until Detected\n");
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
    printf("Rotary Knob NOT Found, 20second Timeout!\n");
  } else {
    printf("Rotary Knob Found! %d %d\n", rotaryA, rotaryB);
  }

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

int lib_buttons_thread() {
  if (lib_buttonsDisabled) { return 0; } // Check Buttons Disabled

  int i_now = millis();

  // Left Press and Held
  if (lib_buttonsLastInterruptAction[E_BUTTON_LEFT_PRESSED] &&
      !lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] &&
      ((i_now - lib_buttonsLastInterruptTime[E_BUTTON_LEFT_LOW]) > lib_buttonsBtnHoldDelay)
  ) {
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] = 1;

    // debug_print("Set Left Held: %d\n", btn_l_h);
    // Execute Left Held when Right isn't also pressed (dbl btn)
    if (
      !lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_PRESSED] &&
      cbBtns[E_BUTTON_LEFT_HELD]
    ) {
      // debug_print("Execute Left Held: %d\n", btn_l_h);
      // Execute CB Left Held Function
      cbBtns[E_BUTTON_LEFT_HELD]();
    }
    return 1;
  }





  // Right Press and Held
  if (lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_PRESSED] &&
      !lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] &&
      ((i_now - lib_buttonsLastInterruptTime[E_BUTTON_RIGHT_LOW]) > lib_buttonsBtnHoldDelay)
  ) {
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] = 1;

    // Execute Right Held when Left isn't also pressed (dbl btn)
    if (
      !lib_buttonsLastInterruptAction[E_BUTTON_LEFT_PRESSED] &&
      cbBtns[E_BUTTON_RIGHT_HELD]
    ) {
      // Execute CB Right Held Function
      cbBtns[E_BUTTON_RIGHT_HELD]();
    }
    return 1;
  }





  // Rotary Press and Held
  if (lib_buttonsLastInterruptAction[E_BUTTON_ROTARY_PRESSED] &&
      !lib_buttonsLastInterruptAction[E_BUTTON_ROTARY_HELD] &&
      ((i_now - lib_buttonsLastInterruptTime[E_BUTTON_ROTARY_LOW]) > lib_buttonsBtnHoldDelay)
  ) {
    lib_buttonsLastInterruptAction[E_BUTTON_ROTARY_HELD] = 1;

    // Execute CB Rotary Held Function
    if (cbBtns[E_BUTTON_ROTARY_HELD]) cbBtns[E_BUTTON_ROTARY_HELD]();
    return 1;
  }



  // Dbl Button Press Held Found
  if (
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_PRESSED] &&
    ((i_now - lib_buttonsLastInterruptTime[E_BUTTON_RIGHT_LOW]) > lib_buttonsBtnHoldDelay) &&
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_PRESSED] &&
    ((i_now - lib_buttonsLastInterruptTime[E_BUTTON_LEFT_LOW]) > lib_buttonsBtnHoldDelay)
  ) {
    if (cbBtns[E_BUTTON_DOUBLE_HELD]) { cbBtns[E_BUTTON_DOUBLE_HELD](); }
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_PRESSED] = 0;
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_PRESSED] = 0;
    return 1;
  }
  return 0;
}




PI_THREAD (buttonsEventThread) {
  if (lib_buttonsThreadRunning) {
    // debug_print("%s\n", "Not Starting Buttons Event Thread, Already Started");
    return NULL;
  }
  lib_buttonsThreadRunning = 1;

  if (lib_buttonsThreadKill) {
    // debug_print("%s\n", "Not Starting Buttons Event Thread, Stop Flag Set");
    lib_buttonsThreadRunning = 0;
    return NULL;
  }

  // debug_print("%s\n", "Starting Buttons Event Thread");

//  int i_now = millis();
//  int btn_r_h, btn_l_h, btn_c_h;

  while(!lib_buttonsThreadKill) {
    if (!lib_buttons_thread()) {
      usleep(100000);
    }
  }

  lib_buttonsThreadRunning = 0;
  return NULL;
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

  if ( wiringPiISR (ROTARY_PIN_A, INT_EDGE_BOTH, &lib_buttonsRotaryInterrupt) < 0 ) {
    // debug_print("Unable to setup Rotary Pin A ISR: %s\n", strerror(errno));
  }
  if ( wiringPiISR (ROTARY_PIN_B, INT_EDGE_BOTH, &lib_buttonsRotaryInterrupt) < 0 ) {
    // debug_print("Unable to setup Rotary Pin B ISR: %s\n", strerror(errno));
  }
  if ( wiringPiISR (ROTARY_PIN_BTN, INT_EDGE_BOTH, &lib_buttonsRotaryBtnInterrupt) < 0 ) {
    // debug_print("Unable to setup Rotary Pin Btn ISR: %s\n", strerror(errno));
  }
  if ( wiringPiISR (RIGHT_PIN_BTN, INT_EDGE_BOTH, &lib_buttonsRightBtnInterrupt) < 0 ) {
    // debug_print("Unable to setup Right Btn ISR: %s\n", strerror(errno));
  }
  if ( wiringPiISR (LEFT_PIN_BTN, INT_EDGE_BOTH, &lib_buttonsLeftBtnInterrupt) < 0 ) {
    // debug_print("Unable to setup Left Btn ISR: %s\n", strerror(errno));
  }

}

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






// Set callback for button actions
void lib_buttonsSetCallbackFunc(int btn, void (*function)()) {
  cbBtns[btn] = function;
  lib_buttonsLastInterruptAction[btn] = 0;
  return;
}








// Overall Button Management Routine
int lib_buttonsManageBtnInterrupt(int pin, int timeLow, int timeHigh, \
                int actionPressed, int actionReleased, int actionHeld
) {
  if (lib_buttonsDisabled) { return 0; } // Check Buttons Disabled

  int p = digitalRead(pin);
  unsigned int i_now = millis();

  // printf("Pin: %d - %d - Delay: %d\n", pin, p, lib_buttonsBtnDebounceDelay);
  // Press down just set's flags
  if (p == LOW) {
    // printf("Low %d : %d\n", i_now, lib_buttonsLastInterruptTime[timeLow]);

    if ((i_now - lib_buttonsLastInterruptTime[timeLow]) < lib_buttonsBtnDebounceDelay) {
      return 0;
    }
    lib_buttonsLastInterruptTime[timeLow] = i_now;

    // Set Action flags for released / pressed
    lib_buttonsLastInterruptAction[actionReleased] = 0;
    lib_buttonsLastInterruptAction[actionPressed] = 1;

    // Execute CB Pressed Function
    if (cbBtns[actionPressed]) cbBtns[actionPressed]();

    return 0; // No nothing for now
  } else if (!lib_buttonsLastInterruptAction[actionPressed]) {
    // Button is not known to be pressed right now
    // printf("Not Pressed\n");

    // Reset High Pin Time
    if (i_now - lib_buttonsLastInterruptTime[timeHigh] > lib_buttonsBtnDebounceDelay) {
      lib_buttonsLastInterruptTime[timeHigh] = i_now;
    }
    return 0;
  } // Else Button Released

  // printf("High %d\n", lib_buttonsLastInterruptTime[timeHigh]);
  // Reset High Pin Time
  if (i_now - lib_buttonsLastInterruptTime[timeHigh] > lib_buttonsBtnDebounceDelay) {
    lib_buttonsLastInterruptTime[timeHigh] = i_now;
  }

  // Set Action flags for released / pressed
  lib_buttonsLastInterruptAction[actionReleased] = 1;
  lib_buttonsLastInterruptAction[actionPressed] = 0;


  // Was Btn Hold Activated? Don't Run Press / Release Actions
  if (lib_buttonsLastInterruptAction[actionHeld] == 1) {
    // debug_print("%s\n", "Held Was Activated");
    lib_buttonsLastInterruptAction[actionHeld] = 0;
    lib_buttonsLastInterruptAction[actionReleased] = 1;
    return 0;
  }

  // Exceute CB Released Function
  // printf("%s\n", "Execute Released");
  if (cbBtns[actionReleased]) cbBtns[actionReleased]();
  lib_buttonsLastInterruptAction[actionReleased] = 1;
  return 1;
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
  lib_buttonsManageBtnInterrupt(ROTARY_PIN_BTN, E_BUTTON_ROTARY_LOW, E_BUTTON_ROTARY_HIGH,
                  E_BUTTON_ROTARY_PRESSED, E_BUTTON_ROTARY_RELEASED, E_BUTTON_ROTARY_HELD);
}

// Left Button Pressed
void lib_buttonsLeftBtnInterrupt(void) {
  if (lib_buttonsDisabled) { return; } // Check Buttons Disabled
  lib_buttonsManageBtnInterrupt(LEFT_PIN_BTN, E_BUTTON_LEFT_LOW, E_BUTTON_LEFT_HIGH,
                  E_BUTTON_LEFT_PRESSED, E_BUTTON_LEFT_RELEASED, E_BUTTON_LEFT_HELD);
}

// Right Button Pressed
void lib_buttonsRightBtnInterrupt(void) {
  if (lib_buttonsDisabled) { return; } // Check Buttons Disabled
  lib_buttonsManageBtnInterrupt(RIGHT_PIN_BTN, E_BUTTON_RIGHT_LOW, E_BUTTON_RIGHT_HIGH,
                  E_BUTTON_RIGHT_PRESSED, E_BUTTON_RIGHT_RELEASED, E_BUTTON_RIGHT_HELD);
}

