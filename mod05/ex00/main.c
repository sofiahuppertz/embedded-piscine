#include    "utils.h"
#include    <util/delay.h>


int main(void) {

    uint8_t     v;

    uart_init();
    adc_init();

    // One single conversion per iteration (24.3);
    while (1){
        v = read_adc();
        uart_print_hex(v);
        uart_printstr("\r\n");
        _delay_ms(20);
    }
}
