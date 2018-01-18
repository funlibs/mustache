# mustache Makefile
#
# make: will build libmustache.a
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


# TESTS directory
.PHONY: tests check profile

SUBOPTS += CFLAGS="$(CFLAGS)"
SUBOPTS += LDFLAGS="$(LDFLAGS)"
SUBOPTS += MUSTACHE_DIR="$(MUSTACHE_DIR)"
SUBOPTS += RM="$(RM)"

tests: libmustache.a
	@$(MAKE) -C tests $(SUBOPTS) clean tests

check: libmustache.a
	@$(MAKE) -C tests $(SUBOPTS) clean check

profile: libmustache.a
	@$(MAKE) -C tests $(SUBOPTS) clean profile


# clean
.PHONY: clean

clean:
	$(RM) $(OBJECTS) libmustache.a
	$(MAKE) -C tests $(SUBOPTS) clean
