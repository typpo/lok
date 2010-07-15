CC=gcc
CFLAGS=-c -Wall -pedantic -std=c99 -g -DSQLITE_HAS_CODEC
LDFLAGS=-L/usr/local/lib -lncurses -lmenu -lsqlite3
SOURCES=main.c writing.c db.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=lok

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *~ lok
