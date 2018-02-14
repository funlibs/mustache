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

OBJECTS = mustache_utils.o mustache_load.o mustache_expand.o

libmustache.a: $(OBJECTS)
	$(AR) rcs libmustache.a $(OBJECTS)

mustache_expand.o: mustache_expand.c mustache.h
mustache_load.o: mustache_load.c mustache.h
mustache_utils.o: mustache_utils.c mustache.h
tests.o: tests.c mustache.h
tests: tests.o
	$(CC) tests.o -o tests $(LDFLAGS)

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

