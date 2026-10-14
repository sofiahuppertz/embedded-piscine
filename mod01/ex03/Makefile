MCU				= atmega328p
F_CPU 			= 16000000UL
BAUD 			= 115200
PROGRAMMER_TYPE = arduino
# Serial port of the board, detected at run time.
# macOS: /dev/cu.usbserial-* or /dev/cu.usbmodem*   Linux: /dev/ttyUSB* or /dev/ttyACM*
# Override from the command line if needed: make flash PORT=/dev/ttyUSB1
PORT			?= $(firstword $(wildcard /dev/cu.usbserial-* /dev/cu.usbmodem* /dev/ttyUSB* /dev/ttyACM*))

CC				= avr-gcc
CFLAGS			= -Wall -Wextra -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)
OBJCOPY			= avr-objcopy
OBJDUMP			= avr-objdump
AVRDUDE			= avrdude

all: hex flash

hex: main.hex

asm: main.bin
	$(OBJDUMP) -d -S $<

main.hex: main.bin
	$(OBJCOPY) -O ihex $< $@

main.bin: main.c
	$(CC) $(CFLAGS) $< -o $@

flash: main.hex
ifeq ($(PORT),)
	$(error No serial port found. Is the board plugged in? Or pass PORT=/dev/...)
endif
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<:i

clean:
	rm -f main.hex main.bin

re: clean all

.PHONY: all hex flash clean re
