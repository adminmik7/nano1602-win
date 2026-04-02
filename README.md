# 🔌 nano1602 — Arduino Nano + LCD1602 PC Monitor

CPU load + RAM usage on a 16×2 I2C display. Data sent via USB Serial.

---

## 📁 Файлы

| Файл | Назначение |
|---|---|
| `nano1602.ino` | Прошивка для Arduino Nano |
| `sender.py` | Отправка данных с Linux-ПК |

## 🔌 Подключение

```
Arduino Nano          LCD1602 (I2C)
──────────────────────────────────────
A4 (SDA)      ─────►  SDA
A5 (SCL)      ─────►  SCL
5V            ─────►  VCC
GND           ─────►  GND
USB           ─────►  PC (данные + питание)
```

I2C адрес: **0x27** (если не работает — попробуй 0x3F)

## 🖥️ Экран

```
CPU:45%  [███████  ]
RAM:62%  [█████    ]
```

Формат данных: `CPU:XX|RAM:XX\n`

## 🚀 Быстрый старт

### 1. Arduino
Залей `nano1602.ino` через Arduino IDE.

### 2. Linux PC

```bash
pip3 install pyserial psutil
python3 sender.py /dev/ttyUSB0
```

Или (авто-поиск порта):
```bash
python3 sender.py
```

## 📊 Зависимости

**Arduino:**
- `LiquidCrystal_I2C` (менеджер библиотек Arduino IDE)

**Python:**
- `pyserial`
- `psutil`
