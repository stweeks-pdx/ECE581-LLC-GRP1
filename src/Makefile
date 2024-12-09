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
TGDIR=../testing/GenFiles/

_DEPS = defines.h trace.h cache.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o trace.o cache.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TEST = TestGen.o
TEST = $(patsubst %,$(TGDIR)/%,$(_TEST))

$(EXE):
	mkdir -p $@

$(ODIR)/%.o: %.c $(DEPS) | $(EXE)
	$(CC) $(TRACEFLAGS) -DBYTESELECTWIDTH=6 -DASSOCIATIVITY=16 -DSETS=16*1024 -c -o $@ $< $(CFLAGS)

build: $(OBJ) | $(EXE)
	$(CC) -o $(EXE)/llc $^ $(CFLAGS)

testgen: $(TEST)
	$(CC) -o $(TGDIR)/TestGen $(TEST) $(CFLAGS) 

.PHONY: clean all

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR) /*~
	rm -f $(EXE)/*

