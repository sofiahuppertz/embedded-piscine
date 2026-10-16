// Build constants; defined here so util/delay.h and the macros below can use them
#ifndef F_CPU
# define F_CPU 16000000UL
#endif
#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

// Datasheet 20.3.1, Table 20-1: UBRR formula, "Asynchronous Double Speed" row (divisor 8).
// Check result against Table 20-7 (fosc = 16MHz, 115.2k, U2Xn = 1 column).
#define UBRR_VALUE ((F_CPU / 8 / UART_BAUDRATE) - 1)

// Timer1 ticks per second with prescaler 256: 16 MHz / 256 = 62500 (16.11.2, Table 16-5 Clock Select).
#define TICKS_PER_SECOND (F_CPU / 256)
// TOP for a 1 s period: counter runs 0..TOP, so TOP+1 = 62500 ticks -> TOP = 62499 (16.11.5).
#define TIMER_TOP ((TICKS_PER_SECOND) - 1)


void    uart_init( void) {
    // 20.11.5: UBRR0 is 12 bits split over UBRR0H (upper 4 bits, rest must be 0) and UBRR0L.
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;

    // 20.3.2 + 20.11.2: U2X0 halves the divisor, gives 2.1% error at 16MHz instead of -3.5% (Table 20-7).
    UCSR0A = (1 << U2X0);

    // 20.11.3: TXEN0 enables the transmitter. UCSZ02 stays 0 for 8 data bits (Table 20-11).
    UCSR0B = (1 << TXEN0);

    // 20.11.4: UCSZ01:00 = 11 -> 8 data bits (Table 20-11).
    // Writing 0 to UMSEL (Table 20-8) = async, UPM (Table 20-9) = no parity, USBS (Table 20-10) = 1 stop bit.
    UCSR0C  = (1 << UCSZ01) | (1 << UCSZ00);
}


void    uart_tx(char c) {

    // 20.6.1: poll UDRE0 (buffer empty) before writing UDR0. 20.6.3: UDRE0 not TXC0, we only need buffer space.
    while ( ! ( UCSR0A & (1 << UDRE0 )));

    UDR0 = c;

}

void    timer1_init( void ) {

    // 16.3: 16-bit register, compiler writes high byte first via the TEMP register.
    OCR1A   = TIMER_TOP;

    // 16.11.8: OCIE1A unmasks the Compare Match A interrupt (flag OCF1A, auto-cleared on ISR entry, 16.11.9).
    TIMSK1 = (1 << OCIE1A);

    // WGM12 = CTC mode 4, TOP = OCR1A (Waveform Generation Mode table); CS12 = clk/256 and starts the timer
    TCCR1B  = ((1 << WGM12) | (1 << CS12));

}

// Table 12-6 vector 12 "TIMER1 COMPA". ISR macro and TIMER1_COMPA_vect name come from avr/interrupt.h (avr-libc).
ISR     (TIMER1_COMPA_vect) {
    uart_tx ('Z');
}

int main(void) {
    uart_init();
    timer1_init();
    // Enable interrupts.
    sei();
    while (1) {
        ;
    }
}