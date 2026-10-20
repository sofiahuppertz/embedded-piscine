#include "utils.h"

int main(void){

    uart_init();
    i2c_init();

    // Single handshake with the AHT20, steps 1 to 5 of the 22.6 example; each step prints status code (TWSR) (Table 22-2):
    //   0x08 TW_START      START transmitted, bus taken
    //   0x18 TW_MT_SLA_ACK SLA+W transmitted, sensor at 0x38 answered ACK (0x20 = no ACK: wrong address or no sensor)
    i2c_start();
    i2c_stop();

    while(1) {
        ;
    }
}
