#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "mqtt_temp.h"
#include "tm1637_gpiod.h"

#define DEFAULT_PORT 1883
#define DEFAULT_INTERVAL 60

static volatile sig_atomic_t running = 1;

void sigint_handler(int sig) {
    (void)sig;
    running = 0;
}

/*
 * Parse interval argument with validation
 * Returns interval value on success, -1 on error
 */
static int parse_interval(const char *str) {
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);

    if (errno != 0 || endptr == str || *endptr != '\0') {
        return -1;
    }
    if (val <= 0 || val > INT_MAX) {
        return -1;
    }
    return (int)val;
}

/*
 * Parse port argument with validation
 * Returns port value on success, -1 on error
 */
static int parse_port(const char *str) {
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);

    if (errno != 0 || endptr == str || *endptr != '\0') {
        return -1;
    }
    if (val <= 0 || val > 65535) {
        return -1;
    }
    return (int)val;
}

/*
 * Print usage information
 */
void print_usage(const char *progname) {
    printf("Usage: %s -b <broker> -t <topic> [-p port] [-i interval] [-h]\n", progname);
    printf("  -b broker   MQTT broker hostname/IP (required)\n");
    printf("  -t topic    MQTT topic for temperature (required)\n");
    printf("  -p port     MQTT port (default: %d)\n", DEFAULT_PORT);
    printf("  -i interval Display update interval in seconds (default: %d)\n", DEFAULT_INTERVAL);
    printf("  -h          Display this help\n");
}

/*
 * Main
 */
int main(int argc, char *argv[]) {

    int16_t temp;
    char *broker = NULL;
    char *topic = NULL;
    int port = DEFAULT_PORT;
    int interval = DEFAULT_INTERVAL;
    int opt;

    while ((opt = getopt(argc, argv, "b:t:p:i:h")) != -1) {
        switch (opt) {
            case 'b':
                broker = optarg;
                break;
            case 't':
                topic = optarg;
                break;
            case 'p':
                port = parse_port(optarg);
                if (port < 0) {
                    fprintf(stderr, "Error: Port must be 1-65535\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'i':
                interval = parse_interval(optarg);
                if (interval < 0) {
                    fprintf(stderr, "Error: Interval must be a positive integer\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case '?':
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (!broker || !topic) {
        fprintf(stderr, "Error: -b broker and -t topic are required\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    printf("Temperature display on TM1637 (MQTT subscriber)\n");
    printf("================================================\n");
    printf("Broker: %s:%d\n", broker, port);
    printf("Topic: %s\n", topic);
    printf("Update interval: %d seconds\n", interval);
    printf("Press Ctrl+C to exit\n\n");

    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

    if (TM1637_init() < 0) {
        fprintf(stderr, "Error initializing TM1637!\n");
        return EXIT_FAILURE;
    }

    if (mqtt_init(broker, port, topic, interval * 2) < 0) {
        fprintf(stderr, "Error initializing MQTT!\n");
        TM1637_cleanup();
        return EXIT_FAILURE;
    }

    while (running) {
        mqtt_loop(100);

        temp = mqtt_get_temp();
        if (temp == TEMP_NO_DATA) {
            TM1637_write_waiting();
        } else if (temp == TEMP_STALE) {
            TM1637_write_stale();
        } else if (temp == TEMP_ERROR) {
            TM1637_write_err();
        } else {
            TM1637_write_num(temp);
        }

        for (int i = 0; i < interval * 10 && running; i++) {
            mqtt_loop(100);
        }
    }

    printf("\nExiting program...\n");
    mqtt_cleanup();
    TM1637_cleanup();
    return EXIT_SUCCESS;
}
