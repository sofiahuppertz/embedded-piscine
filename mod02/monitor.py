#!/usr/bin/env python3
"""
Serial monitor for the ATmega328P devkit on macOS.
Opens the CH340 port at 115200 8N1, pulses DTR/RTS (resets the board and
wakes up the driver), then prints everything the board sends until Ctrl-C.
Typed keys are forwarded to the board (needed from ex02 on).

Usage:  python3 mod02/monitor.py            (auto-detects /dev/cu.usbserial-*)
        python3 mod02/monitor.py /dev/cu.usbserial-10
"""
import os, sys, time, glob, select, struct, fcntl, termios, tty

# ioctl codes and flag bits for the modem control lines on macOS.
TIOCMBIS = 0x8004746c          # "modem bits set"   -> drive a line high
TIOCMBIC = 0x8004746b          # "modem bits clear" -> drive a line low
DTR_RTS  = struct.pack('I', 0x002 | 0x004)

def open_port(path):
    # O_NOCTTY: don't make it our controlling terminal. O_NONBLOCK: reads never hang.
    fd = os.open(path, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    attr = termios.tcgetattr(fd)
    attr[0] = 0                                   # iflag: no input translation
    attr[1] = 0                                   # oflag: no output translation
    attr[2] = termios.CS8 | termios.CREAD | termios.CLOCAL   # 8 data bits, enable rx, ignore modem status
    attr[3] = 0                                   # lflag: raw, no echo, no line editing
    attr[4] = attr[5] = termios.B115200           # input and output speed
    termios.tcsetattr(fd, termios.TCSANOW, attr)
    return fd

def pulse_reset(fd):
    # Low then high on DTR and RTS: the devkit ties them to RESET through a capacitor,
    # so the falling edge reboots the MCU. It is also the only thing that makes the
    # macOS CH340 driver start delivering received bytes.
    fcntl.ioctl(fd, TIOCMBIC, DTR_RTS)
    time.sleep(0.05)
    fcntl.ioctl(fd, TIOCMBIS, DTR_RTS)

def main():
    ports = sys.argv[1:] or glob.glob('/dev/cu.usbserial-*')
    if not ports:
        sys.exit("no /dev/cu.usbserial-* found, is the board plugged in?")
    path = ports[0]
    try:
        fd = open_port(path)
    except OSError as e:
        sys.exit(f"cannot open {path}: {e}\n(another program holds it? try: lsof {path})")

    print(f"listening on {path} at 115200 8N1, Ctrl-C to stop")
    pulse_reset(fd)
    # The driver hands over bytes buffered while nobody was reading, plus the glitches the
    # CH340 sees while the MCU is in reset. The bootloader keeps the MCU busy for ~1.5 s,
    # so anything arriving in the first 0.5 s cannot be from our program: throw it away.
    time.sleep(0.5)
    termios.tcflush(fd, termios.TCIFLUSH)

    # Put our own terminal in raw mode so single keystrokes reach the board immediately.
    stdin = sys.stdin.fileno()
    keyboard = os.isatty(stdin)          # False when run from a script or with stdin redirected
    if keyboard:
        saved = termios.tcgetattr(stdin)
        tty.setraw(stdin)
    try:
        while True:
            # Wait up to 100 ms for data on either the serial port or the keyboard.
            readable, _, _ = select.select([fd, stdin] if keyboard else [fd], [], [], 0.1)
            if fd in readable:
                try:
                    data = os.read(fd, 256)
                    # Board sends '\n' only; add '\r' so lines start at column 0 in raw mode.
                    sys.stdout.write(data.decode('ascii', 'replace').replace('\n', '\r\n'))
                    sys.stdout.flush()
                except (BlockingIOError, OSError):
                    pass
            if keyboard and stdin in readable:
                key = os.read(stdin, 1)
                if key == b'\x03':            # Ctrl-C in raw mode arrives as a byte, not a signal
                    break
                os.write(fd, key)
    except KeyboardInterrupt:
        pass
    finally:
        if keyboard:
            termios.tcsetattr(stdin, termios.TCSADRAIN, saved)
        os.close(fd)
        print()

if __name__ == '__main__':
    main()
