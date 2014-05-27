
CC=gcc
override CFLAGS+=-Wall -O3 -g3 $(shell pkg-config --cflags fftw3 glib-2.0)
override LDFLAGS+=$(shell pkg-config --libs fftw3 glib-2.0) -lm

.PHONY: all clean

all: lvdoenc lvdodec

lvdoenc: lvdoenc.o lvdomain.o lvdocommon.o

lvdodec: lvdodec.o lvdomain.o lvdocommon.o

clean:
	rm -f lvdoenc lvdodec lvdomain.o lvdocommon.o lvdoenc.o lvdodec.o
