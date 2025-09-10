#include "E7Clock.h"
#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200

E7Clock clock({ 30, 5, 5, 5 });

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  clock.begin();
}

void loop() {
  clock.tick();
}
