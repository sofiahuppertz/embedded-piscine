#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define LED_MASK    ((1 << PD3) | (1 << PD5) | (1 << PD6))

int main(void) {

    static const uint8_t leds[3] = { PD5, PD6, PD3 };

    DDRD |= LED_MASK;
    PORTD   &= ~LED_MASK;


    while (1) {
        for(uint8_t i = 0; i < 3; i++) {
            PORTD =  (PORTD & ~LED_MASK) | (1 << leds[i]);
            _delay_ms(1000);
        }
    }
}