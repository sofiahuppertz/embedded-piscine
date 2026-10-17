// Build constants; defined here so util/delay.h can use them
#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart_utils.h"

#define USERNAME "sofia"
#define PASSWORD "1234"

// Key codes as they arrive on the serial line (ASCII table)
#define KEY_CR        '\r'   // Enter in screen sends CR only
#define KEY_LF        '\n'   // Enter in other terminals
#define KEY_DEL       0x7f   // Backspace in screen / most terminals sends DEL
#define KEY_BS        0x08   // Backspace in some terminals sends BS
#define IS_ENTER(c)     ((c) == KEY_CR || (c) == KEY_LF)
#define IS_BACKSPACE(c) ((c) == KEY_DEL || (c) == KEY_BS)

#define BUF_SIZE 32
static volatile char    user_buf[BUF_SIZE];
static volatile char    pass_buf[BUF_SIZE];
static volatile uint8_t idx  = 0;
static volatile uint8_t step = 0;   // 0 = typing username, 1 = typing password

// D1..D4 are on PB0, PB1, PB2, PB4 (schematic; PB3 is MOSI/OC2A so D4 was moved to PB4)
static const uint8_t leds[4] = {PB0, PB1, PB2, PB4};
#define LED_MASK ((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4))

// No string.h allowed: compare two NUL-terminated strings, 1 if equal
static uint8_t  str_eq(volatile const char *a, const char *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

// Reset the login state machine and ask for the username again
static void prompt(void) {
    idx = 0;
    step = 0;
    uart_printstr("Enter your login:\r\n    username: ");
}

// Dramatic effect: one LED sweeps left-right-left, then all four flash together
static void display_leds(void) {
    for (uint8_t round = 0; round < 3; round++) {
        for (int8_t i = 0; i < 4; i++) {
            PORTB = (PORTB & ~LED_MASK) | (1 << leds[i]);
            _delay_ms(80);
        }
        for (int8_t i = 2; i > 0; i--) {
            PORTB = (PORTB & ~LED_MASK) | (1 << leds[i]);
            _delay_ms(80);
        }
    }
    for (uint8_t i = 0; i < 3; i++) {
        PORTB |= LED_MASK;
        _delay_ms(200);
        PORTB &= ~LED_MASK;
        _delay_ms(200);
    }
}

static void check_credentials(void) {
    if (str_eq(user_buf, USERNAME) && str_eq(pass_buf, PASSWORD)) {
        uart_printstr("\r\nHello ");
        uart_printstr(USERNAME);
        uart_printstr("!\r\nShall we play a game?\r\n");
        display_leds();
    } else {
        uart_printstr("\r\nBad combination username/password\r\n");
    }
    prompt();
}

// Vector 19 "USART, RX" (Interrupt Vectors table); name from avr/interrupt.h
ISR(USART_RX_vect) {
    // Read UDR0 once: this clears RXC0, otherwise the interrupt fires again on exit (Receive Complete Flag and Interrupt)
    char c = UDR0;
    volatile char *buf = step ? pass_buf : user_buf;

    if (IS_ENTER(c)) {
        // Enter validates the current field
        buf[idx] = '\0';
        if (step == 0) {
            uart_printstr("\r\n    password: ");
            step = 1;
            idx = 0;
        } else {
            check_credentials();
        }
    } else if (IS_BACKSPACE(c)) {
        // "\b \b" moves the cursor back, overwrites with a space, moves back again
        if (idx > 0) {
            idx--;
            uart_printstr("\b \b");
        }
    } else if (idx < BUF_SIZE - 1) {
        // Store the real character; echo it for the username, '*' for the password
        buf[idx++] = c;
        uart_tx(step ? '*' : c);
    }
    // else: buffer full, drop the character
}

int main(void) {
    // LEDs as outputs, off (DDRx = 1 output, PORTx = 0 low)
    DDRB |= LED_MASK;
    PORTB &= ~LED_MASK;

    uart_init();
    prompt();
    // I bit in SREG is 0 after reset; enable after the UART is ready
    sei();
    // Everything happens in the ISR
    while (1) {
        ;
    }
}
