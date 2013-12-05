

SOURCES=events_sender.c main.c client.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=events

CFLAGS=-ggdb
LIBS=-lX11 -lXtst

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	gcc $(OBJECTS) $(LIBS) -o $@

.c.o:
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm $(EXECUTABLE)
	rm *.o
