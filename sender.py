#!/usr/bin/env python3
"""Send CPU load + RAM usage to Arduino Nano (nano1602.ino) via USB Serial."""

import psutil
import time
import os
import sys
import glob

BAUD = 9600
UPDATE_INTERVAL = 2  # seconds


def find_port():
    """Find available USB-Serial port."""
    for dev in sorted(glob.glob("/dev/ttyUSB*") + glob.glob("/dev/ttyACM*")):
        return dev
    return None


def main():
    import serial

    port = sys.argv[1] if len(sys.argv) > 1 else find_port()
    if not port:
        print("Error: No USB-Serial port found. Specify: python3 sender.py /dev/ttyUSB0")
        sys.exit(1)

    print(f"Connecting to {port} @ {BAUD} baud ...")

    with serial.Serial(port, BAUD, timeout=1) as ser:
        time.sleep(2)  # wait for Arduino reset
        ser.reset_input_buffer()
        print(f"Connected! Sending data every {UPDATE_INTERVAL}s (Ctrl+C to stop)")

        while True:
            cpu = psutil.cpu_percent(interval=0.5)
            ram = psutil.virtual_memory().percent
            line = f"CPU:{int(cpu)}|RAM:{int(ram)}\n"
            ser.write(line.encode())
            response = ser.readline().decode().strip()

            print(f"  {line.strip()}  ->  {response}" if response else f"  {line.strip()}")
            time.sleep(UPDATE_INTERVAL - 0.5)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nStopped.")
        sys.exit(0)
