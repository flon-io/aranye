
CFLAGS=-I../src -g -Wall
#CFLAGS=-I../src -g -Wall -O3
LDLIBS=
CC=c99
VPATH=../src

RODZO=$(shell which rodzo)
ifeq ($(RODZO),)
  RODZO=../../rodzo/bin/rodzo
endif

NAMES=flutil.o aabro.o fara_dom.o fara_haml.o


s.c: ../spec/*_spec.c
	$(RODZO) ../spec -o s.c

s: $(NAMES)

spec: s
	time ./s

vspec: s
	valgrind --leak-check=full -v ./s
	#valgrind --read-var-info=yes --track-origins=yes --leak-check=full -v ./s

dspec: s
	gdb ./s

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.c
	rm -f s
	rm -f fara
	rm -f ../bin/fara

.PHONY: spec vspec dspec clean

fara: $(NAMES)

