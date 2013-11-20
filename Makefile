

#SOURCES=msg.c queue.c tcpserver.c btlib.c btserver.c
SOURCES= msg.c queue.c listener.c btlib.c tcpserver.c btserver.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=btserver

JSONGLIB_CFLAGS=`pkg-config --cflags json-glib-1.0`
JSONGLIB_LIBS=`pkg-config --libs json-glib-1.0`

CFLAGS=-ggdb $(JSONGLIB_CFLAGS)
LIBS=-lpthread -lbluetooth $(JSONGLIB_LIBS)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	gcc $(OBJECTS) $(LIBS) -o $@

.c.o:
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm $(EXECUTABLE)
	rm *.o