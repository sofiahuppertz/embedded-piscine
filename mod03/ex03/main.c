// utils.h first: it defines F_CPU, which util/delay.h needs
#include "utils.h"
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile char    user_buf[BUF_SIZE];
static volatile uint8_t idx  = 0;

// Alarm: all four LEDs flash together three times, then off
static void error_leds(void) {
    for (uint8_t i = 0; i < 3; i++) {
        PORTB |= LED_MASK;
        _delay_ms(100);
        PORTB &= ~LED_MASK;
        _delay_ms(100);
    }
}

// One hex character -> its 0..15 value, or 0xFF if it is not a hex digit
static uint8_t hex_val(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0xFF;
}

// Parse user_buf as "#RRGGBB". Returns 1 and fills r/g/b on success,
// 0 without touching the outputs if the line is malformed.
static uint8_t parse_hex(uint8_t *r, uint8_t *g, uint8_t *b) {
    uint8_t nib[6];

    // Exactly '#' + 6 hex digits
    if (idx != 7 || user_buf[0] != '#')
        return 0;
    for (uint8_t i = 0; i < 6; i++) {
        nib[i] = hex_val(user_buf[i + 1]);
        if (nib[i] == 0xFF)
            return 0;
    }
    // Each colour is two nibbles: high << 4 | low
    *r = (nib[0] << 4) | nib[1];
    *g = (nib[2] << 4) | nib[3];
    *b = (nib[4] << 4) | nib[5];
    return 1;
}

// Vector 19 "USART, RX" (Interrupt Vectors table); name from avr/interrupt.h
ISR(USART_RX_vect) {
    // Read UDR0 once: this clears RXC0, otherwise the interrupt fires again on exit (Receive Complete Flag and Interrupt)
    char c = UDR0;
    volatile char *buf = user_buf;

    if (IS_ENTER(c)) {
        // Enter validates the line
        buf[idx] = '\0';
        uint8_t r, g, b;
        if (parse_hex(&r, &g, &b)) {
            // Gamma-correct so the colour looks like the one requested
            set_rgb(gamma8(r), gamma8(g), gamma8(b));
            uart_printstr("\r\n");
        } else {
            uart_printstr("\r\nInvalid HEX color, expected #RRGGBB\r\n");
            error_leds();
        }
        // Start a fresh line either way
        idx = 0;
    } else if (IS_BACKSPACE(c)) {
        // "\b \b" moves the cursor back, overwrites with a space, moves back again
        if (idx > 0) {
            idx--;
            uart_printstr("\b \b");
        }
    } else if (idx < BUF_SIZE - 1) {
        // Store the character and echo it so the user sees what they type
        buf[idx++] = c;
        uart_tx(c);
    }
    // else: buffer full, drop the character
}

int main(void) {
    // LEDs as outputs, off (DDRx = 1 output, PORTx = 0 low)
    DDRB |= LED_MASK;
    PORTB &= ~LED_MASK;

    uart_init();
    init_rgb();
    // I bit in SREG is 0 after reset; enable after the UART is ready
    sei();
    // Everything happens in the ISR
    while (1) {
        ;
    }
}
