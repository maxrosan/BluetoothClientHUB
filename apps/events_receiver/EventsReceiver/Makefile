

SOURCES=events_sender.c main.c client.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=events

JSONGLIB_CFLAGS=`pkg-config --cflags json-glib-1.0`
JSONGLIB_LIBS=`pkg-config --libs json-glib-1.0`

CFLAGS=-ggdb $(JSONGLIB_CFLAGS)
LIBS=-lX11 -lXtst $(JSONGLIB_LIBS)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	gcc $(OBJECTS) $(LIBS) -o $@

.c.o:
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm $(EXECUTABLE)
	rm *.o
