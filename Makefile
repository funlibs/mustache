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
# Example:
# $ cat myprog.c
# #include "mustache.h"
# int main(int argc, char **argv) {
#     RessourceStore *s = Mstc_ressource_create();
#     Mstc_ressource_free(s);
#     return 0;
# }
#
# $ cc -Ipath_to_mustache.h_dir -static myprog.c -Lpath_to_libmustache.a_dir -lmustache -o myprog
# 
CWD != pwd

MUSTACHE_LIBDIR := $(CWD)/src
MUSTACHE_INCDIR := $(CWD)/src

CFLAGS  = -ggdb -O2 -Wall -I$(MUSTACHE_INCDIR)
LDFLAGS = -L$(MUSTACHE_LIBDIR) -lmustache
ARFLAGS = rcs

CP ?= cp
CC ?= cc
AR ?= ar
RM ?= rm -f
MAKE  = @make
STRIP = strip --strip-all

SUBOPTS += CC="$(CC)"
SUBOPTS += CP="$(CP)"
SUBOPTS += AR="$(AR)"
SUBOPTS += RM="$(RM)"
SUBOPTS += ARFLAGS="$(ARFLAGS)"
SUBOPTS += CFLAGS="$(CFLAGS)"
SUBOPTS += LDFLAGS="$(LDFLAGS)"
SUBOPTS += MAKE="$(MAKE)"
SUBOPTS += STRIP="$(STRIP)"
SUBOPTS += MUSTACHE_LIBDIR="$(MUSTACHE_LIBDIR)"
SUBOPTS += MUSTACHE_INCDIR="$(MUSTACHE_INCDIR)"
 
libmustache.a:
	$(MAKE) -C src $(SUBOPTS) libmustache.a
	$(STRIP) -o libmustache.a src/libmustache.a

libmustache.so:
	$(MAKE) -C src $(SUBOPTS) libmustache.so
	$(STRIP) -o libmustache.so src/libmustache.so

.PHONY: clean
clean:
	$(MAKE) -C src $(SUBOPTS) clean
	$(MAKE) -C tests $(SUBOPTS) clean
	$(RM) libmustache.a

.PHONY: tests
tests: libmustache.a
	$(MAKE) -C tests $(SUBOPTS)

.PHONY: check
check: libmustache.a
	$(MAKE) -C tests $(SUBOPTS) check
