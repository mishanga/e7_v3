#pragma once

#define EB_NO_FOR
#define EB_NO_CALLBACK
#define EB_NO_COUNTER
#define EB_NO_BUFFER
#define EB_CLICK_TIME 200
#define EB_HOLD_TIME 400
#define EB_STEP_TIME 150

#include <Arduino.h>
#include <EncButton.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

#define BTN_MODE_PIN A1
#define BTN_PLUS_PIN A2
#define BTN_MINUS_PIN A3

#define FULL_DATE_FORMAT "DD.MM.YY hh:mm"
#define FULL_DATE_FORMAT_NO_DOT "DD.MM.YY hh mm"

enum E7_ClockMode : uint8_t {
  E7_SHOW = 0,
  E7_SET = 1
};

class E7Clock {

private:
  RTC_DS3231 _rtc;
  uint16_t _prd;
  uint32_t _tmr;
  uint32_t _tmr_set;
  E7_ClockMode _mode;
  uint8_t _state;
  uint16_t _set_delay;
  Button _btnMode;
  Button _btnMinus;
  Button _btnPlus;
  DateTime _now;

  void _updateView() {
    if (_mode == E7_SHOW) {
    }

    if (_mode == E7_SET) {
    }
  }

  void _resetView() {
    _updateView();
  }

  void _updateTime() {
    if (_mode == E7_SET) {
      int8_t coef = 0;
      if (_btnPlus.press() | _btnPlus.step()) {
        coef = 1;
      }
      if (_btnMinus.press() | _btnMinus.step()) {
        coef = -1;
      }

      if (coef) {
        DateTime newTime = DateTime(
          _now.year() + coef * (_state == 2),
          _now.month() + coef * (_state == 3),
          _now.day() + coef * (_state == 4),
          (_now.hour() + 24 + coef * (_state == 1)) % 24,
          (_now.minute() + 60 + coef * (_state == 0)) % 60,
          _now.second());

        _rtc.adjust(newTime);
        _now = _rtc.now();
        _updateView();
      }
    }
  }

  void _updateMode() {
    if (_mode == E7_SHOW) {
      if (_btnMode.press()) {
        _mode = E7_SET;
        _resetView();
      }
    }

    if (_mode == E7_SET) {
      if (_btnMode.pressing() | _btnMinus.pressing() | _btnPlus.pressing()) {
        _tmr_set = millis();
      }

      if (_btnMode.press()) {
        _state = (_state + 1) % 5;
        _resetView();
      }

      if (millis() - _tmr_set >= _set_delay) {
        _mode = E7_SHOW;
        _resetView();
      }
    }
  }

public:
  E7Clock(uint16_t set_delay = 10)
    : _prd(1000),
      _tmr(0),
      _tmr_set(0),
      _mode(E7_SHOW),
      _state(4),
      _set_delay(set_delay * 1000),
      _btnMode(BTN_MODE_PIN),
      _btnMinus(BTN_MINUS_PIN),
      _btnPlus(BTN_PLUS_PIN) {}

  void begin() {
    _rtc.begin();
    _now = _rtc.now();
  }

  bool tick() {
    _btnMode.tick();
    _btnMinus.tick();
    _btnPlus.tick();

    _updateMode();
    _updateTime();

    if (millis() - _tmr >= _prd) {
      _tmr = millis();
      _now = _rtc.now();
      _updateView();
      return true;
    }
    return false;
  }

  DateTime now() {
    return _now;
  }

  int8_t getTempInt() {
    return round(_rtc.getTemperature());
  }

  String getFullDateTime() {
    char format[] = FULL_DATE_FORMAT;
    return _now.toString(format);
  }
};
