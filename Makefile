CC=gcc
CFLAGS=-Wall
LDFLAGS=
SOURCES=howtouse.c  pack.c      pfordelta.c s16.c       unpack.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=howtouse

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
