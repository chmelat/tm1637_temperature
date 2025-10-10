#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>

#include "get_temp.h"
#include "tm1637_rpi_pigpio.h"

#define DEFAULT_INTERVAL 60

static volatile bool running = true;

void sigint_handler(int sig) {
    (void)sig;
    running = false;
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
                interval = atoi(optarg);
                if (interval <= 0) {
                    fprintf(stderr, "Error: Interval must be a positive number\n");
                    return 1;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case '?':
                print_usage(argv[0]);
                return 1;
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
        printf("Error initializing pigpio library!\n");
        return -1;
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
    return 0;
}
