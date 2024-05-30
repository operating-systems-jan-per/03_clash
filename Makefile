CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

# Object files to build
OBJS = clash.o plist.o

# Default target
all: clash

# Executable target
clash: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Object file for clash.c
clash.o: clash.c
	$(CC) $(CFLAGS) -c $<

# Object file for plist.c
plist.o: plist.c plist.h
	$(CC) $(CFLAGS) -c $<

# Clean up build artifacts
clean:
	rm -f clash $(OBJS)
