# Mustache
#
# No build dependencies.
#
# To deploy use the striped version of libmustache.a (in this directory),
# and src/mustache.h
#

export MUSTACHE_LIB    := libmustache.a
export MUSTACHE_LIBDIR := $(shell pwd)/src
export MUSTACHE_INCDIR := $(shell pwd)/src

export CFLAGS  = -ggdb -O2 -Wall -I$(MUSTACHE_INCDIR)
export LDFLAGS = -L$(MUSTACHE_LIBDIR) -lmustache
export ARFLAGS = rcs

export CP 		:= cp
export CC 		:= cc
export AR 		:= ar
export RM 		:= rm -f
export MAKE  	:= make
export STRIP 	:= strip --strip-all

.PHONY: lib
lib: $(MUSTACHE_LIB)
	strip $(MUSTACHE_LIB)

$(MUSTACHE_LIB):
	$(MAKE) -C src $(MUSTACHE_LIB)
	$(CP) src/$(MUSTACHE_LIB) $(MUSTACHE_LIB)


.PHONY: clean
clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean
	$(RM) $(MUSTACHE_LIB)

.PHONY: tests
tests: lib
	$(MAKE) -C tests

.PHONY: check
check: lib
	$(MAKE) -C tests check

