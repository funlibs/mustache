# Mustache
#
# No build dependencies.
#
# To use, link with the striped version of libmustache.a (in this directory),
# and src/mustache.h.
#
# For a shared library, you will have to build it.
#

CWD != pwd

MUSTACHE_LIBDIR := $(CWD)/src
MUSTACHE_INCDIR := $(CWD)/src

CFLAGS  = -ggdb -O3 -Wall -I$(MUSTACHE_INCDIR)
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


.PHONY: clean
clean:
	$(MAKE) -C src $(SUBOPTS) clean
	$(MAKE) -C tests $(SUBOPTS) clean
	$(RM) libmustache.a

.PHONY: tests
tests: lib
	$(MAKE) -C tests $(SUBOPTS)

.PHONY: check
check: libmustache.a
	$(MAKE) -C tests $(SUBOPTS) check

