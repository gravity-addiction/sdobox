HOME = /home/pi
DEBUG = -O1 -g
# CC = clang -fsanitize=address
# CC = clang -fsanitize=leak
CC = gcc
INCLUDE = -I. $(GSLC_INCLUDES) $(TOUCHAPP_INCLUDES)
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g -pthread -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64
LDFLAGS = -L/usr/local/lib -L/opt/vc/lib
LDLIB_EXTRA = -lwiringPi -lconfig -liw -lmpv -lxml2 -lsystemd -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -Llibs/jsmn -ljsmn

GSLC_CORE := GUIslice/GUIslice.c $(wildcard GUIslice/elem/*.c) #GUIslice/GUIslice_config.h
GSLC_INCLUDES = -I./GUIslice

TOUCHAPP_INCLUDES = -I./libs -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host -I/opt/vc/include/interface/vmcs_host/linux
TOUCHAPP_CORE := $(wildcard libs/*.c) $(wildcard libs/**/*.c) $(wildcard gui/*.c) $(wildcard gui/**/*.c) \
		         $(wildcard wpa/*.c)

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
  LDLIBS = -Llibs/SDL -lSDL -lSDL_ttf ${GSLC_LDLIB_EXTRA}
endif

## === SDL2.0 ===
#ifeq (SDL2,${GSLC_DRV})
#  $(info GUIslice driver mode: SDL2)
#  GSLC_SRCS = GUIslice/GUIslice_drv_sdl.c
#  # - Add extra linker libraries if needed
#  LDLIBS = -lSDL2 -lSDL2_ttf ${GSLC_LDLIB_EXTRA}
#endif

all: touchapp

TOUCHAPP_SRCS=touchapp.c $(TOUCHAPP_CORE) $(GSLC_CORE) $(GSLC_SRCS)
TOUCHAPP_OBJS=$(patsubst %.c,%.o,$(TOUCHAPP_SRCS))

ifeq ($(filter clean cleaner,$(MAKECMDGOALS)),)
  include $(patsubst %.c,%.dep,$(TOUCHAPP_SRCS))
endif

%.dep: %.c
	$(COMPILE.c) -MM $< | sed 's&^.*:&$(@) $(*).o:&' $(if $(REBUILD_ON_MAKEFILE_CHANGES),| sed 's/:/ $@: Makefile/') > $@

clean:
	@echo "Cleaning directory..."
	$(RM) touchapp $(TOUCHAPP_OBJS) $(patsubst %.c,%.dep,$(TOUCHAPP_SRCS)) TAGS

wpa/wpa_ctrl.o:
	@echo [Building $@]
	gcc -Wall -Wextra -I./wpa/ -MMD -c -g -o ./wpa/wpa_ctrl.o ./wpa/wpa_ctrl.c -D CONFIG_CTRL_IFACE -D CONFIG_CTRL_IFACE_UNIX

wpa/os_unix.o:
	@echo [Building $@]
	gcc -Wall -Wextra -I./wpa/ -MMD -c -g -o ./wpa/os_unix.o ./wpa/os_unix.c -D CONFIG_CTRL_IFACE -D CONFIG_CTRL_IFACE_UNIX

touchapp: $(TOUCHAPP_OBJS)
	@echo [Building $@]
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) $(LDLIB_EXTRA)

tags:
	etags $(TOUCHAPP_SRCS) *.h GUIslice/GUIslice_config.h $(shell find libs -name '*.h') $(shell find gui -name '*.h') wpa/*.h
