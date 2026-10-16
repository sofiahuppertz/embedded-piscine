// Build constants; defined here so util/delay.h and the macros below can use them
#ifndef F_CPU
# define F_CPU 16000000UL
#endif
#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

#include <avr/io.h>

// UBRR for double speed mode: F_CPU / (8 * baud) - 1 = 16 at 16 MHz, 115200 (Table 20-1, checked in Table 20-7)
#define UBRR_VALUE ((F_CPU / 8 / UART_BAUDRATE) - 1)

void    uart_init( void) {
    // 12-bit baud value split in two registers, high byte first (UBRRnL and UBRRnH)
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;

    // U2X0 = double speed: 2.1 % baud error instead of -3.5 % at 16 MHz (Table 20-7)
    UCSR0A = (1 << U2X0);

    // RXEN0 + TXEN0 = enable receiver and transmitter; UCSZ02 stays 0 for 8 data bits (UCSRnB)
    UCSR0B = (1<<RXEN0) | (1 << TXEN0);

    // 8N1: UCSZ01:00 = 11 -> 8 bits; UMSEL, UPM, USBS = 0 -> async, no parity, 1 stop bit (UCSRnC tables)
    UCSR0C  = (1 << UCSZ01) | (1 << UCSZ00);
}


void    uart_tx(char c) {

    // Wait for UDRE0 = transmit buffer empty, then write the byte (Sending Frames with 5 to 8 Data Bit)
    while ( ! ( UCSR0A & (1 << UDRE0 )));

    UDR0 = c;

}


char uart_rx( void )
{
    // Wait for RXC0 = a byte is in the receive buffer (Receiving Frames with 5 to 8 Data Bits)
    while ( !(UCSR0A & (1<<RXC0)) )
        ;
    // Reading UDR0 returns the byte and clears RXC0 (Receive Complete Flag and Interrupt)
    return UDR0;
}


int main(void) {
    uart_init();
    // Echo: uart_rx blocks until a key arrives, so the loop runs once per character, no delay needed
    while (1) {
        uart_tx(uart_rx());
    }
}