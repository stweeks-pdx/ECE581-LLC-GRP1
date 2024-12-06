DEBUG?=false
ifeq ($(DEBUG), true)
	TRACEFLAGS="-DDEBUG"
else
	TRACEFLAGS=
endif

IDIR=../include
CC=gcc
CFLAGS=-I$(IDIR) -std=c99 -Wall
ODIR=obj
EXE=bin


_DEPS = defines.h trace.h cache.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o trace.o cache.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(EXE):
	mkdir -p $@

$(ODIR)/%.o: %.c $(DEPS) | $(EXE)
	$(CC) $(TRACEFLAGS) -c -o $@ $< $(CFLAGS)

build: $(OBJ) | $(EXE)
	$(CC) -o $(EXE)/llc $^ $(CFLAGS)

.PHONY: clean all

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR) /*~
	rm -f $(EXE)/*

