// Build constants; defined here so util/delay.h and the macros below can use them
#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include    <avr/io.h>
#include    <util/delay.h>

// Timer1 ticks per second with prescaler 256: 16 MHz / 256 = 62500 (TCCR1B, Clock Select table)
# define TICKS_PER_SECOND (F_CPU / 256)
// TOP for a 1 s period: counter runs 0..TOP, so TOP+1 = 62500 ticks -> TOP = 62499 (Fast PWM Mode)
# define TIMER_TOP (TICKS_PER_SECOND - 1)
// Ticks per 10 % of the period. Pulse is OCR1A+1 ticks, so OCR1A = n * DUTY_CHUNK - 1, the -1 applied once in main
# define DUTY_CHUNK (TICKS_PER_SECOND / 10)
// Two reads this far apart agree only once the contact has stopped bouncing
# define DEBOUNCE_TIME 10

void setup_buttons( void ) {
    // SW1 on PD2, SW2 on PD4 (devkit schematic), DDRD bit = 0 makes them inputs (I/O-Ports, Configuring the Pin)
    DDRD    &=  ~((1 << PD2) | (1 << PD4));
    // PORTD bit = 1 on an input enables the internal pull-up; the buttons short the pin to GND when pressed
    PORTD   |=  ((1 << PD2) | (1 << PD4));
}

void setup_timer( void ) {
    // PB1 (OC1A, LED D2) as output; the timer drives the latch but DDRB must still enable the pin (I/O-Ports, DDRB)
    DDRB    |= (1 << PB1);

    // COM1A1=1, COM1A0=0: non-inverting, set OC1A at BOTTOM, clear on match (Compare Output Mode, Fast PWM table); WGM11 = low bit of mode 14
    TCCR1A  = (1 << COM1A1) | (1 << WGM11);

    // ICR1 holds TOP in mode 14, written before the timer starts since it is not double-buffered (ICR1 description)
    ICR1    = TIMER_TOP;

    // Start at 10 % so the first period is already correct (Fast PWM Mode)
    OCR1A   = DUTY_CHUNK - 1;

    // WGM13..10 = 1110: Fast PWM, TOP = ICR1, mode 14 (Waveform Generation Mode table); CS12 = clk/256 and starts the timer
    TCCR1B  = (1 << WGM13) | (1 << WGM12) | (1 << CS12);
}

// Pull-up means idle = 1, pressed = 0; invert so 1 means pressed (I/O-Ports, Reading the Pin Value)
static  uint8_t read_pin(uint8_t pin) {
    return !(PIND & (1 << pin));
}

// Only use of _delay_ms, for debouncing: accept a level only if it is stable across the delay, else keep the previous one
static uint8_t  debounced(uint8_t pin, uint8_t prev) {
    uint8_t a   =   read_pin(pin);
    _delay_ms(DEBOUNCE_TIME);
    uint8_t b   =   read_pin(pin);
    return (a == b) ? a : prev;
}

// 16-bit write, interrupts off so no SREG guard. In PWM modes OCR1A is double-buffered and applied at TOP, so a change shows on the next period (Accessing 16-bit Registers, Fast PWM Mode)
static  void    set_ocr(uint16_t value) {
    OCR1A   = value;
}

int main(void) {
    // Duty in tenths: 1 = 10 % .. 10 = 100 %. At 10, OCR1A = TOP, which the datasheet defines as constantly high (Fast PWM Mode)
    uint16_t value = 1;
    uint8_t sw1_prev = 0;
    uint8_t sw2_prev = 0;

    setup_buttons();
    setup_timer();

    while (1) {
        uint8_t sw1 = debounced(PD2, sw1_prev);
        uint8_t sw2 = debounced(PD4, sw2_prev);

        // Act on the rising edge only (pressed now, not before), so holding a button counts once
        if (sw1 && !sw1_prev && value < 10) {
            value++;
            set_ocr(DUTY_CHUNK * value - 1);
        }
        if (sw2 && !sw2_prev && value > 1) {
            value--;
            set_ocr(DUTY_CHUNK * value - 1);
        }
        sw1_prev = sw1;
        sw2_prev = sw2;


    }
}
