#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCE_TIME 10 // ms

static uint8_t pressed(void) {
    return !(PIND & (1 << PD2));
}

void setup(void) {
    DDRD  &= ~(1 << PD2);   // PD2 input
    PORTD |=  (1 << PD2);   // enable pull-up
    DDRB  |=  (1 << PB0);   // PB0 output
}

int main(void) {

    setup();
    while(1) {
        if (!pressed())
            continue;
        _delay_ms(DEBOUNCE_TIME);
        if (!pressed())
            continue;

        PORTB   ^= (1 << PB0);      // toggle state of LED

        while (pressed())           // wait for release
            ;
        _delay_ms(DEBOUNCE_TIME);   // release bounces too
    }
}
