#pragma once

#define E7M_MATRIX_SIZE 4

#define E7M_MIN_LEVEL 100
#define E7M_MAX_LEVEL 800
#define E7M_MIN_BRIGHT 1
#define E7M_MAX_BRIGHT 15
#define E7M_LDR_PIN A7
#define E7M_LED_PIN 3

#include "E7Clock.h"
#include "E7Symbol.h"
#include <Arduino.h>
#include <GyverMAX7219.h>
#include <math.h>

class E7Matrix {

private:
  MAX7219< E7M_MATRIX_SIZE, 1, 6, 7, 5 > _matrix;
  uint32_t _tmr;
  uint8_t _state;
  uint16_t _delay[3];
  uint8_t _bright;
  uint8_t _ledBright;
  static const E7Symbol _e7s;

  void _updateBright() {
    uint16_t val = 1024 - analogRead(E7M_LDR_PIN);
    _bright = map(
      constrain(val, E7M_MIN_LEVEL, E7M_MAX_LEVEL),
      E7M_MIN_LEVEL, E7M_MAX_LEVEL, E7M_MIN_BRIGHT, E7M_MAX_BRIGHT);

    _matrix.setBright(_bright);
    _ledBright = _bright * 2;
  }

  void _updateView(String text, bool showDot = false) {
    _matrix.clear();

    for (uint8_t seg = 0; seg < E7M_MATRIX_SIZE; seg++) {
      uint8_t glyph[8];
      _e7s.convertBigGlyphTo8x8(_e7s.getBigSymbolGlyph(text.charAt(seg)), glyph);
      for (uint8_t i = 0; i < 8; i++) {
        _matrix.setCursor(seg * 8 + i, 0);
        _matrix.drawByte(glyph[i]);
      }
    }

    _matrix.update();
    _updateDot(showDot);
  }

  void _updateDot(bool showDot = false) {
    analogWrite(E7M_LED_PIN, showDot ? _ledBright : 0);
  }

public:
  E7Matrix(uint8_t time_delay = 10, uint8_t date_delay = 3, uint8_t temp_delay = 3)
    : _delay{
        static_cast<uint16_t>(time_delay * 1000u),
        static_cast<uint16_t>(date_delay * 1000u),
        static_cast<uint16_t>(temp_delay * 1000u)
      },
      _bright(E7M_MIN_BRIGHT), _ledBright(E7M_MIN_BRIGHT * 2) {}

  void update(E7Clock clock) {
    char format_time[] = "hhmm";
    char format_date[] = "DDMM";

    DateTime now = clock.now();

    _updateBright();

    switch (_state) {
      case 0:
        _updateView(String(now.toString(format_time)), now.second() % 2);
        break;
      case 1:
        _updateView(String(now.toString(format_date)), true);
        break;
      case 2:
        int8_t iTemp = clock.getTempInt();
        char sTemp[5];
        if (iTemp == 0) {
          snprintf(sTemp, 5, "  0C");
        } else {
          snprintf(sTemp, 5, " %c%02d", iTemp < 0 ? '-' : '+', abs(iTemp));
        }
        _updateView(String(sTemp), false);
        break;
    }

    if (millis() - _tmr >= _delay[_state]) {
      _tmr = millis();
      _state = (_state + 1) % 3;
    }
  }

  void begin() {
    _tmr = millis();
    _matrix.begin();
    _matrix.setRotation(3);
    _updateBright();
    _updateDot();
  }

  uint8_t getBright() {
    return _bright;
  }
};

const E7Symbol E7Matrix::_e7s;
