

SOURCES=tcpserver.c btserver.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=btserver

CFLAGS=-O2
LIBS=-lpthread

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	gcc $(OBJECTS) $(LIBS) -o $@

.c.o:
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm $(EXECUTABLE)
	rm *.o