CC=gcc
CFLAGS=-Wall -O9
LDFLAGS=
SOURCES=howtouse.c pack.c pfordelta.c s16.c unpack.c coding_policy.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=howtouse

debug: CFLAGS+=-g
debug: LDFLAGS+=-g

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
