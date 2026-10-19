#include "utils.h"

void    adc_init(void) {

    // REFS0 = AVcc reference (Table 24-3); ADLAR = 0 -> right adjusted, full 10 bits across ADCL/ADCH (24.9.3.1); MUX3:0 set per read
    ADMUX   = (1 << REFS0);

    // ADEN = enable; ADPS2:0 = 111 -> 16 MHz / 128 = 125 kHz, only factor inside the 50-200 kHz window (24.4, Table 24-5)
    ADCSRA  = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Disable the digital input buffers on PC0..PC2: analog pins at mid-level would make them toggle and draw current (24.9.5)
    DIDR0   = (1 << ADC2D) | (1 << ADC1D) |(1 << ADC0D);
}

uint16_t read_adc(uint8_t channel) {

    uint16_t value;

    // Keep REFS1:0 (high nibble), replace MUX3:0 with the channel (24.9.1, Table 24-4); safe while ADSC = 0 (24.5)
    ADMUX   &= 0xF0;
    ADMUX   |= channel;

    // ADSC = start one conversion; hardware clears it when done, 13 ADC clocks (24.3, Table 24-1)
    ADCSRA  |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC));

    // ADCL first, then ADCH: reading ADCL locks both until ADCH is read, so both bytes come from the same conversion (24.2, 24.9.3)
    // Result = Vin * 1024 / Vref, 0x3FF = Vref minus one LSB (24.7)
    value   =   ADCL;
    value   |=  (uint16_t)ADCH << 8;

    return value;
}
