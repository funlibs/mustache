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
# #include "mustache_api.h"
# #include <stdlib.h>   /* free */
# #include <stdio.h>    /* printf */
#
# int main(int argc, char **argv) {
#
#     TemplateStore *s = Mstc_template_create();
#     Template *t = Mstc_template_get(s, "my.html.tpl");
#     Dict *d = Mstc_dict_new();
#
#     /* populate the dict with some values */
#     Mstc_dict_setValue(d, "title", "%s", "hello world");
#
#     Dict *sub = Mstc_dict_addSectionItem(d, "mylist);
#     Mstc_dict_setValue(sub, "elem1", "%s", "hello1");
#     Dict *sub = Mstc_dict_addSectionItem(d, "mylist);
#     Mstc_dict_setValue(sub, "elem2", "%s", "hello2");
#
#     char *out = Mstc_expand(t,d);
#     Mstc_dict_free(d);
#
#     printf("%s", out);
#     free(out);
#
#     Mstc_template_free(s);
#     return 0;
# }
#
# $ cc -I. -static myprog.c -L. -lmustache -o myprog

CFLAGS  = -std=c99 -Wall -ggdb -O2 -I$(PWD)
LDFLAGS = -L$(PWD) -lmustache
STRIP   = strip --strip-unneeded
RM      = rm -f

all: libmustache.a
	$(STRIP) libmustache.a

OBJECTS = mustache_utils.o mustache_load.o mustache_expand.o tests.o

libmustache.a: $(OBJECTS)
	$(AR) rcs libmustache.a $(OBJECTS)

mustache_expand.o: mustache_expand.c mustache.h
mustache_load.o: mustache_load.c mustache.h
mustache_utils.o: mustache_utils.c mustache.h
tests.o: tests.c mustache.h
tests: tests.o libmustache.a
	$(CC) tests.o -o tests $(LDFLAGS)

.PHONY: clean check profile
clean:
	$(RM) $(OBJECTS) libmustache.a tests samples/perf.data*

check: tests
	./tests samples/sample.html.tpl

profile: tests
	sudo perf stat -d ./tests samples/sample.html.tpl 300000 > /dev/null
	@echo "Press enter to continue"; read A
	sudo perf record -e task-clock,cycles,instructions,cache-references,cache-misses,branches,branch-misses ./tests samples/sample.html.tpl 300000 > /dev/null
	sudo perf report
	@#cd samples; valgrind ../tests.bin 10 > /dev/null;

