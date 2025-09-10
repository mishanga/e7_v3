#include "E7Matrix.h"

const E7Symbol E7Matrix::_e7s;

E7Matrix::E7Matrix() : _bright(E7M_MIN_BRIGHT) {}

void E7Matrix::_updateBright() {
  uint16_t val = 1024 - analogRead(E7M_LDR_PIN);
  _bright = map(
    constrain(val, E7M_MIN_LEVEL, E7M_MAX_LEVEL),
    E7M_MIN_LEVEL, E7M_MAX_LEVEL, E7M_MIN_BRIGHT, E7M_MAX_BRIGHT);

  _matrix.setBright(_bright);
}

void E7Matrix::_updateView(String text, bool showDot) {
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

void E7Matrix::_updateDot(bool showDot) {
  analogWrite(E7M_LED_PIN, showDot ? (_bright * 2 + 1) : 0);
}

void E7Matrix::begin() {
  _matrix.begin();
  _matrix.setRotation(3);
  _updateBright();
  _updateDot();
}

void E7Matrix::update(String text, bool showDot) {
  _updateView(text, showDot);
  _updateBright();
}
