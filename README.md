# 🔌 nano1602 — Arduino Nano + LCD1602 PC Monitor

CPU load + RAM usage on a 16×2 I2C display. Data sent via USB Serial.

---

## 📁 Файлы

| Файл | Назначение |
|---|---|
| `nano1602.ino` | Прошивка для Arduino Nano |
| `sender.py` | Отправка данных с Linux-ПК |
| `start.sh` | Один скрипт: ставит Python + зависимости + запускает |
| `requirements.txt` | Зависимости для `pip install -r` |

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
CPU:45% [██████    ]
RAM:62% [█████     ]
```

Формат данных: `CPU:XX|RAM:XX\n`

## 🚀 Быстрый старт

### 1. Arduino
Залей `nano1602.ino` через Arduino IDE. Нужна библиотека `LiquidCrystal_I2C`.

### 2. Linux PC

**Один скрипт (рекомендуется):**
```bash
chmod +x start.sh
./start.sh              # авто-поиск порта
./start.sh /dev/ttyUSB0 # конкретный порт
./start.sh setup        # только установка зависимостей, без запуска
```

**Или вручную:**
```bash
python3 sender.py /dev/ttyUSB0
```

> `sender.py` и `start.sh` автоматически устанавливают все зависимости (`pyserial`, `psutil`), если они отсутствуют.

## 📊 Зависимости

**Arduino:**
- `LiquidCrystal_I2C` (менеджер библиотек Arduino IDE)

**Python:**
- `pyserial` — авто-установка
- `psutil` — авто-установка (без него работает из `/proc` — но менее точно)
