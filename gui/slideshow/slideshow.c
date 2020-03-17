// System Headers
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <wiringPi.h> // Gordons Wiring Pi

#include "slideshow.h"

#include "shared.h"
#include "queue/queue.h"
#include "buttons/buttons.h"
#include "mpv/mpv.h"
#include "gui/pages.h"
#include "fbcp/fbcp.h"

void pg_slideshowReset(gslc_tsGui *pGui) {
  // debug_print("%s\n", "Slideshow Reset");
  if (pg_slideshowVimLock == 0) {
    pg_slideshowVimLock = 1;
    pg_slideshow_destroy(pGui);
    pg_slideshow_open(pGui);
    pg_slideshowVimLock = 0;
  }
}

void pg_slideshowSendChar(char* cmd) {
  // debug_print("Send Char: %s\n", cmd);
  int lockCnt = 0;
  while (pg_slideshowVimLock == 1 && lockCnt < 25) {
    // debug_print("%s\n", "Locked VIM");
    usleep(100);
    lockCnt++;
  }
  fputs(cmd, pg_slideshowFD);
}

void pg_slideshowPrevImage() {
  if (pg_slideshowZooming) { return; }

  size_t cmdSz = strlen("playlist-prev\n") + 1;
  char *cmd = (char*)malloc(cmdSz * sizeof(char));
  strlcpy(cmd, "playlist-prev\n", cmdSz);
  mpv_cmd(cmd);
  //-/ pg_slideshowSendChar("p");
}

void pg_slideshowNextImage() {
  if (pg_slideshowZooming) { return; }

  size_t cmdSz = strlen("playlist-next\n") + 1;
  char *cmd = (char*)malloc(cmdSz * sizeof(char));
  strlcpy(cmd, "playlist-next\n", cmdSz);
  mpv_cmd(cmd);
  //-/ pg_slideshowSendChar("n");
}



double pg_slideshow_zoomPanChange() {
  if (pg_slideshowZoom > 2) {
    return .05;
  } else {
    return .1;
  }
}

void pg_slideshow_ctrlUp() {
  if (pg_slideshowZooming == 1) {
    pg_slideshowPanY += pg_slideshow_zoomPanChange();
    mpv_set_prop_double("video-pan-y", pg_slideshowPanY);
    //-/ pg_slideshowSendChar("k");
  }
}
void pg_slideshow_ctrlDown() {
  if (pg_slideshowZooming == 1) {
    pg_slideshowPanY -= pg_slideshow_zoomPanChange();
    mpv_set_prop_double("video-pan-y", pg_slideshowPanY);
    //-/ pg_slideshowSendChar("j");
  }
}
void pg_slideshow_ctrlRight() {
  if (pg_slideshowZooming == 1) {
    pg_slideshowPanY -= pg_slideshow_zoomPanChange();
    mpv_set_prop_double("video-pan-x", pg_slideshowPanY);
    //-/ pg_slideshowSendChar("l");
  }
}
void pg_slideshow_ctrlLeft() {
  if (pg_slideshowZooming == 1) {
    pg_slideshowPanY += pg_slideshow_zoomPanChange();
    mpv_set_prop_double("video-pan-x", pg_slideshowPanY);
    //-/ pg_slideshowSendChar("h");
  }
}
void pg_slideshow_ctrlZoom() {
  if (pg_slideshowZoom <= 0) {
    pg_slideshowZoom = 0;
    if (pg_slideshowIsPicture) { mpv_play(); }
  } else {
    if (pg_slideshowIsPicture) { mpv_pause(); }
  }
  mpv_set_prop_double("video-zoom", pg_slideshowZoom);
}
void pg_slideshow_ctrlZoomIn() {
  pg_slideshowZoom += .5;
  pg_slideshow_ctrlZoom();
  //-/pg_slideshowSendChar("+");
}
void pg_slideshow_ctrlZoomOut() {
  pg_slideshowZoom -= .5;
  pg_slideshow_ctrlZoom();
  //-/pg_slideshowSendChar("+");
}


bool pg_slideshow_cbBtn_close(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  touchscreenPageGoBack(pGui);
  return true;
}

bool pg_slideshow_cbBtn_fbcp(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);

  if (!fbcp_toggle()) {
    gslc_PageRedrawSet(pGui, true);
  }
  return true;
}


