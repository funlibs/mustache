SOURCES = utils.c load.c expand.c
HEADERS = mustache.h
OBJECTS = $(SOURCES:.c=.o)

all: libmustache.a

libmustache.a: $(OBJECTS) $(HEADERS)
	$(AR) $(ARFLAGS) libmustache.a $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: libmustache.a

.PHONY: clean
clean:
	$(RM) *.o *.a *.bin perf.* cachegrind.out*
