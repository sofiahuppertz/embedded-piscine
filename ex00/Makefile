MCU				= atmega328p
F_CPU 			= 16000000UL
BAUD 			= 115200
PROGRAMMER_TYPE = arduino
PORT			= /dev/cu.usbserial-210

CC				= avr-gcc
CFLAGS			= -Wall -Wextra -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)
OBJCOPY			= avr-objcopy
AVRDUDE			= avrdude

all: hex flash

dude:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -P $(PORT) -b $(BAUD)

hex: main.hex

main.hex: main.bin
	$(OBJCOPY) ihex $< $@

main.bin: main.c
	$(CC) $(CFLAGS) $< -o $@

flash:hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$@:i


.PHONY: all flash clean dude

