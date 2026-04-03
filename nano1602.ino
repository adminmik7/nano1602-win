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
bool waitState = false;
unsigned long lastBlink = 0;
char input[32];
byte idx = 0;

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, block);
  
  // Имитация загрузки
  lcd.setCursor(0, 0);
  lcd.print("  PC Monitor   ");
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.write((byte)0);
    delay(50);
  }
  delay(500);
  lcd.clear();
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

  // Если данных нет 3 секунды — мигающий WAIT на чистом экране
  if (millis() - lastUpdate > 3000) {
    if (!waitState) {
      lcd.clear();
      waitState = true;
      lastBlink = millis();
    }
    
    if (millis() - lastBlink > 800) {
      lcd.setCursor(6, 0);
      lcd.print("WAIT");
      delay(800);
      lcd.setCursor(6, 0);
      lcd.print("    ");
      lastBlink = millis();
    }
    return;
  }
  waitState = false;

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
  // Формат: CPU:XXX% |||||| (6 блоков без скобок)
  lcd.setCursor(0, 0);
  lcd.print("CPU:");
  if (cpu < 100) lcd.print(" ");
  lcd.print(cpu);
  lcd.print("%  ");
  drawBar(cpu, 6);
  lcd.print(" "); // Очистка хвоста

  lcd.setCursor(0, 1);
  lcd.print("RAM:");
  if (ram < 100) lcd.print(" ");
  lcd.print(ram);
  lcd.print("%  ");
  drawBar(ram, 6);
  lcd.print(" ");
}

void drawBar(int val, int len) {
  int blocks = map(constrain(val, 0, 100), 0, 100, 0, len);
  for (int i = 0; i < len; i++) {
    if (i < blocks) lcd.write((byte)0);
    else lcd.print(" ");
  }
}