bool pg_slideshow_cbBtn_up(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  pg_slideshow_ctrlUp();
  return true;
}
bool pg_slideshow_cbBtn_down(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  pg_slideshow_ctrlDown();
  return true;
}
bool pg_slideshow_cbBtn_right(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  pg_slideshow_ctrlRight();
  return true;
}
bool pg_slideshow_cbBtn_left(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch != GSLC_TOUCH_UP_IN) { return true; }
  pg_slideshow_ctrlLeft();
  return true;
}

bool pg_slideshowCbBtn(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY) {
  if (eTouch == GSLC_TOUCH_DOWN_IN) {
    pg_slideshowGentureXa = nX;
    pg_slideshowGentureYa = nY;

  } else if (eTouch == GSLC_TOUCH_MOVE_IN) {
    // printf("T: %d, X: %d, Y: %d\n", eTouch, nX, nY);

  } else if (eTouch == GSLC_TOUCH_UP_IN) {
    pg_slideshowGentureXb = nX;
    pg_slideshowGentureYb = nY;

    // Gesture Calcuator
    int zX = pg_slideshowGentureXa - pg_slideshowGentureXb;
    int zY = pg_slideshowGentureYa - pg_slideshowGentureYb;

    if (abs(zX) == abs(zY)) {
      // Diagonal
      if (abs(zX) > 10) {
        // debug_print("Diagonal %d\n", abs(zX));
      } else {
        // printf("Clicked %d\n", abs(zX));
        pg_slideshowZooming = 1;
        pg_slideshow_ctrlZoomIn();
      }
    } else if (abs(zX) > abs(zY)) {
      // Majority Horizontal
      if (zX > 15) {
        // printf("Leftish %d\n", abs(zX));
        pg_slideshow_ctrlRight();
      } else if (zX < -15) {
        // printf("Rightish %d\n", abs(zX));
        pg_slideshow_ctrlLeft();
      } else {
        // printf("Clicked %d\n", abs(zX));
        pg_slideshowZooming = 1;
        pg_slideshow_ctrlZoomIn();
      }
    } else if (abs(zX) < abs(zY)) {
      // Majority Vertical
      if (zY > 10) {
        // printf("Upish %d\n", abs(zY));
        pg_slideshow_ctrlUp();

      } else if (zY < -10) {
        // printf("Downish %d\n", abs(zY));
        pg_slideshow_ctrlUp();
      } else {
        // printf("Clicked %d\n", abs(zY));
        pg_slideshowZooming = 1;
        pg_slideshow_ctrlZoomIn();
      }
    }
  }
  return true;
}

