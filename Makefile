HOME = /home/pi
DEBUG = -O1 -g
# CC = clang -fsanitize=address
# CC = clang -fsanitize=leak
CC = gcc
GSLC_INCLUDES = -I./GUIslice/src
INCLUDE = -I./src $(GSLC_INCLUDES) $(TOUCHAPP_INCLUDES)
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g -pthread -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64
LDFLAGS = -L/usr/local/lib -L/opt/vc/lib -Llibs
LDLIB_EXTRA = -lwiringPi -lconfig -liw -lmpv -lxml2 -lsystemd -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos \
-lvchiq_arm -lpthread -Lsrc/libs/jsmn -ljsmn -lsqlite3 -lulfius -lorcania -lasound -lxdo -lavahi-client -lavahi-common

GSLC_CORE := GUIslice/src/GUIslice.c $(wildcard GUIslice/src/elem/*.c) #GUIslice/GUIslice_config.h


TOUCHAPP_INCLUDES = -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host -I/opt/vc/include/interface/vmcs_host/linux
TOUCHAPP_CORE := $(wildcard src/libs/*.c) $(wildcard src/libs/**/*.c) $(wildcard src/gui/*.c) $(wildcard src/gui/**/*.c) \
		         $(wildcard src/wpa/*.c)

APP_GUI =
APP_MODULES =

GSLC_DRV = SDL1

ifndef GSLC_TOUCH
  GSLC_LDLIB_EXTRA = -lm -lts
else
  ifeq (TSLIB,${GSLC_TOUCH})
    GSLC_LDLIB_EXTRA = -lm -lts
  else
    GSLC_LDLIB_EXTRA = -lm
  endif
endif

# === SDL1.2 ===
ifeq (SDL1,${GSLC_DRV})
  GSLC_SRCS = GUIslice/src/GUIslice_drv_sdl.c
  # - Add extra linker libraries if needed
  LDLIBS = src/libs/SDL/libSDL.a -ldl -lSDL_ttf ${GSLC_LDLIB_EXTRA}
endif

## === SDL2.0 ===
#ifeq (SDL2,${GSLC_DRV})
#  $(info GUIslice driver mode: SDL2)
#  GSLC_SRCS = GUIslice/src/GUIslice_drv_sdl.c
#  # - Add extra linker libraries if needed
#  LDLIBS = -lSDL2 -lSDL2_ttf ${GSLC_LDLIB_EXTRA}
#endif

all: bin/sdobox

TOUCHAPP_SRCS=src/touchapp.c $(TOUCHAPP_CORE) $(GSLC_CORE) $(GSLC_SRCS)
TOUCHAPP_OBJS=$(patsubst %.c,%.o,$(TOUCHAPP_SRCS))

ifeq ($(filter clean cleaner,$(MAKECMDGOALS)),)
  include $(patsubst %.c,%.dep,$(TOUCHAPP_SRCS))
endif

%.dep: %.c
	$(COMPILE.c) -MM $< | sed 's&^.*:&$(@) $(*).o:&' $(if $(REBUILD_ON_MAKEFILE_CHANGES),| sed 's/:/ $@: Makefile/') > $@

clean:
	@echo "Cleaning directory..."
	$(RM) bin/sdobox $(TOUCHAPP_OBJS) $(patsubst %.c,%.dep,$(TOUCHAPP_SRCS)) TAGS

src/wpa/wpa_ctrl.o:
	@echo [Building $@]
	gcc -Wall -Wextra -I./src/wpa/ -MMD -c -g -o ./src/wpa/wpa_ctrl.o ./src/wpa/wpa_ctrl.c -D CONFIG_CTRL_IFACE -D CONFIG_CTRL_IFACE_UNIX

src/wpa/os_unix.o:
	@echo [Building $@]
	gcc -Wall -Wextra -I./src/wpa/ -MMD -c -g -o ./src/wpa/os_unix.o ./src/wpa/os_unix.c -D CONFIG_CTRL_IFACE -D CONFIG_CTRL_IFACE_UNIX

bin/sdobox: $(TOUCHAPP_OBJS)
	@echo [Building $@]
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) $(LDLIB_EXTRA)

tags:
	etags $(TOUCHAPP_SRCS) *.h GUIslice/src/GUIslice_config.h $(shell find src/libs -name '*.h') $(shell find src/gui -name '*.h') wpa/*.h
