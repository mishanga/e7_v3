#include "E7Clock.h"
#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200

E7Clock clock(3, 2, 2, 4);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  clock.begin();
}

void loop() {
  clock.tick();
  //Serial.println(millis());
  //Serial.println(clock.getFullTime());
  //delay(2000);
}
