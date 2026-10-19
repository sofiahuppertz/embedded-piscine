MCU				= atmega328p
F_CPU 			= 16000000UL
UART_BAUDRATE	= 115200
PROGRAMMER_TYPE = arduino
# Serial port of the board, detected at run time.
# macOS: /dev/cu.usbserial-* or /dev/cu.usbmodem*   Linux: /dev/ttyUSB* or /dev/ttyACM*
# Override from the command line if needed: make flash PORT=/dev/ttyUSB1
PORT			?= $(firstword $(wildcard /dev/cu.usbserial-* /dev/cu.usbmodem* /dev/ttyUSB* /dev/ttyACM*))

CC				= avr-gcc
CFLAGS			= -Wall -Wextra -Os -mmcu=$(MCU)
OBJCOPY			= avr-objcopy
OBJDUMP			= avr-objdump
AVRDUDE			= avrdude

all: hex flash

hex: main.hex

asm: main.bin
	$(OBJDUMP) -d -S $<

main.hex: main.bin
	$(OBJCOPY) -O ihex $< $@

main.bin: main.c uart_utils.c adc_utils.c utils.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@

flash: main.hex
ifeq ($(PORT),)
	$(error No serial port found. Is the board plugged in? Or pass PORT=/dev/...)
endif
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -P $(PORT) -b $(UART_BAUDRATE) -U flash:w:$<:i

# Serial terminal at the board's baud rate (Linux / evaluation machines). Quit with Ctrl-A then K.
# On macOS, screen does not pulse DTR and shows nothing: use python3 ../monitor.py instead.
screen:
ifeq ($(PORT),)
	$(error No serial port found. Is the board plugged in? Or pass PORT=/dev/...)
endif
	screen $(PORT) $(UART_BAUDRATE)

# Kill whatever holds the serial port (a forgotten screen or monitor), so flash can run.
free:
ifeq ($(PORT),)
	$(error No serial port found. Is the board plugged in? Or pass PORT=/dev/...)
endif
	@pids=$$(lsof -t $(PORT) 2>/dev/null); \
	if [ -n "$$pids" ]; then echo "killing $$pids holding $(PORT)"; kill $$pids; else echo "$(PORT) is free"; fi

clean:
	rm -f main.hex main.bin

re: clean all

.PHONY: all hex flash screen free clean re
