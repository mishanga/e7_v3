#pragma once

#define E7M_MATRIX_SIZE 4
#define E7M_MIN_LEVEL 100
#define E7M_MAX_LEVEL 800
#define E7M_MIN_BRIGHT 0
#define E7M_MAX_BRIGHT 15
#define E7M_LDR_PIN A7
#define E7M_LED_PIN 3

#include "E7Symbol.h"
#include <Arduino.h>
#include <GyverMAX7219.h>

class E7Matrix {

private:
  MAX7219< E7M_MATRIX_SIZE, 1, 6, 7, 5 > _matrix;
  uint8_t _bright;
  static const E7Symbol _e7s;

  void _updateBright() {
    uint16_t val = 1024 - analogRead(E7M_LDR_PIN);
    _bright = map(
      constrain(val, E7M_MIN_LEVEL, E7M_MAX_LEVEL),
      E7M_MIN_LEVEL, E7M_MAX_LEVEL, E7M_MIN_BRIGHT, E7M_MAX_BRIGHT);

    _matrix.setBright(_bright);
  }

  void _updateView(String text, bool showDot = false) {
    _matrix.clear();

    for (uint8_t seg = 0; seg < E7M_MATRIX_SIZE; seg++) {
      uint8_t glyph[8];

      if (_bright > 1) {
        _e7s.convertBigGlyphTo8x8(_e7s.getBigSymbolGlyph(text.charAt(seg)), glyph);
      } else {
        _e7s.convertMediumGlyphTo8x8(_e7s.getMediumSymbolGlyph(text.charAt(seg)), glyph);
      }

      for (uint8_t i = 0; i < 8; i++) {
        _matrix.setCursor(seg * 8 + i, 0);
        _matrix.drawByte(glyph[i]);
      }
    }

    _matrix.update();
    _updateDot(showDot);
  }

  void _updateDot(bool showDot = false) {
    analogWrite(E7M_LED_PIN, showDot ? (_bright * 2 + 1) : 0);
  }

public:
  E7Matrix() : _bright(E7M_MIN_BRIGHT) {}

  void begin() {
    _matrix.begin();
    _matrix.setRotation(3);
    _updateBright();
    _updateDot();
  }

  void update(String s, bool showDot) {
    _updateView(s, showDot);
    _updateBright();
  }
};

const E7Symbol E7Matrix::_e7s;
