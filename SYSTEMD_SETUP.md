# Systemd Setup for TM1637 Temperature Display

## Installation and Configuration

### 1. Project Preparation
```bash
# Compile project
make

# Test manually (connect display with pull-up resistors)
sudo ./tm1637_temperature
```

### 2. Systemd Service Installation
```bash
# Copy service file
sudo cp tm1637_temperature.service /etc/systemd/system/

# Reload configuration
sudo systemctl daemon-reload

# Enable autostart on boot
sudo systemctl enable tm1637_temperature.service

# Start service
sudo systemctl start tm1637_temperature.service
```

### 3. Configuration Verification
```bash
# Check service status
sudo systemctl status tm1637_temperature.service

# Monitor logs in real-time
sudo journalctl -u tm1637_temperature -f

# Display recent logs
sudo journalctl -u tm1637_temperature -n 20
```

## Expected Behavior

### With Connected Display
```
● tm1637_temperature.service - TM1637 Temperature Display Service
   Loaded: loaded
   Active: active (running)

Logs:
TM1637 pins validated and initialized (GPIO 24=DIO, GPIO 23=CLK)
```

### Without Connected Display
```
● tm1637_temperature.service - TM1637 Temperature Display Service
   Loaded: loaded
   Active: failed (Result: start-limit-hit)

Logs:
Error: Missing pull-up resistors
GPIO 24 (DIO): LOW, GPIO 23 (CLK): LOW
```

**Behavior:** 2 attempts after 10 minutes, then 24h pause

## Service Management

### Basic Commands
```bash
# Restart service
sudo systemctl restart tm1637_temperature.service

# Stop service
sudo systemctl stop tm1637_temperature.service

# Start service
sudo systemctl start tm1637_temperature.service

# Disable autostart
sudo systemctl disable tm1637_temperature.service

# Enable autostart
sudo systemctl enable tm1637_temperature.service
```

### When Connecting Display (if service is stopped)
```bash
# Manual restart
sudo systemctl restart tm1637_temperature.service

# Or reset start-limit and start
sudo systemctl reset-failed tm1637_temperature.service
sudo systemctl start tm1637_temperature.service
```

### Troubleshooting
```bash
# Detailed status
sudo systemctl status tm1637_temperature.service -l

# All logs since last boot
sudo journalctl -u tm1637_temperature -b

# Logs with timestamps
sudo journalctl -u tm1637_temperature --since "1 hour ago"

# Test systemd file syntax
sudo systemd-analyze verify /etc/systemd/system/tm1637_temperature.service
```

## Hardware Requirements

- TM1637 CLK → GPIO 23 + 4.7kΩ pull-up to 3.3V
- TM1637 DIO → GPIO 24 + 4.7kΩ pull-up to 3.3V
- External temperature sensor binary `r4dcb08` in project directory
- Root/sudo access for GPIO operations

## Notes

- Service requires root privileges for GPIO access
- Pull-up resistors are mandatory for connected display detection
- Service automatically restarts when display is connected during first 20 minutes after boot
- After start-limit-hit, manual service restart is required
