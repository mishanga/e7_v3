#include "E7Clock.h"
#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200

E7Clock clock;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("start");

  clock.begin();
  //Serial.println(clock.getFullTime());
}

void loop() {
  clock.tick();
  //Serial.println(millis());
  //Serial.println(clock.getFullTime());
  //delay(2000);
}
