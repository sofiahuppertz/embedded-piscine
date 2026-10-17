#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

// RGB LED D5: red on PD5, green on PD6, blue on PD3 (devkit schematic, nets LED_R / LED_G / LED_B)
#define RED         (1 << PD5)
#define GREEN       (1 << PD6)
#define BLUE        (1 << PD3)
#define LED_MASK    (RED | GREEN | BLUE)

// Duty register of each colour. The LED pins are output-compare pins: PD5 = OC0B, PD6 = OC0A, PD3 = OC2B (Table 14-9, Port D alternate functions)
#define RED_PWM     OCR0B
#define GREEN_PWM   OCR0A
#define BLUE_PWM    OCR2B


void    init_rgb(void) {
    // DDR bit must be 1 for the compare output to reach the pin (15.6 / 18.6 Compare Match Output Unit)
    DDRD    |= LED_MASK;

    // Timer0: red (OC0B) and green (OC0A)
    // COMnx1 = 1: non-inverting, set at BOTTOM, clear on match (Tables 15-3, 15-6)
    // WGM01:00 = 11 -> Fast PWM mode 3, TOP = 0xFF so OCR0x is the 0..255 duty (Table 15-8, 15.7.3)
    TCCR0A  = ((1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00));
    GREEN_PWM   = 0;
    RED_PWM     = 0;
    // CS01:00 = 11 -> clk/64, 16 MHz / 64 / 256 = 976 Hz, starts the timer (Table 15-9)
    TCCR0B  = ((1 << CS01) | (1 << CS00));

    // Timer2: blue (OC2B), same mode 3 non-inverting (Tables 18-6, 18-8, 18.7.3)
    TCCR2A  = ((1 << COM2B1) | (1 << WGM21) | (1 << WGM20));
    BLUE_PWM    = 0;
    // CS22 = 1 -> clk/64. Timer2 has its own table, bit pattern differs from Timer0 (Table 18-9)
    TCCR2B  = (1 << CS22);
}

// 8-bit writes, no atomicity concern. OCRnx is double-buffered in PWM modes and applied at TOP, so no glitch (15.7.3 / 18.7.3)
void    set_rgb(uint8_t r, uint8_t g, uint8_t b){
    RED_PWM     = r;
    GREEN_PWM   = g;
    BLUE_PWM    = b;
}

// Provided by the subject: 0..255 position on the colour wheel, always a mix of two primaries at most
void wheel(uint8_t pos) {

    pos = 255 - pos;

    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    } else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}


int main(void){

    init_rgb();

    uint8_t i   = 0;

    // PWM runs in hardware, the CPU only paces the wheel. i wraps 255 -> 0 and restarts it
    while (1) {
        wheel(i);
        _delay_ms(80);
        i++;
    }

}