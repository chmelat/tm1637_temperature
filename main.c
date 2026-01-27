#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "get_temp.h"
#include "tm1637_rpi_pigpio.h"

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
        return -1;  /* Conversion error or trailing characters */
    }
    if (val <= 0 || val > INT_MAX) {
        return -1;  /* Out of valid range */
    }
    return (int)val;
}

/*
 * Print usage information
 */
void print_usage(const char *progname) {
    printf("Usage: %s [-i interval]\n", progname);
    printf("  -i interval  Measurement interval in seconds (default: %d)\n", DEFAULT_INTERVAL);
    printf("  -h          Display this help\n");
}

/*
 * Main
 */
int main(int argc, char *argv[]) {

    int16_t temp; /* Temperature [0.1C] */
    int interval = DEFAULT_INTERVAL;
    int opt;

    // Parse command line arguments
    while ((opt = getopt(argc, argv, "i:h")) != -1) {
        switch (opt) {
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

    printf("Temperature display on TM1637 display on Raspberry Pi 1 (pigpio)\n");
    printf("==================================================================\n");
    printf("Measurement interval: %d seconds\n", interval);
    printf("Press Ctrl+C to exit\n\n");

    // Set SIGINT handler
    signal(SIGINT, sigint_handler);

    // Initialization
    if (TM1637_init() < 0) {
        fprintf(stderr, "Error initializing pigpio library!\n");
        return EXIT_FAILURE;
    }


/* Measurement loop */
    while(running) {
      temp = get_temp();
      if (temp != TEMP_ERROR) {
        TM1637_write_num(temp);
      } else {
        TM1637_write_err();
      }

      // Interruptible wait
      for(int i = 0; i < interval && running; i++) {
        sleep(1);
      }
    }

    printf("\nExiting program...\n");
    TM1637_cleanup();
    return EXIT_SUCCESS;
}
