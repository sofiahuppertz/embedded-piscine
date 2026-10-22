#include    <avr/io.h>
#include    <avr/interrupt.h>

// Timer1 TOP (ICR1): duty resolution, OCR1A = TOP is 100 % (Fast PWM Mode).
// One Timer0 tick = one step, 500 up + 500 down = 1 s. PWM = 16 MHz / 8 / 501 ~ 4 kHz.
# define PWM_TOP        500

// Timer0 TOP (OCR0A): clk/64 = 250 000 ticks/s, 250 ticks = 1 ms, so OCR0A = 249.
# define TICK_TOP       249

static uint16_t duty = 0;   // current OCR1A, 0..PWM_TOP
static int8_t   dir  = 1;   // +1 up, -1 down


void setup_timer_1( void ) {
    // Non-inverting on OC1A (Compare Output Mode, Fast PWM table); WGM11 = low bit of mode 14
    TCCR1A  = (1 << COM1A1) | (1 << WGM11);

    // TOP in mode 14, set before start since ICR1 is not double-buffered
    ICR1    = PWM_TOP;

    // Start at 0 %
    OCR1A   = 0;

    // Mode 14: Fast PWM, TOP = ICR1 (Waveform Generation Mode table); CS11 = clk/8, starts the timer
    TCCR1B  = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
}

void setup_timer_0( void ) {
    // CTC mode 2, TOP = OCR0A (Table 15-8). COM0x = 0: no pin output, interrupt only
    TCCR0A  = (1 << WGM01);

    // 1 ms period
    OCR0A   = TICK_TOP;

    // Enable Compare Match A interrupt (15.9.6 TIMSK0)
    TIMSK0  = (1 << OCIE0A);

    // clk/64 (Table 15-9), starts the timer
    TCCR0B  = (1 << CS01) | (1 << CS00);
}

// TIMER0_COMPA_vect = _VECTOR(14) = __vector_14 (iom328p.h), vector 14 in Table 12-6.
// signal: save registers, return with reti. OCF0A is cleared by hardware on entry.
__attribute__((signal, used, externally_visible))
void __vector_14(void) {
    duty    += dir;
    OCR1A   = duty;     // double-buffered, applied at next TOP (Fast PWM Mode)

    // Triangle: turn around at both ends
    if ( duty == PWM_TOP || duty == 0 ) {
        dir *= -1;
    }
}

int main(void) {

    // PB1 (OC1A, LED D2) as output
    DDRB    |= (1 << PB1);

    setup_timer_1();
    setup_timer_0();

    SREG   |= (1 << SREG_I);  // Global Interrupt Enable, SREG bit 7 (7.3.1)
    while (1) {
        ;
    }
}
