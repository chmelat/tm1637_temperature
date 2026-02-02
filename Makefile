# Makefile pro TM1637 teploměr (libgpiod + MQTT)

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -D_DEFAULT_SOURCE
LDFLAGS = -lgpiod -lmosquitto

TARGET = tm1637_temperature
SOURCES = tm1637_gpiod.c mqtt_temp.c main.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = tm1637_gpiod.h mqtt_temp.h

PREFIX = /usr/local/bin

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) $(PREFIX)/

install-service: install
	cp $(TARGET).service /etc/systemd/system/
	systemctl daemon-reload
	systemctl enable --now $(TARGET)

uninstall:
	-systemctl disable --now $(TARGET) 2>/dev/null
	-rm -f /etc/systemd/system/$(TARGET).service $(PREFIX)/$(TARGET)
	-systemctl daemon-reload

debug: CFLAGS += -g -DDEBUG
debug: clean $(TARGET)

syntax-check:
	$(CC) $(CFLAGS) -fsyntax-only $(SOURCES)

check-deps:
	@pkg-config --exists libgpiod && echo "libgpiod OK" || echo "MISSING: sudo apt install libgpiod-dev"
	@pkg-config --exists libmosquitto && echo "libmosquitto OK" || echo "MISSING: sudo apt install libmosquitto-dev"

.PHONY: clean install install-service uninstall debug syntax-check check-deps
