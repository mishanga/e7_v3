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

  void _updateBright();
  void _updateView(String text, bool showDot = false);
  void _updateDot(bool showDot = false);

public:
  E7Matrix();
  void begin();
  void update(String text, bool showDot);
};
