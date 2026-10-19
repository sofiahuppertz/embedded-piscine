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

/* ----------------------------- ADC ------------------------------------ */

// Analog inputs on the devkit (schematic nets ADC_POT / ADC_LDR / ADC_NTC): MUX3:0 values, Table 24-4
# define ADC_POT        0   // RV1 on PC0 / ADC0
# define ADC_LDR        1   // R14 on PC1 / ADC1
# define ADC_NTC        2   // R20 on PC2 / ADC2

void    adc_init(void);
uint8_t read_adc(void);

#endif