void pg_slideshowGuiInit(gslc_tsGui *pGui) {
  pg_slideshowPgPointed = 1;
  int ePage = E_PG_SLIDESHOW;

  gslc_PageAdd(pGui, E_PG_SLIDESHOW, pg_slideshowElem, MAX_ELEM_PG_SLIDESHOW_RAM,
               pg_slideshowElemRef, MAX_ELEM_PG_SLIDESHOW);
  // gslc_SetBkgndColor(pGui, GSLC_COL_GRAY_DK2);

  pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
    (gslc_tsRect){160,100,160,120},
    (char*)" ", 0, E_FONT_MONO14, &pg_slideshowCbBtn);
  gslc_ElemSetCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD], GSLC_COL_WHITE, GSLC_COL_GREEN, GSLC_COL_BLACK);
  gslc_ElemSetFillEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD], true);
  gslc_ElemSetGlowEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD], false);
  gslc_ElemSetFrameEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD], true);

  // Touchpad Up
  pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_UP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {160,0,160,100},
          "^", 0, E_FONT_MONO28, &pg_slideshow_cbBtn_up);
  gslc_ElemSetTxtCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_UP], GSLC_COL_BLACK);
  gslc_ElemSetCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_UP], GSLC_COL_WHITE, GSLC_COL_GREEN_LT3, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_UP], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_UP], true);
  gslc_ElemSetGlowEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_UP], false);
  gslc_ElemSetFrameEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_UP], true);


  // Touchpad Down
  pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_DOWN] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {160,220,160,100},
          "v", 0, E_FONT_MONO28, &pg_slideshow_cbBtn_down);
  gslc_ElemSetTxtCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_DOWN], GSLC_COL_BLACK);
  gslc_ElemSetCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_DOWN], GSLC_COL_WHITE, GSLC_COL_GREEN_LT2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_DOWN], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_DOWN], true);
  gslc_ElemSetGlowEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_DOWN], false);
  gslc_ElemSetFrameEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_DOWN], true);

  // Touchpad Left
  pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_LEFT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {0,100,160,120},
          "<", 0, E_FONT_MONO28, &pg_slideshow_cbBtn_left);
  gslc_ElemSetTxtCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_LEFT], GSLC_COL_BLACK);
  gslc_ElemSetCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_LEFT], GSLC_COL_WHITE, GSLC_COL_GREEN_LT2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_LEFT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_LEFT], true);
  gslc_ElemSetGlowEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_LEFT], false);
  gslc_ElemSetFrameEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_LEFT], true);


  // Touchpad Right
  pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_RIGHT] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {320,100,160,120},
          ">", 0, E_FONT_MONO28, &pg_slideshow_cbBtn_right);
  gslc_ElemSetTxtCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_RIGHT], GSLC_COL_BLACK);
  gslc_ElemSetCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_RIGHT], GSLC_COL_WHITE, GSLC_COL_GREEN_LT2, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_RIGHT], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_RIGHT], true);
  gslc_ElemSetGlowEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_RIGHT], false);
  gslc_ElemSetFrameEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_TOUCHPAD_RIGHT], true);




  // Close Key
  pg_slideshowEl[E_SLIDESHOW_EL_CLOSE] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {370,270,110,50},
          "Close", 0, E_FONT_MONO14, &pg_slideshow_cbBtn_close);
  gslc_ElemSetTxtCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_CLOSE], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_CLOSE], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_slideshowEl[E_SLIDESHOW_EL_CLOSE], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_CLOSE], false);
  gslc_ElemSetGlowEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_CLOSE], false);
  gslc_ElemSetFrameEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_CLOSE], true);

  // FBCP Key
  pg_slideshowEl[E_SLIDESHOW_EL_FBCP] = gslc_ElemCreateBtnTxt(pGui, GSLC_ID_AUTO, ePage,
          (gslc_tsRect) {0,270,110,50},
          "Mirror", 0, E_FONT_MONO14, &pg_slideshow_cbBtn_fbcp);
  gslc_ElemSetTxtCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_FBCP], GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pg_slideshowEl[E_SLIDESHOW_EL_FBCP], GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(pGui, pg_slideshowEl[E_SLIDESHOW_EL_FBCP], GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_FBCP], false);
  gslc_ElemSetGlowEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_FBCP], false);
  gslc_ElemSetFrameEn(pGui, pg_slideshowEl[E_SLIDESHOW_EL_FBCP], true);
}


void pg_slideshowButtonRotaryCW() {
  // Zoom Mode, Right Button Pressed, Rotary CW
  if (pg_slideshowZooming && lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_PRESSED]) {
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] = 1; // Surpress cbHeld
    pg_slideshow_ctrlUp();


  // Zoom Mode, Left Button Pressed, Rotary CW
  } else if (pg_slideshowZooming && lib_buttonsLastInterruptAction[E_BUTTON_LEFT_PRESSED]) {
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] = 1; // Surpress cbHeld
    pg_slideshowZooming = 1;
    pg_slideshow_ctrlZoomIn();


  // Right Button Pressed, Rotary CW
  } else if (lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_PRESSED]) {
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] = 1; // Surpress cbHeld
    // pg_slideshowReset();


  // Left Button Pressed, Rotary CW
  } else if (lib_buttonsLastInterruptAction[E_BUTTON_LEFT_PRESSED]) {
    pg_slideshowZooming = 1; // Start Zoom Mode
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] = 1; // Surpress cbHeld
    pg_slideshow_ctrlZoomIn();


  // Zoom Mode, Rotary CW
  } else if (pg_slideshowZooming) {
    pg_slideshow_ctrlRight();
  // Rotary CW
  } else {
    pg_slideshowNextImage();
  }
}




