
CC= gcc
CFLAGS= -c -Wall
LDFLAGS= -lGeoIP
SOURCES= test-geoip-addr-lookup.c
OBJECTS= $(SOURCES:.c=.o)
EXECUTABLE= test-geoip-addr-lookup


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.c.o: 
		$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -rf *.o
