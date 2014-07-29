
CC=gcc
override CFLAGS+=-Wall -O3 $(shell pkg-config --cflags fftw3 glib-2.0)
override LDFLAGS+=$(shell pkg-config --libs fftw3 glib-2.0) -lm

.PHONY: all clean

all: lvdoenc lvdodec

lvdoenc: lvdoenc.o lvdomain.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
lvdodec: lvdodec.o lvdomain.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^
lvdoenc.c: lvdocommon.h
lvdodec.c: lvdocommon.h

clean:
	rm -f lvdoenc lvdodec lvdomain.o lvdoenc.o lvdodec.o
