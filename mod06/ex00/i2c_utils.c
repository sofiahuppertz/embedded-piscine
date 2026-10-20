#include "utils.h"

void    i2c_init(void) {
    // Bit rate only: SCL = F_CPU / (16 + 2 * TWBR)
    TWBR    =   TWBR_VALUE;
}

static  void    i2c_wait( void ){
    // TWINT is set by hardware when the current step is done and TWSR holds its status (22.6 step 2, 22.9.2)
    while (! (TWCR & (1 << TWINT)));
}

void    print_status(void) {
    // TW_STATUS = TWSR & 0xF8, the two prescaler bits masked out (22.9.3); codes in Table 22-2
    uart_print_hex(TW_STATUS);
    uart_printstr("\r\n");
}

void    i2c_start(void) {
    // Writing 1 to TWINT clears it and starts the action; TWSTA requests a START (22.6 step 1, 22.9.2)
    // Whole register is assigned each time, as in the 22.6 example, so no stale TWSTA carries over
    TWCR    = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    i2c_wait();
    print_status();                             // expect 0x08 TW_START

    // SLA+W: 7-bit address in bits 7..1, bit 0 = 0 for write (22.3.3); TWSTA dropped or a repeated START is sent
    TWDR    = (AHT20_ADDR << 1);
    TWCR    = (1 << TWINT) | (1 << TWEN);
    i2c_wait();
    print_status();                             // expect 0x18 TW_MT_SLA_ACK, 0x20 = no ACK from the sensor
}

void    i2c_stop(void) {
    // TWSTO is cleared by hardware once the STOP is on the bus; TWINT is not set afterwards, so no wait (22.9.2)
    TWCR    = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}
