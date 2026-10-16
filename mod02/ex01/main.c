// Build constants; defined here so util/delay.h and the macros below can use them
#ifndef F_CPU
# define F_CPU 16000000UL
#endif
#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

// UBRR for double speed mode: F_CPU / (8 * baud) - 1 = 16 at 16 MHz, 115200 (Table 20-1, checked in Table 20-7)
#define UBRR_VALUE ((F_CPU / 8 / UART_BAUDRATE) - 1)

// Timer1 ticks per second with prescaler 1024: 16 MHz / 1024 = 15625 (TCCR1B, Clock Select table)
#define TICKS_PER_SECOND (F_CPU / 1024)
// TOP for a 2 s period: counter runs 0..TOP, so TOP+1 = 31250 ticks -> TOP = 31249, fits 16 bits (OCR1A)
#define TIMER_TOP ((TICKS_PER_SECOND * 2) - 1)


void    uart_init( void) {
    // 12-bit baud value split in two registers, high byte first (UBRRnL and UBRRnH)
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;

    // U2X0 = double speed: 2.1 % baud error instead of -3.5 % at 16 MHz (Table 20-7)
    UCSR0A = (1 << U2X0);

    // TXEN0 = enable transmitter; UCSZ02 stays 0 for 8 data bits (UCSRnB)
    UCSR0B = (1 << TXEN0);

    // 8N1: UCSZ01:00 = 11 -> 8 bits; UMSEL, UPM, USBS = 0 -> async, no parity, 1 stop bit (UCSRnC tables)
    UCSR0C  = (1 << UCSZ01) | (1 << UCSZ00);
}


void    uart_tx(char c) {

    // Wait for UDRE0 = transmit buffer empty, then write the byte (Sending Frames with 5 to 8 Data Bit)
    while ( ! ( UCSR0A & (1 << UDRE0 )));

    UDR0 = c;

}

// One uart_tx per byte; uart_tx already waits for buffer space, so no delay needed
void    uart_printstr(const char *str){
    while (*str) {
        uart_tx(*str);
        str++;
    }

}

void    timer1_init( void ) {

    // 16-bit write, high byte first by the compiler (Accessing 16-bit Registers)
    OCR1A   = TIMER_TOP;

    // OCIE1A = interrupt on compare match A; its flag clears itself when the ISR runs (TIMSK1, TIFR1)
    TIMSK1 = (1 << OCIE1A);

    // WGM12 = CTC mode 4, TOP = OCR1A (Waveform Generation Mode table); CS bits = clk/1024 and start the timer (Clock Select table)
    TCCR1B  = ((1 << WGM12) | (1 << CS12));

}

// Vector 12 "TIMER1 COMPA" (Interrupt Vectors table); ISR macro and vector name from avr/interrupt.h
ISR     (TIMER1_COMPA_vect) {
    uart_printstr("Hello World!\n");
}

int main(void) {
    uart_init();
    timer1_init();
    // I bit in SREG is 0 after reset; enable last so nothing fires before UART and timer are ready
    sei();
    // Everything happens in the ISR
    while (1) {
        ;
    }
}