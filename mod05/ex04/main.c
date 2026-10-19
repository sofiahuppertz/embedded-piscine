#include    "utils.h"
#include    <util/delay.h>

// Gauge LEDs D1..D4 on PB0, PB1, PB2, PB4; PB3 is skipped (devkit schematic)
#define GAUGE_MASK  ((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4))

void    led_init(void) {
    // DDxn = 1 -> output (14.2.1, Table 14-1); |= keeps the other PORTB pins as they are
    DDRB    |= GAUGE_MASK;
    PORTB   &= ~GAUGE_MASK;
}

// 8-bit ADC value is already the pot position on a 0..255 scale (24.7): 25 % = 64, 50 % = 128, 75 % = 192, 100 % = 255
void    display_gauge(uint8_t pos) {
    uint8_t mask = 0;

    if (pos >= 64)
        mask |= (1 << PB0);
    if (pos >= 128)
        mask |= (1 << PB1);
    if (pos >= 192)
        mask |= (1 << PB2);
    if (pos >= 255)
        mask |= (1 << PB4);

    // One write so the four LEDs change together, other PORTB bits untouched
    PORTB   = (PORTB & ~GAUGE_MASK) | mask;
}

int main(void) {

    rgb_init();
    led_init();
    adc_init();

    // First conversion after ADEN takes 25 ADC clocks and is less accurate: discard it (24.4)
    read_adc();

    // One single conversion per iteration (24.3); the 20 ms period comes from the delay, not from the ADC
    while (1){
        uint8_t    v;

        v = read_adc();
        wheel(v);
        display_gauge(v);
        _delay_ms(20);
    }
}
