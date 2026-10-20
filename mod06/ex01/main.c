#include "utils.h"
#include <util/delay.h>

int main(void){

    uart_init();
    i2c_init();

    // Sensor needs 40 ms after power-on before it accepts commands (AHT20 5.4 step 1)
    _delay_ms(40);

    while (1) {
        // Trigger: SLA+W, 0xAC 0x33 0x00, STOP (AHT20 5.4 step 2). Each call returns 0 and stops the bus on a bad status
        if (i2c_start(TW_WRITE)
            && i2c_write(AHT20_TRIGGER)
            && i2c_write(AHT20_PARAM1)
            && i2c_write(AHT20_PARAM2)) {
            i2c_stop();

            // Conversion takes more than 75 ms (AHT20 5.4 step 3)
            _delay_ms(75);

            // Read back: SLA+R, 7 bytes, STOP
            if (i2c_start(TW_READ)) {
                i2c_read();
                i2c_stop();
            }
        } else {
            uart_printstr("i2c error\r\n");
        }

        // Sampling interval recommended by the manufacturer (AHT20 p2 footnote 5)
        _delay_ms(2000);
    }
}
