#include "utils.h"
#include <util/delay.h>

int main(void){

    uart_init();
    i2c_init();

    // Sensor needs 40 ms after power-on before it accepts commands (AHT20 5.4 step 1)
    _delay_ms(40);

    while (1) {
        t_aht20 m = read_aht20_measurements();

        if (!m.ok)
            uart_printstr("i2c error\r\n");
        // TODO: keep the last 3 raw values, average, convert (AHT20 6.1 / 6.2), dtostrf, print

        // Sampling interval recommended by the manufacturer (AHT20 p2 footnote 5)
        _delay_ms(2000);
    }
}
