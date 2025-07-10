#include "E7Clock.h"
#include "E7Matrix.h"
#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200

E7Clock clock(3, 2, 2, 4);
E7Matrix matrix;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  clock.begin();
  matrix.begin();
}

void loop() {
  if (clock.tick()) {
    matrix.update(clock);
  };
}