void pg_slideshowButtonRotaryCCW() {
  // Zoom Mode, Right Button Pressed, Rotary CCW
  if (pg_slideshowZooming && lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_PRESSED]) {
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] = 1; // Surpress cbHeld
    pg_slideshow_ctrlDown();

  // Zoom Mode, Left Button Pressed, Rotary CCW
  } else if (pg_slideshowZooming && lib_buttonsLastInterruptAction[E_BUTTON_LEFT_PRESSED]) {
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] = 1; // Surpress cbHeld
    pg_slideshow_ctrlZoomOut();


  // Right Button Pressed, Rotary CCW
  } else if (lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_PRESSED]) {
    lib_buttonsLastInterruptAction[E_BUTTON_RIGHT_HELD] = 1; // Surpress cbHeld
    // pg_slideshowReset();


  // Left Button Pressed, Rotary CCW
  } else if (lib_buttonsLastInterruptAction[E_BUTTON_LEFT_PRESSED]) {
    pg_slideshowZooming = 1; // Start Zoom Mode
    lib_buttonsLastInterruptAction[E_BUTTON_LEFT_HELD] = 1; // Surpress cbHeld
    pg_slideshow_ctrlZoomOut();


  // Zoom Mode, Rotary CCW
  } else if (pg_slideshowZooming) {
    pg_slideshow_ctrlLeft();

  // Rotary CCW
  } else {
    pg_slideshowPrevImage();
  }
}


void pg_slideshowButtonLeftPressed() {
  if (pg_slideshowZooming) {
    pg_slideshowZooming = 0;
    pg_slideshowZoom = 0;
    pg_slideshowPanY = 0;
    pg_slideshowPanX = 0;
  }
  pg_slideshowPrevImage();
}

void pg_slideshowButtonRightPressed() {
  if (pg_slideshowZooming) {
    pg_slideshowZooming = 0;
    pg_slideshowZoom = 0;
    pg_slideshowPanY = 0;
    pg_slideshowPanX = 0;
  }
  pg_slideshowNextImage();
}

void pg_slideshowButtonRotaryPressed() {
  // debug_print("%s\n", "Rotary Button!");
  if (pg_slideshowZooming) {
    // Zoom mode disable
    pg_slideshowZooming = 0;
    pg_slideshowZoom = 0;
    pg_slideshowPanY = 0;
    pg_slideshowPanX = 0;
    // reset image to auto zoom size

    mpv_set_prop_double("video-pan-x", pg_slideshowPanX);
    mpv_set_prop_double("video-pan-y", pg_slideshowPanY);
    mpv_set_prop_double("video-zoom", pg_slideshowZoom);

  } else {
    mpv_playpause_toggle();
  }
}

void pg_slideshowButtonLeftHeld() {
  // debug_print("%s\n", "Left Held Slideshow");
  // printf("Zoom ^\n");
  mpv_set_prop_int("playlist-pos", 0);
  //-/ pg_slideshowSendChar("^"); // Goto Beginning
}

void pg_slideshowButtonRightHeld() {
  // printf("%s\n", "Right Held Slideshow");
  mpv_stop();
  mpv_playlist_clear();
  char* plFile = "/home/pi/shared/ssoa/playlist.txt";
  size_t cmdSz = snprintf(NULL, 0, "loadlist \"%s\"\n", plFile) + 1;
  char *cmd = (char*)malloc(cmdSz * sizeof(char));
  snprintf(cmd, cmdSz, "loadlist \"%s\"\n", plFile);
  mpv_cmd(cmd);
}

void pg_slideshowButtonRotaryHeld() {
  // debug_print("%s\n", "Rotary Held Slideshow");
  touchscreenPageGoBack(&m_gui);
}

void pg_slideshowButtonDoubleHeld() {
  // kill(0, SIGINT);
  touchscreenPageGoBack(&m_gui);
}

void pg_slideshowButtonSetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, &pg_slideshowButtonRotaryCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, &pg_slideshowButtonRotaryCCW);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, &pg_slideshowButtonLeftPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, &pg_slideshowButtonRightPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, &pg_slideshowButtonRotaryPressed);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, &pg_slideshowButtonLeftHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, &pg_slideshowButtonRightHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, &pg_slideshowButtonRotaryHeld);
  lib_buttonsSetCallbackFunc(E_BUTTON_DOUBLE_HELD, &pg_slideshowButtonDoubleHeld);
}

