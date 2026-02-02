/*  TM1637 driver using libgpiod */

#ifndef TM1637_GPIOD_H
#define TM1637_GPIOD_H

#include <stdint.h>

// GPIO pin configuration (can be overridden at compile time)
#ifndef DIO_PIN
#define DIO_PIN  24
#endif

#ifndef CLK_PIN
#define CLK_PIN  23
#endif

extern int TM1637_init(void);
extern void TM1637_cleanup(void);
extern int TM1637_write_num(int16_t num);
extern int TM1637_write_err(void);
extern int TM1637_write_waiting(void);
extern int TM1637_write_stale(void);

#endif /* TM1637_GPIOD_H */
