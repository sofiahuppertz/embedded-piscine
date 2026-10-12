#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCE_TIME 10 // 10 ms
#define COUNTER_MASK 0x0F

void setup(void) {
    DDRD    &= ~((1 << PD2) | (1 << PD4)); // Cleanup DDRD
    PORTD   |= ((1 << PD2) | (1 << PD4)); // Enable Pull-Up
    DDRB    |= ((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // Set output leds
}

void display(uint8_t num) {
    PORTB   &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4));
    PORTB   |= ((num >> 0) & 1 )  << PB0
            | ((num >> 1) & 1 )  << PB1
            | ((num >> 2) & 1 )  << PB2
            | ((num >> 3) & 1 )  << PB4;
}

static uint8_t read_pin(uint8_t pin) {
    return !(PIND & (1 << pin));
}

static uint8_t debounced(uint8_t pin, uint8_t prev) {
    uint8_t a = read_pin(pin);
    _delay_ms(DEBOUNCE_TIME);
    uint8_t b = read_pin(pin);
    return (a == b) ? a : prev;
}

int main(void) {

    uint8_t value = 0;
    uint8_t sw1_prev = 0;
    uint8_t sw2_prev = 0;

    setup();
    display(value);

    while (1) {
        uint8_t sw1 = debounced(PD2, sw1_prev);
        uint8_t sw2 = debounced(PD4, sw2_prev);

        if (sw1 && !sw1_prev && value < 15) // sw1 pressed
            value = (value + 1) & COUNTER_MASK;
        if (sw2 && !sw2_prev && value > 0) // sw2 pressed
            value = (value - 1 ) & COUNTER_MASK;

        sw1_prev = sw1;
        sw2_prev = sw2;

        display(value);
    }

}
