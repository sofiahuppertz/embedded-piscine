// Build constants; defined here so util/delay.h and the macros below can use them
#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include <avr/io.h>

// Timer1 ticks per second with prescaler 256: 16 MHz / 256 = 62500 (TCCR1B, Clock Select table)
# define TICKS_PER_SECOND (F_CPU / 256)
// TOP for a 1 s period: counter runs 0..TOP, so TOP+1 = 62500 ticks -> TOP = 62499 (Fast PWM Mode)
# define TIMER_TOP (TICKS_PER_SECOND - 1)
// 10 % duty: pin is high for OCR1A+1 ticks, so OCR1A+1 = 62500 / 10 -> OCR1A = 6249 (Fast PWM Mode)
# define TIMER_COMP ((TICKS_PER_SECOND / 10) - 1)

void setup( void ) {
    // PB1 (OC1A, LED D2) as output; the timer drives the latch but DDRB must still enable the pin (I/O-Ports, DDRB)
    DDRB    = (1 << PB1);

    // COM1A1=1, COM1A0=0: non-inverting, set OC1A at BOTTOM, clear on match (Compare Output Mode, Fast PWM table); WGM11 = low bits of mode 14
    TCCR1A  = (1 << COM1A1) | (1 << WGM11);

    // ICR1 holds TOP in mode 14, written before the timer starts since it is not double-buffered (ICR1 description)
    ICR1    = TIMER_TOP;

    // Compare value for the 10 % point;
    OCR1A   = TIMER_COMP;

    // WGM13..10 = 1110: Fast PWM, TOP = ICR1, mode 14 (Waveform Generation Mode table); CS12 = clk/256 and starts the timer
    TCCR1B  = (1 << WGM13) | (1 << WGM12) | (1 << CS12);
}

int main ( void ) {

    setup();
    // Timer1 drives the LED in hardware; nothing left for the CPU to do
    while(1) {
        ;
    }

}
