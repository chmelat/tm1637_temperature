# CLAUDE.md

## Project Overview

TM1637 temperature display using libgpiod and MQTT. Subscribes to MQTT broker for temperature data, displays with 0.1°C resolution.

Works on any Linux SBC with /dev/gpiochip0 (RPi 1-5, Orange Pi, etc.)

## Build

```bash
make                       # compile
make debug                 # with -g -DDEBUG
sudo make install          # to /usr/local/bin
sudo make install-service  # + systemd
sudo make uninstall        # remove all
make check-deps            # verify dependencies
```

Custom pins: `make CFLAGS="-DDIO_PIN=18 -DCLK_PIN=19"`

## Usage

```bash
./tm1637_temperature -b <broker> -t <topic> [-p port] [-i interval] [-h]
  -b broker   MQTT broker hostname/IP (required)
  -t topic    MQTT topic for temperature (required)
  -p port     MQTT port (default: 1883)
  -i interval Display update interval in seconds (default: 1)
  -h          Display this help
```

Example:
```bash
./tm1637_temperature -b 192.168.200.12 -t sensors/r4dcb08/1/temperature/ch1
```

## Architecture

```
main.c         - CLI (-b broker, -t topic, -p port, -i interval), signal handling
tm1637_gpiod.c - TM1637 protocol, open-drain via libgpiod
tm1637_gpiod.h - pin definitions (DIO=24, CLK=23)
mqtt_temp.c    - MQTT subscriber, returns temp*10
mqtt_temp.h    - TEMP_ERROR (-9999), TEMP_NO_DATA (-9998)
```

## Hardware

- 4.7kΩ pull-ups on CLK/DIO (mandatory)
- 3.3V only (5V damages GPIO)
- Default: GPIO23=CLK, GPIO24=DIO

## Dependencies

```bash
sudo apt install libgpiod-dev libmosquitto-dev
```
