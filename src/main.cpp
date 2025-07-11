#include "E7Clock.h"
#include "E7Matrix.h"
#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200

E7Clock clock(4);
E7Matrix matrix({ 5, 3, 3 });

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
