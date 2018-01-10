ARFLAGS = rcs
RM = rm -f

OBJECTS = utils.o load.o expand.o
libmustache.a: $(OBJECTS)
	$(AR) $(ARFLAGS) libmustache.a $(OBJECTS)

.PHONY: clean depend
clean:
	$(RM) $(OBJECTS) libmustache.a

# Do not edit, generated with: cc -E -MM *.c
expand.o: expand.c mustache.h
load.o: load.c mustache.h
utils.o: utils.c mustache.h
