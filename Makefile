
CC=gcc-4.4
override CFLAGS+=-Wall -O3 -g3 $(shell pkg-config --cflags fftw3 glib-2.0)
override LDFLAGS+=$(shell pkg-config --libs fftw3 glib-2.0) -lm

OBJDIR=build
SRCDIR=src

default: clean all postclean

all: $(OBJDIR)/lvdoenc $(OBJDIR)/lvdodec

%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(LDFLAGS) -o $@ $^

$(SRCDIR)/%.c: lvdocommon.h

$(OBJDIR):
	mkdir -p $@

# Dependencies
$(OBJDIR)/lvdoenc: $(OBJDIR)/lvdoenc.o $(OBJDIR)/lvdomain.o | $(OBJDIR)
$(OBJDIR)/lvdodec: $(OBJDIR)/lvdodec.o $(OBJDIR)/lvdomain.o | $(OBJDIR)
$(OBJDIR)/foo.o $(OBJDIR)/bar.o: lvdocommon.h | $(OBJDIR)

clean:
	mkdir -p build
	rm -rf build
	mkdir build

postclean:
	rm -f $(OBJDIR)/lvdoenc.o $(OBJDIR)/lvdodec.o $(OBJDIR)/lvdomain.o
