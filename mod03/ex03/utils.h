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

/* ----------------------------- UART ----------------------------------- */

// Key codes as they arrive on the serial line (ASCII table)
# define KEY_CR         '\r'   // Enter in screen sends CR only
# define KEY_LF         '\n'   // Enter in other terminals
# define KEY_DEL        0x7f   // Backspace in screen / most terminals sends DEL
# define KEY_BS         0x08   // Backspace in some terminals sends BS
# define IS_ENTER(c)     ((c) == KEY_CR || (c) == KEY_LF)
# define IS_BACKSPACE(c) ((c) == KEY_DEL || (c) == KEY_BS)

// UBRR for double speed mode: F_CPU / (8 * baud) - 1 = 16 at 16 MHz, 115200 (Table 20-1, checked in Table 20-7)
# define UBRR_VALUE     ((F_CPU / 8 / UART_BAUDRATE) - 1)

void    uart_init(void);
void    uart_tx(char c);
void    uart_printstr(const char *str);

/* ----------------------------- RGB LED D5 ----------------------------- */

// Red on PD5, green on PD6, blue on PD3 (devkit schematic, nets LED_R / LED_G / LED_B)
# define RED            (1 << PD5)
# define GREEN          (1 << PD6)
# define BLUE           (1 << PD3)
# define RGB_MASK       (RED | GREEN | BLUE)

// Duty register of each colour. The LED pins are output-compare pins: PD5 = OC0B, PD6 = OC0A, PD3 = OC2B (Table 14-9, Port D alternate functions)
# define RED_PWM        OCR0B
# define GREEN_PWM      OCR0A
# define BLUE_PWM       OCR2B

void    init_rgb(void);
void    set_rgb(uint8_t r, uint8_t g, uint8_t b);
uint8_t gamma8(uint8_t v);

/* ----------------------------- Status LEDs D1..D4 --------------------- */

// D1..D4 are on PB0, PB1, PB2, PB4 (schematic; PB3 is MOSI/OC2A so D4 was moved to PB4)
# define LED_MASK       ((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4))

/* ----------------------------- Input line ----------------------------- */

// "#RRGGBB" is 7 characters, plus one slot for the '\0' terminator
# define BUF_SIZE       8

#endif
