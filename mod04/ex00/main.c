#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#define DEBOUNCE_TIME 50 // ms

__attribute__((signal, used, externally_visible))
void __vector_1(void) {
    uint8_t was_press = !(EICRA & (1 << ISC00));   // ISC00 clear = falling edge

    if (was_press)
        PORTB ^= (1 << PB0);                    // toggle LED D1 on press only

    // Debounce: wait for the contact to settle before touching anything.
    _delay_ms(DEBOUNCE_TIME);

    // Swap the edge INT0 watches (Table 13-2): rising after a press so the
    // next interrupt is the release, falling after a release.
    if (was_press)
        EICRA = (1 << ISC01) | (1 << ISC00);
    else
        EICRA = (1 << ISC01);

    // Clear INTF0 last: drops the bounces seen during the delay and anything
    // the edge change may have set. Datasheet 13.2.3: "cleared by writing a
    // logical one to it".
    EIFR = (1 << INTF0);
}

int main(void) {

    // Init LED D1
    DDRB = (1 << PB0);

    // Init SW1
    DDRD    &=  ~(1 << PD2);
    PORTD   |=  (1 << PD2);

    // Setup Interrupt
    EICRA   = (1 << ISC01);                 // falling edge = press (Table 13-2)
    EIMSK   = (1 << INT0);                  // enable INT0 (13.2.2)
    EIFR    = (1 << INTF0);                 // drop any pending request (13.2.3)

    sei();                                  // SREG I-bit (7.3.1)

    while (1) {
        ;
    }
}


