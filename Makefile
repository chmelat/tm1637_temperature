# Makefile pro TM1637 teploměr (libgpiod)

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lgpiod

TARGET = tm1637_temperature
SOURCES = tm1637_gpiod.c get_temp.c main.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = tm1637_gpiod.h get_temp.h

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

check-libgpiod:
	@pkg-config --exists libgpiod && echo "libgpiod OK" || echo "libgpiod chybí: sudo apt install libgpiod-dev"

.PHONY: clean install install-service uninstall debug syntax-check check-libgpiod
