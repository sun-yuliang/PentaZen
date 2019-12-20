# For mingw-w64 on Windows

CC = gcc
MAINSRCS = main.c
OTHSRCS = board.c display.c distance.c mvlist.c pattab.c protocol.c search.c test.c
HEADERS = bitboard.h board.h display.h distance.h index.h mvlist.h pattab.h protocol.h public.h search.h test.h
SRCS = $(MAINSRCS) $(OTHSRCS)
OBJS = $(SRCS:.c=.o)
TARGETS = pbrain-PentaZen.exe

# CFLAGS = -g -Wall
CFLAGS = -O3

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGETS)

$(TARGETS): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGETS) $(OBJS)
	del $(OBJS)

clean:
#	rm -f $(OBJS) $(TARGETS)
	del $(OBJS) $(TARGETS)

.PHONY: all clean
