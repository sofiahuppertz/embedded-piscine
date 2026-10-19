#include    "utils.h"
#include    <util/delay.h>

int main(void) {

    static const    uint8_t channels[] = { ADC_POT, ADC_LDR, ADC_NTC };

    uart_init();
    adc_init();

    // Three single conversions per iteration, one per channel (24.3); the 20 ms period comes from the delay, not from the ADC
    while (1){
        for (uint8_t i = 0; i < 3; i++) {
            uint8_t v;
            v = read_adc(channels[i]);
            uart_print_hex(v);
            if (i < 2)
                uart_printstr(", ");
        }
        uart_printstr("\r\n");
        _delay_ms(20);
    }
}
