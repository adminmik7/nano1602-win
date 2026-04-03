#!/usr/bin/env python3
"""Send CPU load + RAM usage to Arduino Nano (nano1602.ino) via USB Serial.
Works with OR without psutil — falls back to /proc/stat + /proc/meminfo.
Auto-installs all dependencies."""

import time
import sys
import glob
import os
import subprocess

BAUD = 9600
UPDATE_INTERVAL = 2  # seconds


# ─── Auto-install dependencies ───────────────────────────

def _pip_install(package):
    """Install a Python package via pip."""
    print(f"[!] {package} not found, installing...")
    try:
        subprocess.check_call(
            [sys.executable, "-m", "pip", "install", "--quiet", package],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        return True
    except Exception as e:
        print(f"[✗] Failed to install {package}: {e}")
        return False


# pyserial — required (connection to Arduino)
try:
    import serial
except ImportError:
    if _pip_install("pyserial"):
        import serial
        print("[✓] pyserial installed successfully")
    else:
        print("  Manual: pip3 install pyserial")
        sys.exit(1)


# psutil — optional (better metrics, but /proc fallback works)
try:
    import psutil
    _psutil_available = True
except ImportError:
    if _pip_install("psutil"):
        import psutil
        print("[✓] psutil installed successfully (best accuracy)")
        _psutil_available = True
    else:
        print("[?] psutil install skipped, will use /proc (less accurate)")
        _psutil_available = False


# ─── Port detection ──────────────────────────────────────


def find_port():
    """Smart port detection: finds Arduino by hardware ID."""
    
    if os.name == 'nt': # Windows
        try:
            import wmi
            c = wmi.WMI()
            # Ищем устройства, в имени которых есть Arduino, CH340, FTDI или USB-SERIAL
            for item in c.Win32_PnPEntity():
                if item.Name and ( 
                    "Arduino" in item.Name or 
                    "CH340" in item.Name or 
                    "FTDI" in item.Name or 
                    "USB-SERIAL" in item.Name.upper() 
                ):
                    # Извлекаем номер COM-порта из имени (например, "... (COM3)")
                    import re
                    match = re.search(r"\(COM(\d+)\)", item.Name)
                    if match:
                        port = f"COM{match.group(1)}"
                        print(f"[✓] Found Arduino device on {port}")
                        return port
        except:
            pass # Если wmi не установлен, вернемся к простому перебору
            
        # Fallback: простой перебор COM-портов
        for i in range(1, 10):
            port = f"COM{i}"
            try:
                with serial.Serial(port, 9600, timeout=0.5) as s:
                    s.reset_input_buffer()
                    return port
            except:
                continue

    # Linux / Mac
    else:
        for dev in sorted(glob.glob("/dev/ttyUSB*") + glob.glob("/dev/ttyACM*")):
            if os.path.exists(dev):
                try:
                    with serial.Serial(dev, 9600, timeout=0.5) as s:
                        s.reset_input_buffer()
                        return dev
                except:
                    continue
                
    return None


# ─── CPU / RAM metrics ─────────────────────────────────

# Windows uses psutil; Linux can use /proc fallback
if os.name == 'nt' or _psutil_available:
    # On Windows, psutil is the only way
    def get_cpu():
        return psutil.cpu_percent(interval=0.5)

    def get_ram():
        return psutil.virtual_memory().percent

else:
    # Linux /proc fallback (only if psutil is missing)
    def _read_proc_stat():
        with open("/proc/stat") as f:
            parts = f.readline().split()
        idle = int(parts[3]) + int(parts[4])
        total = sum(int(x) for x in parts[1:])
        return idle, total

    _cpu_prev = _read_proc_stat()

    def get_cpu_proc():
        global _cpu_prev
        time.sleep(0.5)
        idle2, total2 = _read_proc_stat()
        idle_d = idle2 - _cpu_prev[0]
        total_d = total2 - _cpu_prev[1]
        _cpu_prev = (idle2, total2)
        if total_d == 0:
            return 0.0
        return max(0.0, min(100.0, (1 - idle_d / total_d) * 100))

    def get_ram_proc():
        mem = {}
        with open("/proc/meminfo") as f:
            for line in f:
                parts = line.split()
                mem[parts[0].rstrip(":")] = int(parts[1])
        total = mem.get("MemTotal", 0)
        avail = mem.get("MemAvailable", mem.get("MemFree", 0))
        if total == 0:
            return 0.0
        return max(0.0, min(100.0, (1 - avail / total) * 100))

    get_cpu = get_cpu_proc
    get_ram = get_ram_proc


# ─── Main ────────────────────────────────────────────────

def main():
    port = sys.argv[1] if len(sys.argv) > 1 else find_port()
    if not port:
        print("Error: No USB-Serial port found. Use: python3 sender.py /dev/ttyUSB0")
        sys.exit(1)

    with serial.Serial(port, BAUD, timeout=1) as ser:
        time.sleep(2)  # wait for Arduino reset
        ser.reset_input_buffer()
        print(f"Connected on {port} @ {BAUD} baud (Ctrl+C to stop)")

        while True:
            cpu = get_cpu()
            ram = get_ram()
            line = f"CPU:{int(cpu)}|RAM:{int(ram)}\n"
            ser.write(line.encode())

            timeout = time.time() + 1
            response = ""
            while time.time() < timeout:
                if ser.in_waiting:
                    try:
                        response = ser.readline().decode("utf-8", errors="replace").strip()
                    except Exception:
                        response = ""
                    break
                time.sleep(0.05)

            if response:
                print(f"  {line.strip()}  ->  {response}")
            else:
                print(f"  {line.strip()}")

            time.sleep(max(0, UPDATE_INTERVAL - 0.5))


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nStopped.")
        sys.exit(0)
