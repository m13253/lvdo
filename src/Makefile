
CC=gcc
CFLAGS+=-Wall -O3 -g3 $(shell pkg-config --cflags fftw3 glib-2.0)
LDFLAGS+=$(shell pkg-config --libs fftw3 glib-2.0)

.PHONY: all clean

all: lvdoenc lvdodec

lvdoenc: lvdoenc.o lvdomain.o

lvdodec: lvdodec.o lvdomain.o

clean:
	rm -f lvdoenc lvdodec lvdomain.o lvdoenc.o lvdodec.o
