#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "GUIslice.h"
#include "libs/shared.h"


enum {
  E_BUTTON_ROTARY_CW,
  E_BUTTON_ROTARY_CCW,
  E_BUTTON_ROTARY_PRESSED,
  E_BUTTON_ROTARY_RELEASED,
  E_BUTTON_ROTARY_HELD,
  E_BUTTON_LEFT_PRESSED,
  E_BUTTON_LEFT_RELEASED,
  E_BUTTON_LEFT_HELD,
  E_BUTTON_RIGHT_PRESSED,
  E_BUTTON_RIGHT_RELEASED,
  E_BUTTON_RIGHT_HELD,
  E_BUTTON_DOUBLE_HELD,
  MAX_BUTTONS
};

enum {
  E_BUTTON_ROTARY_DOWN,
  E_BUTTON_ROTARY_UP,
  E_BUTTON_LEFT_DOWN,
  E_BUTTON_LEFT_UP,
  E_BUTTON_RIGHT_DOWN,
  E_BUTTON_RIGHT_UP,
  E_BUTTON_ROTARY,
  MAX_BUTTON_STATES
};

// ------------------------------------------------
// Defines for Rotary Knob and Buttons
// ------------------------------------------------
int ROTARY_PIN_A; // = 22;
int ROTARY_PIN_B; // = 23;
int ROTARY_PIN_BTN; // = 21;
int RIGHT_PIN_BTN; // = 24;
int LEFT_PIN_BTN; // = 25;
int DEBOUNCE_DELAY; // = 0;

// placeholders for the rotary knob, determines which direction the knob was turned
unsigned int r_a;
unsigned int r_b;
unsigned int r_seq_a;
unsigned int r_seq_b;

unsigned int volatile lib_buttonsLastInterruptAction[MAX_BUTTONS];
unsigned int volatile lib_buttonsLastInterruptTime[MAX_BUTTON_STATES];

int       lib_buttonsBtnDebounceDelay;
int       lib_buttonsBtnHoldDelay;

int      lib_buttonsThreadKill; // Stopping mpvRpcSocket
int      lib_buttonsThreadRunning; // Running flag for Mpv RPC

int lib_buttonsDisabled; // Complete Disable Buttons While in Mirroring

int (*cbBtns[MAX_BUTTONS])();

void lib_buttons_searchGPIO(const int whitelistPinsSize, const int *whitelistedPins, int pinCache[]);
int lib_buttons_findGPIO(int i_timeout, int isleep);
void lib_buttons_waitRelease(int pin);

int lib_buttons_saveConfig(char* config_path, int leftBtn, int rightBtn, int rotaryBtn, int rotaryA, int rotaryB);
int lib_buttons_configure(char* config_path);

int lib_buttons_thread();
void lib_buttons_init();

int lib_buttonsThreadStart();
void lib_buttonsThreadStop();

void lib_buttonsSetCallbackFunc(int btn, int (*function)());

int lib_buttonsManageBtnInterrupt(int pin, int timeLow, int timeHigh, int actionPressed, \
                       int actionReleased, int actionHeld);
void lib_buttonsRotaryInterrupt(void);
void lib_buttonsRotaryBtnInterrupt(void);
void lib_buttonsLeftBtnInterrupt(void);
void lib_buttonsRightBtnInterrupt(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _BUTTONS_H_