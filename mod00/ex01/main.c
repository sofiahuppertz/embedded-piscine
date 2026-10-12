#include <avr/io.h>


int main(void)
{

    // Data Direction Register: 1 = Output, 0 = Input, per pin
    // B stands for bank B
    DDRB = 1;

    // Output value of bank B.
    while (1) {
        PORTB = 1;
    }

    return (0);

};
