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
void    uart_print_dec(uint16_t v);

/* ----------------------------- ADC ------------------------------------ */

// Internal temperature sensor is wired to the mux as ADC8, no pin: MUX3:0 = 1000 (24.8, Table 24-4)
# define ADC_TEMP       8

void    adc_init(void);
uint16_t read_adc(void);
int16_t adc_to_celsius(uint16_t code);

#endif
