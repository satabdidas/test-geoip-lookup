
CC= gcc
CFLAGS= -g -c -Wall -I/usr/include/ -I/usr/include/json-glib-1.0 -I/home/satabdi/src/myopt/gnome/include/glib-2.0 -I/home/satabdi/src/myopt/gnome/lib/glib-2.0/include -I/usr/include/libsoup-2.4
LDFLAGS= -L/home/satabdi/src/myopt/gnome/lib -lGeoIP -ljson-glib-1.0 -lsoup-2.4 -lgobject-2.0 -lglib-2.0
SOURCES= test-geoip-addr-lookup.c
OBJECTS= $(SOURCES:.c=.o)
EXECUTABLE= test-geoip-addr-lookup.cgi


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.c.o: 
		$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -rf *.o
