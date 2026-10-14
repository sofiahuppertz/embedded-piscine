#include <avr/io.h>

// Timer1 ticks per second with prescaler 256: 16 MHz / 256 = 62500 (TCCR1B, Clock Select table)
# define TICKS_PER_SECOND (F_CPU / 256)
// TOP for a 0.5 s period: counter runs 0..TOP, so TOP+1 = 31250 ticks -> TOP = 31249
# define TIMER_TOP ((TICKS_PER_SECOND / 2) - 1)

void setup( void ) {
    // PB1 (OC1A, LED D2) as output; the timer toggles but DDRB must still drive the pin (I/O-Ports, DDRB)
    DDRB    = (1 << PB1);

    // COM1A0 = toggle OC1A on compare match (Compare Output Mode table, non-PWM)
    TCCR1A  = (1 << COM1A0);

    // 16-bit write, high byte first by the compiler
    OCR1A   = TIMER_TOP;

    // WGM12 = CTC mode 4, TOP = OCR1A (Waveform Generation Mode table); CS12 = clk/256 and starts the timer
    TCCR1B  = ((1 << WGM12) | (1 << CS12));
}

int main ( void ) {

    setup();
    // Timer1 drives the LED in hardware; nothing left for the CPU to do
    while(1) {
        ;
    }

}
