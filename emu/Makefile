CC=gcc
CFLAGS=-c -Wall -I../include
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=emu

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf *.o
	rm -rf $(EXECUTABLE)
	

.c.o:
	$(CC) $(CFLAGS) $< -o $@