void pg_slideshowButtonUnsetFuncs() {
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CW, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_CCW, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_RELEASED, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_LEFT_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_RIGHT_HELD, NULL);
  lib_buttonsSetCallbackFunc(E_BUTTON_ROTARY_HELD, NULL);
}


/*
PI_THREAD (pg_slideshowFolderWatchThread) {
  if (pg_slideshowFolderWatchThreadRunning) { return NULL; }
  pg_slideshowFolderWatchThreadRunning = 1;
  char tmp_inotify_ret[1024];

  // debug_print("%s\n", "Opening Slideshow");
  // Open the command for reading.
  pg_slideshowFolderWatchFP = popen("/usr/bin/inotifywait -m -e create,delete,move --exclude '/\\.' /home/pi/shared", "r");
  if (pg_slideshowFolderWatchFP == NULL) {
    // puts("Failed to run command");
    return NULL;
  }

  int d = fileno(pg_slideshowFolderWatchFP);
  fcntl(d, F_SETFL, O_NONBLOCK);

  size_t chread;
  // Read the output a line at a time - output it.
  while ((chread = fread(tmp_inotify_ret, 1, sizeof(char), pg_slideshowFolderWatchFP)) != 0) {
  // while (fgets(tmp_inotify_ret, sizeof(char)-1, pg_slideshowFolderWatchFP) != NULL) {
    // debug_print("INotify--:%s:--\n", tmp_inotify_ret);
	  char delim[] = " ";
    char *ptr = strtok(tmp_inotify_ret, delim);

    // Filename is last space separated value
    char *filename = ptr;
    // debug_print(" %s\n", ptr);
    while ((ptr = strtok(NULL," ")) != NULL) {
      // debug_print(" %s\n", ptr);
      filename = ptr;
    }

    // Not Dotfiles.. macbooks are bad about these
    if (filename[0] != '.') {
      // debug_print("Update Set For %s\n", filename);
      // pg_slideshowUpdateFileList();
    }
//     if (strcmp(tmp_inotify_ret, "/home/pi/shared/ CREATE mpv.socket\n") == 0) {
//      printf("INotify Found.\n");
//    }
  }

  // close
  int exitCode = WEXITSTATUS(pclose(pg_slideshowFolderWatchFP));
  pg_slideshowFolderWatchThreadRunning = 0;
  if (exitCode) {
    return NULL;
  } else {
    return NULL;
  }
}

int pg_slideshowFolderWatchThreadStart() {
  // debug_print("%s\n", "Starting Slideshow Folder Watch");
  return piThreadCreate(pg_slideshowFolderWatchThread);
}

void pg_slideshowFolderWatchThreadStop() {
  if (pg_slideshowFolderWatchThreadRunning) {
    system("killall /usr/bin/inotifywait");
    // fflush(pg_slideshowFolderWatchFP);
    // pclose(pg_slideshowFolderWatchFP);
    pg_slideshowFolderWatchThreadRunning = 0;
    // debug_print("%s\n", "Slideshow Folder Watch Shutdown");
  }
}
*/




