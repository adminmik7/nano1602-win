// Original: LiquidCrystal_I2C - I2C driver for HD44780-style LCDs (v1.1.2)
// Author: Frank de Brabander, Marco Schwartz
// License: MIT — https://github.com/marcoschwartz/LiquidCrystal_I2C

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
extern "C"
{
#include "Wiring.h"
#include "WCharacter.h"
}
#endif

#include "LiquidCrystal_I2C.h"
#include <Wire.h>

// ─── Constructor ────────────────────────────────────────

LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows) {
  _addr    = lcd_addr;
  _cols    = lcd_cols;
  _rows    = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
}

// ─── Public API ─────────────────────────────────────────

void LiquidCrystal_I2C::init() {
  init_priv();
}

void LiquidCrystal_I2C::init_priv() {
  Wire.begin();
  _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

  if (_rows > 1)
    _displayfunction |= LCD_2LINE;

  begin(_cols, _rows);
}

inline void LiquidCrystal_I2C::begin(uint8_t cols, uint8_t lines) {
  if (lines > 1) _displayfunction |= LCD_2LINE;
  _cols   = cols;
  _rows   = lines;

  for (int i = 0; i < 3; i++) {
    _init_pins();
    delay(5);
  }

  _init_pins();
  delayMicroseconds(60);

  if (_displayfunction & LCD_8BITMODE) {
    _write4Bits(0x33);
    delayMicroseconds(60);
  }

  _write4Bits(0x32);
  delayMicroseconds(60);

  command(LCD_FUNCTIONSET | _displayfunction);
  delay(4);

  command(LCD_DISPLAYCONTROL | _displaycontrol);
  delayMicroseconds(60);
  command(LCD_CLEARDISPLAY);
  delay(2);
  command(LCD_ENTRYMODESET | _displaymode);
  delay(4);
}

void LiquidCrystal_I2C::clear() {
  command(LCD_CLEARDISPLAY);
  delay(1);
}

void LiquidCrystal_I2C::home() {
  command(LCD_RETURNHOME);
  delay(2);
}

void LiquidCrystal_I2C::setCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

inline void LiquidCrystal_I2C::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

inline void LiquidCrystal_I2C::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

inline void LiquidCrystal_I2C::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

inline void LiquidCrystal_I2C::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

inline void LiquidCrystal_I2C::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

inline void LiquidCrystal_I2C::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_I2C::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LiquidCrystal_I2C::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

inline void LiquidCrystal_I2C::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

inline void LiquidCrystal_I2C::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

inline void LiquidCrystal_I2C::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

inline void LiquidCrystal_I2C::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

inline void LiquidCrystal_I2C::command(uint8_t value) {
  send(value, 0);
}

size_t LiquidCrystal_I2C::write(uint8_t value) {
  send(value, Rs);
  return 1;
}

inline void LiquidCrystal_I2C::setBacklightPin(uint8_t value) {
  (void)value;  // deprecated, use setBacklight
}

inline void LiquidCrystal_I2C::setBacklight(uint8_t value) {
  if (value > 0) backlight();
  else noBacklight();
}

inline void LiquidCrystal_I2C::noBacklight(void) {
  _backlightval = LCD_NOBACKLIGHT;
  expanderWrite(0);
}

inline void LiquidCrystal_I2C::backlight(void) {
  _backlightval = LCD_BACKLIGHT;
  expanderWrite(0);
}

uint8_t LiquidCrystal_I2C::backlight_status() {
  return (_backlightval == LCD_BACKLIGHT);
}

inline void LiquidCrystal_I2C::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7;
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) write(charmap[i]);
}

inline void LiquidCrystal_I2C::createChar(uint8_t location, const char *charmap) {
  location &= 0x7;
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) write(byte(charmap[i]));
}

// ─── Low-level I/O ──────────────────────────────────────

void LiquidCrystal_I2C::_init_pins() {
  _write4Bits(0x03 << 4);
  delayMicroseconds(4500);
  _write4Bits(0x03 << 4);
  delayMicroseconds(4500);
  _write4Bits(0x03 << 4);
  delayMicroseconds(150);
  _write4Bits(0x02 << 4);
}

void LiquidCrystal_I2C::expanderWrite(uint8_t _data) {
  Wire.beginTransmission(_addr);
  Wire.write((int)(_data) | _backlightval);
  Wire.endTransmission();
}

void LiquidCrystal_I2C::pulseEnable(uint8_t _data) {
  expanderWrite(_data | En);
  delayMicroseconds(1);
  expanderWrite(_data & ~En);
  delayMicroseconds(50);
}

void LiquidCrystal_I2C::send(uint8_t value, uint8_t mode) {
  uint8_t highnib = value & 0xf0;
  uint8_t lownib  = (value << 4) & 0xf0;
  _write4Bits(highnib | mode);
  _write4Bits(lownib  | mode);
}

inline void LiquidCrystal_I2C::_write4Bits(uint8_t _value) {
  expanderWrite(_value);
  pulseEnable(_value);
}
