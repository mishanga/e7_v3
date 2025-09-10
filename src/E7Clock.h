#pragma once

#define EB_NO_FOR
#define EB_NO_CALLBACK
#define EB_NO_COUNTER
#define EB_NO_BUFFER
#define EB_CLICK_TIME 200
#define EB_HOLD_TIME 400
#define EB_STEP_TIME 150

#include "E7Matrix.h"
#include <Arduino.h>
#include <EncButton.h>
#include <RTClib.h>

#define BTN_MODE_PIN A1
#define BTN_PLUS_PIN A2
#define BTN_MINUS_PIN A3

class E7Clock {

private:
  RTC_DS3231 _rtc;
  uint32_t _tmr_tick;
  uint32_t _tmr_state;
  uint8_t _mode;
  uint8_t _state;
  uint16_t _delay[4];
  Button _btnMode;
  Button _btnMinus;
  Button _btnPlus;
  DateTime _now;
  E7Matrix _matrix;

  void _updateView();
  void _updateTime();
  void _updateMode();

public:
  E7Clock(
    uint8_t time_delay = 10,
    uint8_t date_delay = 3,
    uint8_t temp_delay = 3,
    uint8_t set_delay = 5);

  void begin();
  bool tick();
};
