#ifndef UTILS_H
# define UTILS_H

// Build constant; default so util/delay.h works even if the Makefile does not pass it with -D
# ifndef F_CPU
#  define F_CPU 16000000UL
# endif

# include <avr/io.h>

/* ----------------------------- ADC ------------------------------------ */

void    adc_init(void);
uint8_t read_adc(void);

/* ----------------------------- RGB LED D5 ----------------------------- */

// RGB LED D5: red on PD5, green on PD6, blue on PD3 (devkit schematic, nets LED_R / LED_G / LED_B)
# define RED        (1 << PD5)
# define GREEN      (1 << PD6)
# define BLUE       (1 << PD3)
# define LED_MASK   (RED | GREEN | BLUE)

// Duty register of each colour. The LED pins are output-compare pins: PD5 = OC0B, PD6 = OC0A, PD3 = OC2B (Table 14-9, Port D alternate functions)
# define RED_PWM    OCR0B
# define GREEN_PWM  OCR0A
# define BLUE_PWM   OCR2B

void    rgb_init(void);
void    set_rgb(uint8_t r, uint8_t g, uint8_t b);
void    wheel(uint8_t pos);

#endif
