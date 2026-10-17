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

void    uart_init( void) {
    // 12-bit baud value split in two registers, high byte first (UBRRnL and UBRRnH)
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;

    // U2X0 = double speed: 2.1 % baud error instead of -3.5 % at 16 MHz (Table 20-7)
    UCSR0A = (1 << U2X0);

    // RXEN0 + TXEN0 = enable receiver and transmitter; RXCIE0 = interrupt when a byte arrives (Receive Complete Flag and Interrupt)
    UCSR0B = (1<< RXCIE0) | (1<<RXEN0) | (1 << TXEN0);

    // 8N1: UCSZ01:00 = 11 -> 8 bits; UMSEL, UPM, USBS = 0 -> async, no parity, 1 stop bit (UCSRnC tables)
    UCSR0C  = (1 << UCSZ01) | (1 << UCSZ00);
}


void    uart_tx(char c) {

    // Wait for UDRE0 = transmit buffer empty, then write the byte (Sending Frames with 5 to 8 Data Bit)
    while ( ! ( UCSR0A & (1 << UDRE0 )));

    UDR0 = c;
}

// Vector 19 "USART, RX" (Interrupt Vectors table); name from avr/interrupt.h
ISR(USART_RX_vect) {
    // Read UDR0 once: this clears RXC0, otherwise the interrupt fires again on exit (Receive Complete Flag and Interrupt)
    char c = UDR0;
    // ASCII: upper and lower case differ only in bit 5 ('A' 0x41, 'a' 0x61).
    // OR with bit 5 gives the lowercase form for the range test; the test is 1 for a letter, 0 otherwise;
    // shifted to bit 5 it becomes the XOR mask that flips the case of letters and leaves everything else alone.
    uart_tx((c ^ (((c | (1 << 5)) >= 'a' && (c | (1 << 5)) <= 'z' )  << 5)));
}

int main(void) {
    uart_init();
    // I bit in SREG is 0 after reset; enable after the UART is ready
    sei();
    // Everything happens in the ISR
    while (1) {
        ;
    }
}