#include "utils.h"

void    i2c_init(void) {
    // Bit rate only: SCL = F_CPU / (16 + 2 * TWBR), prescaler bits TWPS1:0 in TWSR stay 00 (22.5.2, 22.9.1)
    // No DDR setup: TWEN hands PC4/PC5 to the TWI, pull-ups are the 5.1k on the board (22.5.1)
    TWBR    =   TWBR_VALUE;
}

// Wait for TWINT, then compare TWSR (prescaler bits masked, 22.9.3) with the code this step should produce (22.6 steps 2-3)
static  uint8_t i2c_wait(uint8_t expected) {
    while (! (TWCR & (1 << TWINT)));
    return TW_STATUS == expected;
}

void    i2c_stop(void) {
    // TWSTO is cleared by hardware once the STOP is on the bus; TWINT is not set afterwards, so no wait (22.9.2)
    TWCR    = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t i2c_start(uint8_t rw) {
    // Writing 1 to TWINT clears it and starts the action; TWSTA requests a START (22.6 step 1, 22.9.2)
    TWCR    = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    if (!i2c_wait(TW_START)) {                  // 0x08 (Table 22-2)
        i2c_stop();
        return 0;
    }

    // SLA+R/W: 7-bit address in bits 7..1, bit 0 = R/W (22.3.3); TWSTA dropped or a repeated START is sent
    TWDR    = (AHT20_ADDR << 1) | rw;
    TWCR    = (1 << TWINT) | (1 << TWEN);
    if (!i2c_wait(rw == TW_READ ? TW_MR_SLA_ACK : TW_MT_SLA_ACK)) {   // 0x40 / 0x18; 0x48 / 0x20 = no ACK (Tables 22-3, 22-2)
        i2c_stop();
        return 0;
    }
    return 1;
}

uint8_t i2c_write(unsigned char data) {
    // Load the byte, TWINT + TWEN sends it; expect 0x28 TW_MT_DATA_ACK (22.6 step 6, Table 22-2)
    TWDR    = data;
    TWCR    = (1 << TWINT) | (1 << TWEN);
    if (!i2c_wait(TW_MT_DATA_ACK)) {
        i2c_stop();
        return 0;
    }
    return 1;
}

void    i2c_read(void) {
    for (uint8_t i = 0; i < AHT20_DATA_LEN; i++) {
        // TWEA = ACK after the byte so the slave sends the next one; last byte NACK so the slave releases SDA (22.7.2, AHT20 p8)
        if (i < AHT20_DATA_LEN - 1) {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
            if (!i2c_wait(TW_MR_DATA_ACK))      // 0x50 (Table 22-3)
                break;
        } else {
            TWCR = (1 << TWINT) | (1 << TWEN);
            if (!i2c_wait(TW_MR_DATA_NACK))     // 0x58
                break;
        }
        print_hex_value(TWDR);                  // byte valid once TWINT is set (22.9.4)
    }
    uart_printstr("\r\n");
}

void    print_hex_value(char c) {
    // Two uppercase hex digits and a space; mask after the shift because char is signed on avr-gcc
    static const char   hex[] = "0123456789ABCDEF";

    uart_tx(hex[(c >> 4) & 0x0F]);
    uart_tx(hex[c & 0x0F]);
    uart_tx(' ');
}
