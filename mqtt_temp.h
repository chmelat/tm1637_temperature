#ifndef MQTT_TEMP_H
#define MQTT_TEMP_H

#include <stdint.h>

#define TEMP_ERROR   -9999
#define TEMP_NO_DATA -9998
#define TEMP_STALE   -9997

// Initialize MQTT connection and subscribe to topic
// watchdog_timeout - seconds without message before data is considered stale (0 = disabled)
// Returns 0 on success, -1 on error
int mqtt_init(const char *broker, int port, const char *topic, int watchdog_timeout);

// Returns last received temperature in units of 0.1°C
// Returns TEMP_NO_DATA if no message received yet
// Returns TEMP_ERROR on parse error
int16_t mqtt_get_temp(void);

// Process MQTT events (call in main loop)
// timeout_ms - maximum wait time in milliseconds
// Returns 0 on success, -1 on connection error
int mqtt_loop(int timeout_ms);

// Disconnect and cleanup MQTT resources
void mqtt_cleanup(void);

#endif /* MQTT_TEMP_H */
