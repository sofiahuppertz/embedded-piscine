#include    "utils.h"
#include    <util/delay.h>

int main(void) {

    uart_init();
    adc_init();

    // First conversion after switching to the 1.1 V reference may be wrong while the bandgap settles: discard it (24.5.2, 24.4)
    read_adc();

    // One single conversion per iteration (24.3); the 20 ms period comes from the delay, not from the ADC
    while (1){
        uint16_t    v;
        int16_t     c;

        v = read_adc();

        c = adc_to_celsius(v);
        // uart_print_dec is unsigned: send the sign ourselves
        if (c < 0) {
            uart_tx('-');
            c = -c;
        }
        uart_print_dec((uint16_t)c);
        uart_printstr("\r\n");
        _delay_ms(20);
    }
}
