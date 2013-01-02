
CC= gcc
CFLAGS= -g -c -Wall -I/usr/include/ -I/usr/include/json-glib-1.0 -I/home/satabdi/src/myopt/gnome/include/glib-2.0 -I/home/satabdi/src/myopt/gnome/lib/glib-2.0/include
LDFLAGS= -L/home/satabdi/src/myopt/gnome/lib -lGeoIP -ljson-glib-1.0 -lgobject-2.0 -lglib-2.0
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
