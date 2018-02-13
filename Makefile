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

CWD     != pwd
CFLAGS  = -ggdb -O2 -Wall -I$(CWD)
LDFLAGS = -L$(CWD) -lmustache
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

# TESTS directory
.PHONY: tests check profile

SUBOPTS += CFLAGS="$(CFLAGS)"
SUBOPTS += LDFLAGS="$(LDFLAGS)"
SUBOPTS += MUSTACHE_DIR="$(MUSTACHE_DIR)"
SUBOPTS += RM="$(RM)"

tests: libmustache.a
	@$(MAKE) -C tests $(SUBOPTS)

check: libmustache.a
	@$(MAKE) -C tests $(SUBOPTS) check

profile: libmustache.a
	@$(MAKE) -C tests $(SUBOPTS) profile


# clean
.PHONY: clean

clean:
	$(RM) $(OBJECTS) libmustache.a
	$(MAKE) -C tests $(SUBOPTS) clean
