#ifndef UTILS_H
# define UTILS_H

// Build constants; defaults so util/delay.h and the macros below work even
// if the Makefile does not pass them with -D
# ifndef F_CPU
#  define F_CPU 16000000UL
# endif
# ifndef UART_BAUDRATE
#  define UART_BAUDRATE 115200
# endif

# include <avr/io.h>

/* ----------------------------- UART (TX only) ------------------------- */

// UBRR for double speed mode: F_CPU / (8 * baud) - 1 = 16 at 16 MHz, 115200 (Table 20-1, checked in Table 20-7)
# define UBRR_VALUE     ((F_CPU / 8 / UART_BAUDRATE) - 1)

void    uart_init(void);
void    uart_tx(char c);
void    uart_printstr(const char *str);

/* ----------------------------- I2C (TWI master) ----------------------- */

# include <util/twi.h>

// SCL = F_CPU / (16 + 2 * TWBR * prescaler), prescaler = 1 -> TWBR = 72 for 100 kHz at 16 MHz (22.5.2, 22.9.1)
# define TWBR_VALUE     ((F_CPU / 100000UL - 16) / 2)

// AHT20 7-bit slave address; shifted left by one on the bus, bit 0 = R/W (AHT20 datasheet 5.3, ATmega 22.3.3)
# define AHT20_ADDR     0x38

// Trigger measurement command and its two fixed parameter bytes (AHT20 Table 9, 5.4 step 2)
# define AHT20_TRIGGER  0xAC
# define AHT20_PARAM1   0x33
# define AHT20_PARAM2   0x00

// Reply: status, 20-bit humidity, 20-bit temperature, CRC (AHT20 5.4, read figure p8)
# define AHT20_DATA_LEN 7
# define AHT20_BUSY     (1 << 7)    // status byte bit 7, measurement not finished (AHT20 Table 10)
# define AHT20_FULL_SCALE   (1UL << 20) // 20-bit words, 2^20 = 1048576 is the divisor in AHT20 6.1 / 6.2

void    i2c_init(void);
uint8_t i2c_start(uint8_t rw);      // rw = TW_WRITE or TW_READ; returns 1 on success, 0 if a step failed (bus already stopped)
void    i2c_stop(void);
uint8_t i2c_write(unsigned char data);
uint8_t i2c_read(uint8_t *buf, uint8_t len);   // len bytes, ACK on all but the last; returns 1 on success

/* ----------------------------- AHT20 ---------------------------------- */

// Raw 20-bit sensor words, converted with AHT20 6.1 / 6.2: RH = h / 2^20 * 100, T = t / 2^20 * 200 - 50
typedef struct {
    uint32_t    humidity;
    uint32_t    temperature;
    uint8_t     ok;             // 0 = bus error or sensor busy, fields are not valid
}   t_aht20;

t_aht20 read_aht20_measurements(void);

// Raw 20-bit word -> physical unit (AHT20 6.1 / 6.2). Linear, so averaging raw words before or floats after gives the same result
float   aht20_humidity(uint32_t raw);       // 0..100 %RH
float   aht20_temperature(uint32_t raw);    // -50..150 C

#endif
