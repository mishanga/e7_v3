#pragma once

//#define GFX_NO_PRINT
#define MATRIX_SIZE 4

#include "E7Clock.h"
#include "E7Symbol.h"
#include <Arduino.h>
#include <GyverMAX7219.h>
#include <math.h>

class E7Matrix {

private:
  MAX7219< MATRIX_SIZE, 1, 6, 7, 5 > _matrix;
  uint32_t _tmr;
  uint8_t _state;
  uint16_t _delay[3];
  static const E7Symbol _e7s;

  void _updateView(String text, bool showDot = true) {
    _matrix.clear();

    for (uint8_t seg = 0; seg < min(text.length(), MATRIX_SIZE); seg++) {
      const uint8_t* glyph = _e7s.getSmallSymbolGlyph(text.charAt(seg));

      for (int i = 0; i < E7S_SMALL_SIZE; i++) {
        _matrix.setCursor(seg * 8 + i, 0);
        _matrix.drawByte(glyph[i]);
      }
    }

    _matrix.update();
    digitalWrite(LED_BUILTIN, showDot);
  }

public:
  E7Matrix(uint8_t time_delay = 10, uint8_t date_delay = 3, uint8_t temp_delay = 3)
    : _delay{
        static_cast<uint16_t>(time_delay * 1000u),
        static_cast<uint16_t>(date_delay * 1000u),
        static_cast<uint16_t>(temp_delay * 1000u)
      } {}

  void update(E7Clock clock) {
    char format_time[] = "hhmm";
    char format_date[] = "DDMM";
    DateTime now = clock.now();
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
    _matrix.begin();
    _matrix.setBright(15);
    _tmr = millis();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
  }
};

const E7Symbol E7Matrix::_e7s;
