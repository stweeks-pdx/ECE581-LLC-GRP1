DEBUG?=false
ifeq ($(DEBUG), true)
	TRACEFLAGS="-DDEBUG"
else
	TRACEFLAGS=
endif

IDIR=../include
CC=gcc
CFLAGS=-I$(IDIR) -std=c99

ODIR=obj


_DEPS = defines.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(TRACEFLAGS) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o LLC $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR) /*~
