#ifndef UTILS_H
# define UTILS_H

// Build constants; defaults so util/delay.h and the macros below work even
// if the Makefile does not pass them with -D
# ifndef F_CPU
#  define F_CPU 16000000UL
# endif
# ifndef UART_BAUDRATE
#  define UART_BAUDRATE 115200
# endif

# include <avr/io.h>

/* ----------------------------- UART (TX only) ------------------------- */

// UBRR for double speed mode: F_CPU / (8 * baud) - 1 = 16 at 16 MHz, 115200 (Table 20-1, checked in Table 20-7)
# define UBRR_VALUE     ((F_CPU / 8 / UART_BAUDRATE) - 1)

void    uart_init(void);
void    uart_tx(char c);
void    uart_printstr(const char *str);
void    uart_print_hex(uint8_t v);

/* ----------------------------- I2C (TWI master) ----------------------- */

# include <util/twi.h>

// SCL = F_CPU / (16 + 2 * TWBR * prescaler), prescaler = 1 -> TWBR = 72 for 100 kHz at 16 MHz (22.5.2, 22.9.1)
# define TWBR_VALUE     ((F_CPU / 100000UL - 16) / 2)

// AHT20 7-bit slave address; shifted left by one on the bus, bit 0 = R/W (AHT20 datasheet 5.3, ATmega 22.3.3)
# define AHT20_ADDR     0x38

void    i2c_init(void);
void    i2c_start(void);
void    i2c_stop(void);
void    print_status(void);

#endif
