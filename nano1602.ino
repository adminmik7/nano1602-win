/*
 * PC Monitor — Arduino Nano + LCD1602 (I2C)
 * Line 1: CPU load + progress bar
 * Line 2: RAM usage + progress bar
 * Data via USB Serial @ 9600 baud
 *
 * Format:  CPU:XX|RAM:XX
 * Example: CPU:45|RAM:62
 *
 * Wiring:
 *   SDA -> A4
 *   SCL -> A5
 *   VCC -> 5V
 *   GND -> GND
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD1602 — адрес 0x27 (если не работает, попробуй 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ─── Кастомные символы ───────────────────────────────────
byte blockFull[8] = {
  B11111, B11111, B11111, B11111,
  B11111, B11111, B11111, B11111
};

byte blockEmpty[8] = {
  B11111, B10001, B10001, B10001,
  B10001, B10001, B10001, B11111
};

// ─── Состояние ───────────────────────────────────────────
float cpuLoad  = 0.0;
float ramUsage = 0.0;

unsigned long lastUpdate = 0;
bool connected = false;

String buffer = "";
static const unsigned long TIMEOUT_MS = 5000;

// ─── SETUP ───────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, blockFull);
  lcd.createChar(1, blockEmpty);

  // Приветствие
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   PC Monitor    ");
  lcd.setCursor(0, 1);
  lcd.print(" Waiting for PC  ");
  delay(2000);

  buffer.reserve(64);

  Serial.println("Nano1602 PC Monitor v1.0 — Ready");
}

// ─── LOOP ────────────────────────────────────────────────
void loop() {
  // Читаем Serial
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      parseData(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }

  // Таймаут — 5 сек без данных
  if (millis() - lastUpdate > TIMEOUT_MS) {
    connected = false;
  }

  updateDisplay();
  delay(200);
}

// ─── Парсинг данных ─────────────────────────────────────
void parseData(String data) {
  data.trim();

  int cpuIdx = data.indexOf("CPU:");
  int ramIdx = data.indexOf("RAM:");

  if (cpuIdx != -1) {
    int end = data.indexOf('|', cpuIdx);
    if (end == -1) end = data.length();
    cpuLoad = data.substring(cpuIdx + 4, end).toFloat();
  }

  if (ramIdx != -1) {
    int end = data.indexOf('|', ramIdx);
    if (end == -1) end = data.length();
    ramUsage = data.substring(ramIdx + 4, end).toFloat();
  }

  lastUpdate = millis();
  connected = true;
  Serial.println("OK");
}

// ─── Отрисовка ───────────────────────────────────────────
void drawBar(int col, int row, float value, int bars) {
  int filled = map(constrain((int)value, 0, 100), 0, 100, 0, bars);

  lcd.setCursor(col, row);
  for (int i = 0; i < bars; i++) {
    lcd.write((byte)(i < filled ? 0 : 1));
  }
}

void updateDisplay() {
  if (!connected) {
    // ─── Экран ожидания ────────────────────────────────
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting for PC  ");

    lcd.setCursor(0, 1);
    lcd.print("Connecting");
    static byte dots = 0;
    for (int i = 0; i < (dots % 4); i++) lcd.print('.');
    for (int i = 0; i < 6 - (dots % 4); i++) lcd.print(' ');
    dots++;
    return;
  }

  // ─── Строка 1: CPU ───────────────────────────────────
  lcd.setCursor(0, 0);
  lcd.print("CPU:");
  int cpu = constrain((int)cpuLoad, 0, 100);
  lcd.print(cpu);
  if (cpu < 10) lcd.print(" ");
  lcd.print("%");
  drawBar(9, 0, cpuLoad, 7);

  // ─── Строка 2: RAM ───────────────────────────────────
  lcd.setCursor(0, 1);
  lcd.print("RAM:");
  int ram = constrain((int)ramUsage, 0, 100);
  lcd.print(ram);
  if (ram < 10) lcd.print(" ");
  lcd.print("%");
  drawBar(9, 1, ramUsage, 7);
}
