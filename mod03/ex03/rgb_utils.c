#include "utils.h"

void    init_rgb(void) {
    // DDR bit must be 1 for the compare output to reach the pin (15.6 / 18.6 Compare Match Output Unit)
    DDRD    |= RGB_MASK;

    // Timer0: red (OC0B) and green (OC0A)
    // COMnx1 = 1: non-inverting, set at BOTTOM, clear on match (Tables 15-3, 15-6)
    // WGM01:00 = 11 -> Fast PWM mode 3, TOP = 0xFF so OCR0x is the 0..255 duty (Table 15-8, 15.7.3)
    TCCR0A  = ((1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00));
    GREEN_PWM   = 0;
    RED_PWM     = 0;
    // CS01:00 = 11 -> clk/64, 16 MHz / 64 / 256 = 976 Hz, starts the timer (Table 15-9)
    TCCR0B  = ((1 << CS01) | (1 << CS00));

    // Timer2: blue (OC2B), same mode 3 non-inverting (Tables 18-6, 18-8, 18.7.3)
    TCCR2A  = ((1 << COM2B1) | (1 << WGM21) | (1 << WGM20));
    BLUE_PWM    = 0;
    // CS22 = 1 -> clk/64. Timer2 has its own table, bit pattern differs from Timer0 (Table 18-9)
    TCCR2B  = (1 << CS22);
}

// 8-bit writes, no atomicity concern. OCRnx is double-buffered in PWM modes and applied at TOP, so no glitch (15.7.3 / 18.7.3)
void    set_rgb(uint8_t r, uint8_t g, uint8_t b){
    RED_PWM     = r;
    GREEN_PWM   = g;
    BLUE_PWM    = b;
}

// Software filter: the eye sees brightness roughly as duty^(1/2.2), so a linear
// duty looks crushed toward bright. Squaring the value (gamma 2.0) compensates:
// 255 -> 255, 128 -> 64, 64 -> 16, 0 -> 0. Rounded, so 255 maps exactly to 255.
// A formula instead of a 256-entry table: PROGMEM needs avr/pgmspace.h, which is
// not in the authorized headers, and 256 bytes of RAM for a table is wasteful.
uint8_t gamma8(uint8_t v) {
    return ((uint16_t)v * v + 127) / 255;
}
