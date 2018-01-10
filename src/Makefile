ARFLAGS = rcs
RM = rm -f

OBJECTS = utils.o load.o expand.o
libmustache.a: $(OBJECTS)
	$(AR) $(ARFLAGS) libmustache.a $(OBJECTS)

.PHONY: clean
clean:
	$(RM) $(OBJECTS) libmustache.a

expand.o: expand.c mustache.h
load.o: load.c mustache.h
utils.o: utils.c mustache.h
