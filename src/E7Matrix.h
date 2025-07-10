#pragma once

//#define GFX_NO_PRINT
#define MATRIX_SIZE 4

#include "E7Clock.h"
#include <Arduino.h>
#include <GyverMAX7219.h>
#include <math.h>

class E7Matrix {

private:
  MAX7219< MATRIX_SIZE, 1, 6, 7, 5 > _matrix;

  void _updateView(String text) {
    _matrix.clear();

    for (uint8_t seg = 0; seg < min(text.length(), MATRIX_SIZE); seg++) {
      _matrix.setCursor(seg * 8, 0);
      _matrix.print(text.charAt(seg));
    }
    _matrix.update();
  }


public:
  E7Matrix() {
  }

  void update(E7Clock clock) {
    char format[] = "hhmm";
    _updateView(String(clock.now().toString(format)));
  }

  void begin() {
    _matrix.begin();
    _matrix.setBright(15);
  }
};
