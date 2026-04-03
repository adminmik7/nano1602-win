/*
 * PC Monitor — Arduino Nano + LCD1602 (I2C)
 * Windows Edition
 * 
 * Wiring:
 *   SDA -> A4
 *   SCL -> A5
 *   VCC -> 5V
 *   GND -> GND
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Кастомный символ для шкалы
byte block[8] = {
  B11111, B11111, B11111, B11111,
  B11111, B11111, B11111, B11111
};

int cpu = 0, ram = 0;
unsigned long lastUpdate = 0;
char input[32];
byte idx = 0;

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, block);
  
  lcd.setCursor(0, 0);
  lcd.print("  PC Monitor   ");
  lcd.setCursor(0, 1);
  lcd.print("  Ready...     ");
  delay(1500);
}

void loop() {
  // Чтение данных из Serial
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      input[idx] = 0;
      parseInput();
      idx = 0;
    } else if (idx < 31) {
      input[idx++] = c;
    }
  }

  // Если данных нет 5 секунд — ждем
  if (millis() - lastUpdate > 5000) {
    lcd.setCursor(0, 0);
    lcd.print("Waiting for PC ");
    lcd.setCursor(0, 1);
    lcd.print("---------------");
    return;
  }

  updateScreen();
}

void parseInput() {
  // Ищем "CPU:XX|RAM:XX"
  char* cPtr = strstr(input, "CPU:");
  char* rPtr = strstr(input, "RAM:");
  
  if (cPtr) cpu = atoi(cPtr + 4);
  if (rPtr) ram = atoi(rPtr + 4);
  
  lastUpdate = millis();
  Serial.println("OK");
}

void updateScreen() {
  // Формат: CPU:XXX% [||||] (всегда 3 знака под число)
  lcd.setCursor(0, 0);
  lcd.print("CPU:");
  if (cpu < 100) lcd.print(" "); // Добавляем пробел, если число < 100
  lcd.print(cpu);
  lcd.print("% [");
  drawBar(cpu, 4);
  lcd.print("] "); // Пробел для очистки хвоста

  lcd.setCursor(0, 1);
  lcd.print("RAM:");
  if (ram < 100) lcd.print(" ");
  lcd.print(ram);
  lcd.print("% [");
  drawBar(ram, 4);
  lcd.print("] ");
}

void drawBar(int val, int len) {
  int blocks = map(constrain(val, 0, 100), 0, 100, 0, len);
  for (int i = 0; i < len; i++) {
    if (i < blocks) lcd.write((byte)0);
    else lcd.print(" ");
  }
}
