CC=gcc
CFLAGS = -c -Wall -I../../../common/ $(shell pkg-config --cflags libzmq)
LDFLAGS = $(shell pkg-config --libs libzmq) -lczmq

SOURCES=server.c ../../../common/common.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=server

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE)
	rm -rf *.o