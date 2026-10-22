#include "utils.h"

void    uart_init( void) {
    // 12-bit baud value split in two registers, high byte first (20.11.5 UBRRnL and UBRRnH)
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;

    // U2X0 = double speed: 2.1 % baud error instead of -3.5 % at 16 MHz (Table 20-7)
    UCSR0A = (1 << U2X0);

    // TXEN0 only: this module never reads from the console (20.11.3 UCSRnB)
    UCSR0B = (1 << TXEN0);

    // 8N1: UCSZ01:00 = 11 -> 8 bits; UMSEL, UPM, USBS = 0 -> async, no parity, 1 stop bit (20.11.4, Tables 20-8 to 20-11)
    UCSR0C  = (1 << UCSZ01) | (1 << UCSZ00);
}

void    uart_tx(char c) {
    // Wait for UDRE0 = transmit buffer empty, then write the byte (20.6.1 Sending Frames with 5 to 8 Data Bit)
    while ( ! ( UCSR0A & (1 << UDRE0 )));

    UDR0 = c;
}

void    uart_printstr(const char *str) {
    while (*str)
        uart_tx(*str++);
}
