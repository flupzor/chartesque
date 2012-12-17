MYCFLAGS= $(shell pkg-config --cflags cairo libpng) -fPIC -Wall -g -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
LDLIBS	= $(shell pkg-config --libs cairo libpng) -g -fPIC

NAME    = chartesque
HEADER  = $(NAME).h
OBJECTS = strlcpy.o dataplot.o axis.o
DEMOBJS = chartesque.o

all: $(OBJECTS) $(DEMOBJS)
	$(CC) -o chartesque $(LDLIBS) $(DEMOBJS) $(OBJECTS)

%.o:%.c
	$(CC) $(CFLAGS) $(MYCFLAGS) -c $^

clean:
	rm -f $(PROGRAM) $(OBJECTS)
