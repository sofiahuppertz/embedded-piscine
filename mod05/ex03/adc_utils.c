#include "utils.h"

void    adc_init(void) {

    // REFS1:0 = 11 -> internal 1.1 V reference, required for the temperature sensor (24.8, Table 24-3)
    // ADLAR = 0 -> right adjusted, full 10 bits so 1 LSB ~ 1 mV ~ 1 C (24.9.3.1); MUX3:0 = ADC_TEMP = 8 -> temperature sensor (Table 24-4)
    ADMUX   = (1 << REFS1) | (1 << REFS0) | ADC_TEMP;

    // ADEN = enable; ADPS2:0 = 111 -> 16 MHz / 128 = 125 kHz, only factor inside the 50-200 kHz window (24.4, Table 24-5)
    ADCSRA  = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t read_adc(void) {

    uint16_t value;

    // ADSC = start one conversion; hardware clears it when done, 13 ADC clocks (24.3, Table 24-1)
    ADCSRA  |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC));

    // ADCL first, then ADCH: reading ADCL locks both until ADCH is read, so both bytes come from the same conversion (24.2, 24.9.3)
    // Result = Vin * 1024 / Vref, 0x3FF = Vref minus one LSB (24.7)
    value   =   ADCL;
    value   |=  (uint16_t)ADCH << 8;

    return value;
}

// Datasheet software calibration (24.8): T = (code - TOS) / k
// k = sensor slope in ADC codes per C. Table 24-2: (380 - 242) mV / (85 - (-45)) C = 1.06 mV/C; one code = 1100 / 1024 = 1.07 mV
//     with the 1.1 V reference (24.7), so k = 1.06 / 1.07 = 0.99 ~ 1 code per C: the division disappears
// TOS = code the sensor gives at 0 C on THIS chip. Typical case would be 314 mV - 25 C = 289 mV = 269 codes, this chip reads
//     ~380 codes at ~22 C room -> TOS = 358. Measured once by hand, the datasheet expects it stored in EEPROM by a production test.
// Accuracy is still +/-10 C per 24.8 plus the 1.0..1.2 V reference tolerance (Table 29-15)
#define TEMP_TOS    358

int16_t adc_to_celsius(uint16_t code) {
    return (int16_t)code - TEMP_TOS;
}