// ------------------------
// MPV Socket Thread
// ------------------------
PI_THREAD (pg_slideshowMpvSocketThread)
{
  if (pg_slideshowMpvSocketThreadRunning) {
    // debug_print("%s\n", "Not Starting MPV Event Thread, Already Started");
    return NULL;
  }
  pg_slideshowMpvSocketThreadRunning = 1;

  if (pg_slideshowMpvSocketThreadKill) {
    // debug_print("%s\n", "Not Starting MPV Event Thread, Stop Flag Set");
    pg_slideshowMpvSocketThreadRunning = 0;
    return NULL;
  }

  // debug_print("%s\n", "Starting MPV Event Thread");

  struct sockaddr_un addr;
  // char buf[100];

  // char* mpv_chapter_ret;

  int socket_try = 0;
  // struct queue_head *item;

  // Wait for socket to arrive
  while (!pg_slideshowMpvSocketThreadKill && socket_try < 10 && access(mpv_socket_path, R_OK) == -1) {
    // debug_print("Waiting to See %s\n", mpv_socket_path);
    socket_try++;
    usleep(1000000);
  }

  if ((pg_slideshow_player_mpv_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    // debug_print("%s\n", "MPV Socket Error");
    pg_slideshowMpvSocketThreadKill = 1;
  }
  // Set Socket Non-Blocking
  setnonblock(pg_slideshow_player_mpv_fd);

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  if (*mpv_socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path+1, mpv_socket_path+1, sizeof(addr.sun_path)-2);
  } else {
    strncpy(addr.sun_path, mpv_socket_path, sizeof(addr.sun_path)-1);
  }

  if (connect(pg_slideshow_player_mpv_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
    // MPV Connect Error
    pg_slideshowMpvSocketThreadKill = 1;
  }

  // Grab MPV Events, sent in JSON format
  while(!pg_slideshowMpvSocketThreadKill) {
    if (!fd_is_valid(pg_slideshow_player_mpv_fd)) {
      // printf("FD Re-Connect: %d, %d\n", pg_slideshow_player_mpv_fd_timer, millis());
      // try closing fd
      if (pg_slideshow_player_mpv_fd) { close(pg_slideshow_player_mpv_fd); }
      // reconnect fd
      if (connect(pg_slideshow_player_mpv_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        // MPV Connect Error
        pg_slideshowMpvSocketThreadKill = 1;
      }
    }

// Grab Next Socket Line
    char* mpv_event_ret; // = malloc(128);
    // int rc = getline(&mpv_event_ret, 256, pg_slideshow_player_mpv_fd);
    int rc = sgetline(pg_slideshow_player_mpv_fd, &mpv_event_ret);
    if (rc > 0) {
      char* json_event;// = malloc(128);
      int rcE = ta_json_parse(mpv_event_ret, "event", &json_event);
      // dbgprintf(DBG_MPV, "MPV Event: -%s-, Parsed: %s Len: %d\n\n\n", mpv_event_ret, json_event, rcE);
      free(mpv_event_ret);
      if (rcE == 0) { continue; }


      if (strcmp(json_event, "seek") == 0
          || strcmp(json_event, "playback-restart") == 0
      ) {

      }
      else if (strcmp(json_event, "pause") == 0) {


      }
      // else if (strcmp(json_event, "unpause") == 0) {
      else if (strcmp(json_event, "unpause") == 0) {

      }
      // MPV Video Ended
      else if (strcmp(json_event, "end-file") == 0) {
        // Update GUI Display of Display Ended
        // endVideoPlayer();

      }
      // MPV Start File
      else if (strcmp(json_event, "start-file") == 0) {

      }
      // Meta Updated
      else if (strcmp(json_event, "metadata-update") == 0) {
        // printf("Meta update\n");
        // printf("Parsed: %s Len: %d\n\n\n", json_event, rcE);
        char* retFrameInfo;
        if (mpvSocketSinglet("video-frame-info", &retFrameInfo) != -1) {
          // printf("Info: %s\n", retFrameInfo);
          char* json_picType;
          pg_slideshowIsPicture = ta_json_parse(retFrameInfo, "picture-type", &json_picType);
          free(retFrameInfo);
          if (pg_slideshowIsPicture > 0) { free(json_picType); }
        } else {
          pg_slideshowIsPicture = 0;
        }
      }
      // Chapter Changed
      else if (strcmp(json_event, "chapter-change") == 0) {


      }
      // File Loaded (Unpause)
      else if (strcmp(json_event, "file-loaded") == 0) {
        // sdob_player->paused = 0;

      }
      free(json_event);
    usleep(100);
    }

  // No Work needed done
    else {
      // Nothing to Do, Sleep for a moment
      usleep(200000);
    }
  }
  // close
  // debug_print("%s\n", "Closing MPV RPC");
  pg_slideshowMpvSocketThreadRunning = 0;
  return NULL;
}


int pg_slideshowMpvSocketThreadStart() {
  // debug_print("%s\n", "pg_slideshowMpvSocketThreadStart()");
  if (pg_slideshowMpvSocketThreadRunning) { return 0; }

  // debug_print("SkydiveOrBust MPV Socket Thread Spinup: %d\n", pg_slideshowMpvSocketThreadRunning);
  pg_slideshowMpvSocketThreadKill = 0;
  return piThreadCreate(pg_slideshowMpvSocketThread);
}

void pg_slideshowMpvSocketThreadStop() {
  // debug_print("%s\n", "pg_slideshowMpvSocketThreadStop()");
  // Shutdown MPV Socket Thread
  if (pg_slideshowMpvSocketThreadRunning) {
    pg_slideshowMpvSocketThreadKill = 1;
    int shutdown_cnt = 0;
    while (pg_slideshowMpvSocketThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
    // debug_print("SkydiveOrBust MPV Socket Thread Shutdown %d\n", shutdown_cnt);
  }
}




// GUI Init
void pg_slideshow_init(gslc_tsGui *pGui) {
  // m_clean_dotfiles = "find /home/pi/shared -type f -name \".*\" -exec rm \"{}\" \\;";

  pg_slideshowPaused = 0; // Pause Slideshow

  pg_slideshowZooming = 0;
  pg_slideshowPanY = 0;
  pg_slideshowPanX = 0;
  pg_slideshowVimLock = 0;

  pg_slideshowIsPicture = 0;

  mpv_init(pGui);
  pg_slideshowGuiInit(pGui);

  cbInit[E_PG_SLIDESHOW] = NULL;
}




int pg_slideshow_thread(gslc_tsGui *pGui) {
  char tmp_inotify_ret[1024];
  while (fgets(tmp_inotify_ret, sizeof(tmp_inotify_ret)-1, pg_slideshowFD) != NULL) {
    printf("SLIDESHOW--:%s:--\n", tmp_inotify_ret);

  }
  return 0;
}



// GUI Open
void pg_slideshow_open(gslc_tsGui *pGui) {
  printf("%s\n", "Slideshow Setting Button Functions");
  pg_slideshowButtonSetFuncs();

  pg_slideshowMpvSocketThreadStart();

  fbcp_start();
  // fbcpThreadStart();

  char* retPath;
  if ((mpvSocketSinglet("path", &retPath)) == -1) {
    char *cmd = strdup("loadlist \"/home/pi/shared/playlist.txt\" replace\n");
    mpv_cmd(cmd);
  }

  // Play on Open
  mpv_play();
/*
  if(!(pg_slideshowFD = popen("/usr/bin/fim -d /dev/fb0 -a -q --sort-basename --no-commandline -R /home/pi/shared/", "w"))){
    // debug_print("%s\n", "Cannot Open image folder");
  } else {
    int d = fileno(pg_slideshowFD);
    fcntl(d, F_SETFL, O_NONBLOCK);
    setbuf(pg_slideshowFD, NULL);
  }
*/
  // // debug_print("%s\n", "Slideshow Starting Folder Watch");
  // pg_slideshowFolderWatchThreadStart();
  // debug_print("%s\n", "Slideshow Starting FBCP");
  // fbcp_start();
  // // debug_print("%s\n", "Slideshow Updating Filelist");
  // pg_slideshowUpdateFileList();

  // debug_print("%s\n", "Slideshow Opening");
}


void pg_slideshow_close(gslc_tsGui *pGui) {
  fbcp_stop();
  // fbcpThreadStop();
  mpv_stop();
  pg_slideshowMpvSocketThreadStop();

  //-/ system("killall fim");
  //-/ fflush(pg_slideshowFD);
  //-/ pclose(pg_slideshowFD);
}

// GUI Destroy
void pg_slideshow_destroy(gslc_tsGui *pGui) {
  // debug_print("%s\n", "Slideshow Stopping");
  // pg_slideshowButtonUnsetFuncs();

  mpv_playlist_clear();

  // // debug_print("%s\n", "Slideshow Stopping Folder Watch");
  // pg_slideshowFolderWatchThreadStop();
  // // debug_print("%s\n", "Slideshow Stopping Slideshow Thread");
  // pg_slideshowThreadStop();
  // debug_print("%s\n", "Slideshow Destroyed");
}

void __attribute__ ((constructor)) pg_slideshow_constructor(void) {
  cbInit[E_PG_SLIDESHOW] = &pg_slideshow_init;
  cbOpen[E_PG_SLIDESHOW] = &pg_slideshow_open;
  cbClose[E_PG_SLIDESHOW] = &pg_slideshow_close;
  // cbThread[E_PG_SLIDESHOW] = &pg_slideshow_thread;
  cbDestroy[E_PG_SLIDESHOW] = &pg_slideshow_destroy;
  // pg_slideshowQueue = ALLOC_QUEUE_ROOT();
}

void __attribute__ ((destructor)) pg_slideshow_destructor(void) {

}

