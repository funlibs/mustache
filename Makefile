# mustache Makefile
#
# make all: will build libmustache.a
# make tests: will build the tests
# make check: will run them
# make clean: clean everything
# make help: will show this text
#
# To use it, staticaly link libmustache.a and include mustache.h.
#
# Example that does nothing:
# $ cat myprog.c
# #include "mustache.h"
# int main(int argc, char **argv) {
#     RessourceStore *s = Mstc_ressource_create();
#     Mstc_ressource_free(s);
#     return 0;
# }
#
# $ cc -I. -static myprog.c -L. -lmustache -o myprog

CFLAGS  = -ansi -Wall -ggdb -O2 -I$(PWD)
LDFLAGS = -L$(PWD) -lmustache
STRIP   = strip --strip-unneeded
RM      = rm -f

all: libmustache.a
	$(STRIP) libmustache.a

OBJECTS = utils.o load.o expand.o

libmustache.a: $(OBJECTS)
	$(AR) rcs libmustache.a $(OBJECTS)

expand.o: expand.c mustache.h
	$(CC) $(CFLAGS) -c expand.c -o expand.o

load.o: load.c mustache.h
	$(CC) $(CFLAGS) -c load.c -o load.o

utils.o: utils.c mustache.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o

tests: tests.c libmustache.a
	$(CC) $(CFLAGS) tests.c $(LDFLAGS) -o tests


.PHONY: clean check profile
clean:
	$(RM) $(OBJECTS) libmustache.a tests samples/perf.data*

check: tests
	cd samples; ../tests

profile: tests
	cd samples; sudo perf stat -d ../tests 300000 > /dev/null
	@echo "Press enter to continue"; read A
	cd samples; sudo perf record -e task-clock,cycles,instructions,cache-references,cache-misses,branches,branch-misses ../tests 300000 > /dev/null
	cd samples; sudo perf report
	@#cd samples; valgrind ../tests.bin 10 > /dev/null;

