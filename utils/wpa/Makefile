SOURCES = wpa.c
CC = gcc
CFLAGS = -I../../ ../../wpa/*.o
all: wpa

# Compile the binary 't' by calling the compiler with cflags, lflags, and any libs (if defined) and the list of objects.
wpa: $(SOURCES)
	@echo [Building $@]
	$(CC) $(CFLAGS) -o $@ $^
