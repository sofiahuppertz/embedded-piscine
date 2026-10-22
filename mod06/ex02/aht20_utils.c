#include "utils.h"
#include <util/delay.h>

// One measurement cycle, AHT20 5.4 steps 2-3: trigger, wait, read 7 bytes, unpack the two 20-bit fields.
// ok = 0 if any bus step failed or the sensor still reports busy; the caller decides what to print.
t_aht20 read_aht20_measurements(void) {
    t_aht20 m = { 0, 0, 0 };
    uint8_t raw[AHT20_DATA_LEN];

    if (!(i2c_start(TW_WRITE)
        && i2c_write(AHT20_TRIGGER)
        && i2c_write(AHT20_PARAM1)
        && i2c_write(AHT20_PARAM2)))
        return m;
    i2c_stop();

    // Conversion takes more than 75 ms (AHT20 5.4 step 3)
    _delay_ms(80);

    if (!i2c_start(TW_READ))
        return m;
    if (!i2c_read(raw, AHT20_DATA_LEN)) {
        i2c_stop();
        return m;
    }
    i2c_stop();

    // Status bit 7 = busy, data not ready (AHT20 Table 10)
    if (raw[0] & AHT20_BUSY)
        return m;

    // Byte layout (AHT20 p8 read figure): [1] RH19..12, [2] RH11..4, [3] RH3..0 | T19..16, [4] T15..8, [5] T7..0, [6] CRC
    m.humidity    = ((uint32_t)raw[1] << 12) | ((uint32_t)raw[2] << 4) | (raw[3] >> 4);
    m.temperature = ((uint32_t)(raw[3] & 0x0F) << 16) | ((uint32_t)raw[4] << 8) | raw[5];
    m.ok = 1;
    return m;
}

// RH[%] = S_RH / 2^20 * 100 (AHT20 6.1)
float   aht20_humidity(uint32_t raw) {
    return (float)raw * 100.0f / AHT20_FULL_SCALE;
}

// T[C] = S_T / 2^20 * 200 - 50 (AHT20 6.2)
float   aht20_temperature(uint32_t raw) {
    return (float)raw * 200.0f / AHT20_FULL_SCALE - 50.0f;
}
