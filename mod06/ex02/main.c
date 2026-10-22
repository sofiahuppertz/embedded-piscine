#include "utils.h"
#include <util/delay.h>
#include <stdlib.h>

#define SAMPLE_COUNT   3

// Mean of the last n raw readings; n < SAMPLE_COUNT for the first readings so they stay consistent with the rule
static uint32_t average(const uint32_t *buf, uint8_t n) {
    uint32_t    sum = 0;

    for (uint8_t i = 0; i < n; i++)
        sum += buf[i];
    return sum / n;
}

static void print_float(float v, uint8_t decimals) {
    char    buf[10];

    dtostrf(v, 1, decimals, buf);
    uart_printstr(buf);
}

int main(void){
    uint32_t    hum[SAMPLE_COUNT];
    uint32_t    temp[SAMPLE_COUNT];
    uint8_t     idx = 0;        // next slot to overwrite
    uint8_t     count = 0;      // valid slots, saturates at SAMPLE_COUNT

    uart_init();
    i2c_init();

    // Sensor needs 40 ms after power-on before it accepts commands (AHT20 5.4 step 1)
    _delay_ms(40);

    while (1) {
        t_aht20 m = read_aht20_measurements();

        if (m.ok) {
            // Ring buffer of raw words; average in integer, convert once (AHT20 6.1 / 6.2)
            hum[idx]  = m.humidity;
            temp[idx] = m.temperature;
            idx = (idx + 1) % SAMPLE_COUNT;
            if (count < SAMPLE_COUNT)
                count++;

            // Resolution 0.01 C and 0.024 %RH (AHT20 Tables 3 and 1) -> 2 and 1 decimals
            uart_printstr("Temperature: ");
            print_float(aht20_temperature(average(temp, count)), 2);
            uart_printstr("C, Humidity: ");
            print_float(aht20_humidity(average(hum, count)), 1);
            uart_printstr("%\r\n");
        } else {
            uart_printstr("i2c error\r\n");
        }

        // Manufacturer recommendation: measure every 2 s so self-heating stays below 0.1 C (AHT20 p5, 2.4 Temperature Influence)
        _delay_ms(2000);
    }
}
