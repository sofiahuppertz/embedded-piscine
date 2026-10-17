# Piscine plan — finish mod02 to mod09 by Wed 21 Oct 2026

Submissions and defenses: Wed 21, Thu 22, Fri 23 Oct.
Rules: mandatory exercises in order first. Bonus only inside its cap. 90 min stuck on
an exercise = write the blocker in one line, move on, come back later. Commit after
every exercise.

## Deadlines per module

| Module | Topic            | Done by           | Day budget |
|--------|------------------|-------------------|------------|
| mod02  | UART (ex04 bonus)| Fri 16 Oct 7:15pm | 45 min     |
| mod03  | RGB + timers     | Fri 16 Oct 11pm   | 4 h        |
| mod04  | Interrupts       | Sat 17 Oct        | 4.5 h      |
| mod05  | Analog / ADC     | Sun 18 Oct        | 6.5 h      |
| mod06  | I2C (AHT20)      | Mon 19 Oct        | full day   |
| mod09  | 7-seg / PCA9555  | Tue 20 Oct 6pm    | 6 h        |
| mod08  | SPI (APA102)     | Wed 21 Oct 1pm    | 3 h + 4 h  |
| mod07  | EEPROM           | Wed 21 Oct 11pm   | 6 h        |
| overflow + bonuses + defenses | | Thu 22 - Fri 23 | |

## Exercise time budgets

### Fri 16 Oct (6:30pm - 11pm, 4.5 h)
- [ ] mod02 ex04 Login (bonus) ............ 45 min  HARD STOP 7:15pm
- [ ] mod03 ex00 R, G, B in a loop ......... 15 min  GPIO only
- [ ] mod03 ex01 7 colors, 3-bit ........... 15 min  GPIO only
- [ ] mod03 ex02 PWM + wheel() ............. 90 min  Timer0 (OC0A/OC0B) + Timer2 (OC2B), check pins on schematic
- [ ] mod03 ex03 UART "#RRGGBB" -> LED ..... 60 min  reuse mod02 RX interrupt, hex parser
- buffer .................................... 45 min

### Sat 17 Oct (museum 2-3 h + sport 1 h, ~7 h left)
- [ ] mod04 ex00 INT0 toggles D1 ........... 90 min  no ISR macro: __vector_N + __attribute__((signal)), debounce
- [ ] mod04 ex01 Timer0 ISR drives Timer1 PWM 90 min  0->100->0 % in 1 s
- [ ] mod04 ex02 binary counter (bonus) .... 60 min  CAP. PCINT or INT0/INT1, D4 is PB4 not PB3
- [ ] mod05 ex00 if time remains ........... 60 min
- buffer .................................... 60 min

### Sun 18 Oct (church 2 h + sport 1 h, ~8 h left)
- [ ] mod05 ex00 ADC 8-bit hex, 20 ms ...... 60 min  ADLAR, AVCC ref, prescaler
- [ ] mod05 ex01 RV1 + LDR + NTC hex ....... 30 min  switch MUX, same loop
- [ ] mod05 ex02 same in 10-bit decimal .... 30 min  read ADCL then ADCH, itoa by hand
- [ ] mod05 ex03 internal temperature ...... 60 min  MUX 1000, 1.1 V ref, formula from datasheet
- [ ] mod05 ex04 wheel() + D1-D4 gauge ..... 90 min  glue mod03 ex02 + ADC
- buffer / catch-up on mod03-04 ............ 2 h

### Mon 19 Oct (full day)
- [ ] read TWI section + AHT20 datasheet ... 60 min  before any code
- [ ] mod06 ex00 i2c_init/start/stop + status 2 h    100 kHz TWBR, print TWSR codes from util/twi.h
- [ ] mod06 ex01 raw 7 bytes ............... 2 h     init cmd, trigger 0xAC 0x33 0x00, wait 80 ms, read 7
- [ ] mod06 ex02 temp + humidity, avg of 3 . 2 h     dtostrf, 20-bit values
- [ ] mod09 ex00 D9 blink via PCA9555 ...... 60 min  if ahead of schedule
- buffer .................................... rest

### Tue 20 Oct
- [ ] mod09 ex00 D9 blink via U4 ........... 60 min  PCA9555 config/output registers
- [ ] mod09 ex01 SW3 counter on D9-D11 ..... 60 min  expander input register, debounce
- [ ] mod09 ex02 "2" on rightmost digit .... 45 min  segment map + digit select
- [ ] mod09 ex03 0-9 counter ............... 30 min
- [ ] mod09 ex04 "42", two digits, no flicker 60 min  refresh digits in a timer ISR
- [ ] mod09 ex05 4-digit counter to 9999 ... 45 min
- [ ] mod09 ex06 RV1 on 4 digits ........... 30 min  reuse mod05 ex02
- [ ] mod08 ex00 SPI master, D6 red ........ 60 min  APA102 frame: start 4x00, 4 bytes per LED, end frame
- [ ] mod08 ex01 7 colors on D6 ............ 20 min
- [ ] mod08 ex02 D6-D8 chase, 250 ms ....... 20 min

### Wed 21 Oct
- [ ] mod08 ex03 RV1 gauge on D6-D8 ........ 30 min  reuse mod05
- [ ] mod08 ex04 "#RRGGBBDX" + FULLRAINBOW . 60 min  reuse mod03 ex03 parser
- [ ] mod08 ex05 RV1 + SW1 + SW2 color editor 60 min reuse mod04 buttons
- [ ] mod07 ex00 EEPROM hexdump ............ 60 min  eeprom_read_byte, 1024 bytes, 16 per line
- [ ] mod07 ex01 "> addr val" write + red ... 60 min  ANSI \e[31m, skip write if equal
- [ ] mod07 ex02 config struct, CRC-16, 4 slots, relocation, STATUS/SET_*/FACTORY_RESET  4 h
      design struct + slot layout on paper first, add a debug macro to force a write failure
- [ ] submit mod02, mod03, mod04 ........... 30 min

### Thu 22 - Fri 23 Oct
- defenses and submissions of the rest
- finish anything that hit a cap
- remaining bonuses only after everything mandatory is done

## Before each submission
- only Makefile, *.c, *.h in each exN/ (no .bin/.hex, no notes). `make clean` in every ex.
- datasheet comments present in each file
- mod02/monitor.py: move out of the module dir or confirm with staff
