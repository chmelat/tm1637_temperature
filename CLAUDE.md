# CLAUDE.md

## Project Overview

TM1637 temperature display using libgpiod. Reads from `/usr/local/bin/r4dcb08`, displays with 0.1°C resolution.

Works on any Linux SBC with /dev/gpiochip0 (RPi 1-5, Orange Pi, etc.)

## Build

```bash
make                       # compile
make debug                 # with -g -DDEBUG
sudo make install          # to /usr/local/bin
sudo make install-service  # + systemd
sudo make uninstall        # remove all
make check-libgpiod        # verify dependency
```

Custom pins: `make CFLAGS="-DDIO_PIN=18 -DCLK_PIN=19"`

## Architecture

```
main.c         - CLI (-i interval, -h), signal handling
tm1637_gpiod.c - TM1637 protocol, open-drain via libgpiod
tm1637_gpiod.h - pin definitions (DIO=24, CLK=23)
get_temp.c     - executes r4dcb08, returns temp*10
get_temp.h     - TEMP_ERROR (-9999)
```

## Hardware

- 4.7kΩ pull-ups on CLK/DIO (mandatory)
- 3.3V only (5V damages GPIO)
- Default: GPIO23=CLK, GPIO24=DIO

## Dependency

```bash
sudo apt install libgpiod-dev
```
