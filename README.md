# 🔌 nano1602-win — Windows Edition

CPU load + RAM usage on a 16×2 I2C display. Data sent via USB Serial.
**This version is optimized for Windows users.**

---

## 📁 Files

| File | Purpose |
|---|---|
| `nano1602.ino` | Arduino Nano firmware |
| `sender.py` | Python script to send data |
| `requirements.txt` | Python dependencies |
| `start.bat` | **One-click launcher for Windows** |

## 🔌 Connection

```
Arduino Nano          LCD1602 (I2C)
──────────────────────────────────────
A4 (SDA)      ─────►  SDA
A5 (SCL)      ─────►  SCL
5V            ─────►  VCC
GND           ─────►  GND
USB           ─────►  PC
```
*I2C Address: 0x27 (or 0x3F)*

## 🚀 Quick Start (Windows)

### 1. Arduino Setup
Upload `nano1602.ino` to your board using Arduino IDE.

### 2. Windows PC Setup
1. **Install Python:** Download from [python.org](https://www.python.org/downloads/).
   > ⚠️ **Important:** During installation, check **"Add Python to PATH"**.
2. **Download Project:** Get the latest release from the [Releases page](https://github.com/adminmik7/nano1602-win/releases).
3. **Run:** Double-click **`start.bat`**.

The script will automatically install `pyserial` and `psutil` and start sending data.

## ⚙️ Manual Port Selection
If the script doesn't find your Arduino automatically, edit `sender.py` and change the `port` variable to your COM port (e.g., `COM3`).
