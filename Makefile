MYCFLAGS= $(shell pkg-config --cflags cairo libpng) -fPIC -Wall -g -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
LDLIBS	= $(shell pkg-config --libs cairo libpng) -g -fPIC

NAME    = chartesque
HEADER  = $(NAME).h
OBJECTS = strlcpy.o dataplot.o axis.o

all: demo1 demo2

demo1: $(OBJECTS) demo1.o
	$(CC) -o demo1 $(LDLIBS) demo1.o $(OBJECTS)

demo2: $(OBJECTS) demo2.o gtkwidget.o
	$(CC) $(shell pkg-config --libs gtk+-2.0) -o demo2 demo2.o gtkwidget.o $(OBJECTS)

gtkwidget.o: gtkwidget.c
	$(CC) $(CFLAGS) $(shell pkg-config --cflags gtk+-2.0) -c $^

demo2.o: demo2.c
	$(CC) $(CFLAGS) $(shell pkg-config --cflags gtk+-2.0) -c $^

%.o:%.c
	$(CC) $(CFLAGS) $(MYCFLAGS) -c $^

clean:
	rm -f demo1 demo2 gtkwidget.o demo1.o demo2.o $(OBJECTS)
