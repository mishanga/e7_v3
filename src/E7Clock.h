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

  void _updateView() {
    if (_mode == 0) {
      char format_time[] = "hhmm";
      char format_date[] = "DDMM";

      switch (_state) {
        case 0:
          _matrix.update(_now.toString(format_time), _now.second() % 2);
          break;
        case 1:
          _matrix.update(_now.toString(format_date), true);
          break;
        case 2:
          int8_t iTemp = round(_rtc.getTemperature());
          char sTemp[5];
          if (iTemp == 0) {
            snprintf(sTemp, 5, "  0C");
          } else {
            snprintf(sTemp, 5, " %c%02d", iTemp < 0 ? '-' : '+', abs(iTemp));
          }
          _matrix.update(String(sTemp), false);
          break;
      }

      if (millis() - _tmr_state >= _delay[_state]) {
        _state = (_state + 1) % 3;
        _tmr_state = millis();
      }
    }

    if (_mode == 1) {
      String text;
      char min[] = "mm";
      char hour[] = "hh";
      char day[] = "DD";
      char mon[] = "MM";
      char year[] = "YYYY";
      bool blink = _now.second() % 2;

      switch (_state) {
        case 0:
          text.concat(_now.toString(hour));
          text.concat(blink ? _now.toString(min) : "  ");
          _matrix.update(text, blink);
          break;
        case 1:
          text.concat(blink ? _now.toString(hour) : "  ");
          text.concat(_now.toString(min));
          _matrix.update(text, blink);
          break;
        case 2:
          text.concat(_now.toString(mon));
          text.concat(blink ? _now.toString(day) : "  ");
          _matrix.update(text, true);
          break;
        case 3:
          text.concat(blink ? _now.toString(mon) : "  ");
          text.concat(_now.toString(day));
          _matrix.update(text, true);
          break;
        case 4:
          _matrix.update(blink ? _now.toString(year) : "    ", false);
          break;
      }
    }
  }

  void _updateTime() {
    if (_mode == 1) {
      int8_t coef = 0;
      if (_btnPlus.press() | _btnPlus.step()) {
        coef = 1;
      }
      if (_btnMinus.press() | _btnMinus.step()) {
        coef = -1;
      }

      if (coef) {
        DateTime newTime = DateTime(
          _now.year() + coef * (_state == 4),
          _now.month() + coef * (_state == 3),
          _now.day() + coef * (_state == 2),
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
    if (_mode == 0) {
      if (_btnMode.press()) {
        _mode = 1;
        _state = 4;
        _tmr_state = millis();
        _updateView();
      }
    }

    if (_mode == 1) {
      if (_btnMode.pressing() | _btnMinus.pressing() | _btnPlus.pressing()) {
        _tmr_state = millis();
      }

      if (_btnMode.press()) {
        _state = (_state + 1) % 5;
        _updateView();
      }

      if (millis() - _tmr_state >= _delay[3]) {
        _mode = 0;
        _state = 0;
        _tmr_state = millis();
        _updateView();
      }
    }
  }

public:
  E7Clock(
    uint8_t time_delay = 10,
    uint8_t date_delay = 3,
    uint8_t temp_delay = 3,
    uint8_t set_delay = 5)
    : _tmr_tick(0),
      _tmr_state(0),
      _mode(0),
      _state(0),
      _delay{
        static_cast<uint16_t>(time_delay * 1000u),
        static_cast<uint16_t>(date_delay * 1000u),
        static_cast<uint16_t>(temp_delay * 1000u),
        static_cast<uint16_t>(set_delay * 1000u)
      },
      _btnMode(BTN_MODE_PIN),
      _btnMinus(BTN_MINUS_PIN),
      _btnPlus(BTN_PLUS_PIN),
      _matrix() {}

  void begin() {
    _rtc.begin();
    _matrix.begin();
    _now = _rtc.now();
  }

  bool tick() {
    _btnMode.tick();
    _btnMinus.tick();
    _btnPlus.tick();

    _updateMode();
    _updateTime();

    if (millis() - _tmr_tick >= 1000) {
      _tmr_tick = millis();
      _now = _rtc.now();
      _updateView();
      return true;
    }
    return false;
  }
};
