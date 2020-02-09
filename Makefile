HOME = /home/pi
DEBUG = -O3
CC = clang
INCLUDE = #-I/usr/local/include
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g -pthread
LDFLAGS = -L/usr/local/lib -L/opt/vc/lib 
LDLIB_EXTRA = -lwiringPi -lconfig -ljsmn -liw -lmpv -lxml2 -lsystemd -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread

GSLC_CORE = GUIslice/GUIslice.c GUIslice/elem/*.c #GUIslice/GUIslice_config.h
GSLC_LIBS = -I./GUIslice

TOUCHAPP_LIBS = -I./libs -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host -I/opt/vc/include/interface/vmcs_host/linux
TOUCHAPP_CORE = libs/*.c $(wildcard libs/**/*.c) gui/*.c $(wildcard gui/**/*.c) \
                wpa/*.o 


APP_GUI =
APP_MODULES =

GSLC_DRV := SDL1

ifndef GSLC_TOUCH
  GSLC_LDLIB_EXTRA := -lm -lts
else
  ifeq (TSLIB,${GSLC_TOUCH})
    GSLC_LDLIB_EXTRA := -lm -lts
  else
    GSLC_LDLIB_EXTRA := -lm
  endif
endif

# === SDL1.2 ===
ifeq (SDL1,${GSLC_DRV})
  GSLC_SRCS = GUIslice/GUIslice_drv_sdl.c
  # - Add extra linker libraries if needed
  LDLIBS = -lSDL -lSDL_ttf ${GSLC_LDLIB_EXTRA}
endif

## === SDL2.0 ===
#ifeq (SDL2,${GSLC_DRV})
#  $(info GUIslice driver mode: SDL2)
#  GSLC_SRCS = GUIslice/GUIslice_drv_sdl.c
#  # - Add extra linker libraries if needed
#  LDLIBS = -lSDL2 -lSDL2_ttf ${GSLC_LDLIB_EXTRA}
#endif


SRC = touchapp.c

# OBJ = $(SRC:.c=.o)

BINS = $(SRC:.c=)

all: $(BINS)

clean:
	@echo "Cleaning directory..."
	$(RM) $(BINS)

wpa/wpa_ctrl.o:
	@echo [Building $@]
	gcc -Wall -Wextra -I./wpa/ -MMD -c -g -o ./wpa/wpa_ctrl.o ./wpa/wpa_ctrl.c -D CONFIG_CTRL_IFACE -D CONFIG_CTRL_IFACE_UNIX

wpa/os_unix.o:
	@echo [Building $@]
	gcc -Wall -Wextra -I./wpa/ -MMD -c -g -o ./wpa/os_unix.o ./wpa/os_unix.c -D CONFIG_CTRL_IFACE -D CONFIG_CTRL_IFACE_UNIX

touchapp: touchapp.c $(TOUCHAPP_CORE) $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -fsanitize=address -o $@ touchapp.c $(TOUCHAPP_CORE) $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . $(TOUCHAPP_LIBS) $(GSLC_LIBS) $(LDLIB_EXTRA)
#	@$(CC) $(CFLAGS) -o $@ touchapp.c $(TOUCHAPP_CORE) $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . $(TOUCHAPP_LIBS) $(GSLC_LIBS) $(LDLIB_EXTRA)
