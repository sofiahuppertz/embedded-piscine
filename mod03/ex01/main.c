#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

// RGB LED D5: red on PD5, green on PD6, blue on PD3 (devkit schematic, nets LED_R / LED_G / LED_B)
#define RED         (1 << PD5)
#define GREEN       (1 << PD6)
#define BLUE        (1 << PD3)
#define LED_MASK    (RED | GREEN | BLUE)

// 3-bit colours: each one is the OR of the primaries that are on (table in the subject)
#define YELLOW      (RED | GREEN)
#define CYAN        (GREEN | BLUE)
#define MAGENTA     (RED | BLUE)
#define WHITE       (RED | GREEN | BLUE)

int main(void) {

    static const uint8_t colors[7] = { RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE };

    // LED pins as outputs, all off (I/O-Ports: DDRx = 1 output, PORTx = 0 low)
    DDRD |= LED_MASK;
    PORTD &= ~LED_MASK;

    while (1) {
        for (uint8_t i = 0; i < 7; i++) {
            // Clear the three LED bits, then set the ones for this colour; other PORTD bits untouched
            PORTD = (PORTD & ~LED_MASK) | colors[i];
            _delay_ms(1000);
        }
    }
}
