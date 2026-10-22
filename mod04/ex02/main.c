#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <util/delay.h>

#define DEBOUNCE_TIME 50 // ms

volatile uint8_t value = 0;

void setup(void) {
    DDRD    &= ~((1 << PD2) | (1 << PD4)); // Cleanup DDRD
    PORTD   |= ((1 << PD2) | (1 << PD4)); // Enable Pull-Up
    DDRB    |= ((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // Set output leds

    // SW1 on PD2 = INT0 (Table 14-9): external interrupt
    EICRA   = (1 << ISC00);                 // any logical change
    EIMSK   = (1 << INT0);                  // enable INT0 (13.2.2)
    EIFR    = (1 << INTF0);                 // drop any pending request (13.2.3)

    // SW2 on PD4 = PCINT20 only (Table 14-9)
    PCMSK2  = (1 << PCINT20);               // unmask PD4 in group PCINT[23:16] (13.2.6)
    PCICR   = (1 << PCIE2);                 // enable group 2 -> PCINT2 vector (13.2.4)
    PCIFR   = (1 << PCIF2);                 // drop any pending request (13.2.5)

    SREG    |= (1 << SREG_I);               // Global Interrupt Enable, SREG bit 7 (7.3.1)

}

void display(uint8_t num) {
    PORTB   &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4));
    PORTB   |= ((num >> 0) & 1 )  << PB0
            | ((num >> 1) & 1 )  << PB1
            | ((num >> 2) & 1 )  << PB2
            | ((num >> 3) & 1 )  << PB4;
}

__attribute__((signal, used, externally_visible))
void __vector_1(void) {

    _delay_ms(DEBOUNCE_TIME);

    // Pull-up: low = pressed (14.2.1)
    uint8_t was_press = !(PIND & (1 << PD2));

    if (was_press && value < 15) {
        // increment value
        value++;
        // display LEDs
        display(value);
    }

    // Clear INTF0
    EIFR = (1 << INTF0);
}

// PCINT2_vect = _VECTOR(5) = __vector_5 (iom328p.h), vector 6 in Table 12-6.
// Pin change fires on both edges (13 intro), so this runs on press AND release.
__attribute__((signal, used, externally_visible))
void __vector_5(void) {
    // Debounce
    _delay_ms(DEBOUNCE_TIME);

    // Pull-up: low = pressed (14.2.1)
    uint8_t was_press = !(PIND & (1 << PD4));

    if (was_press && value > 0) {
        // decrement value
        value--;
        // display LEDs
        display(value);
    }

    // Clear PCIF2 last: drops the bounces seen during the delay.
    // 13.2.5: "cleared by writing a logical one to it".
    PCIFR = (1 << PCIF2);
}

int main(void) {

    setup();
    display(value);

    while (1) {
        ;
    }

}
