MYCFLAGS= $(shell pkg-config --cflags cairo libpng) -fPIC -Wall -g -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
LDLIBS	= $(shell pkg-config --libs cairo libpng) -g -fPIC

NAME    = chartesque
HEADER  = $(NAME).h
OBJECTS = strlcpy.o dataplot.o axis.o

all: demo1

demo1: $(OBJECTS) demo1.o
	$(CC) -o demo1 $(LDLIBS) demo1.o $(OBJECTS)

%.o:%.c
	$(CC) $(CFLAGS) $(MYCFLAGS) -c $^

clean:
	rm -f $(PROGRAM) $(OBJECTS)
