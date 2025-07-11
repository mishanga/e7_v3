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
#include <math.h>

#define E7_LCD_COLS 16
#define E7_LCD_ROWS 2

#define FULL_DATE_FORMAT "DD.MM.YY hh:mm"
#define FULL_DATE_FORMAT_NO_DOT "DD.MM.YY hh mm"

enum E7_ClockModeStates : uint8_t {
  E7_SHOW = 0,
  E7_SET = 1
};

class E7Clock {

private:
  RTC_DS3231 _rtc;
  LiquidCrystal_I2C _lcd;
  uint16_t _prd;
  uint32_t _tmr;
  uint32_t _tmr_set;
  E7_ClockModeStates _mode;
  uint8_t _state;
  uint16_t _set_delay;
  Button _btnMode;
  Button _btnMinus;
  Button _btnPlus;
  DateTime _now;

  void _clearLcd() {
    _lcd.setCursor(0, 1);
    _lcd.print("                ");
  }

  void _updateView() {
    if (_mode == E7_SHOW) {
      _lcd.noBacklight();
      _lcd.setCursor(1, 0);

      char format[] = FULL_DATE_FORMAT;
      char format_no_dot[] = FULL_DATE_FORMAT_NO_DOT;
      _lcd.print(_now.toString((_now.second() % 2) ? format : format_no_dot));

      String sTemp = getTempStr();
      _lcd.setCursor(E7_LCD_COLS - sTemp.length() - 1, 1);
      _lcd.print(sTemp);
    }

    if (_mode == E7_SET) {
      _lcd.backlight();
      _lcd.setCursor(1, 0);
      _lcd.print(getFullDateTime());

      _lcd.setCursor(1 + (4 - _state) * 3, 1);
      _lcd.print("^^");
    }
  }

  void _resetView() {
    _clearLcd();
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
          _now.hour() + coef * (_state == 1),
          _now.minute() + coef * (_state == 0),  // TODO: починить убывание минут
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
    : _lcd(0x3F, E7_LCD_COLS, E7_LCD_ROWS),
      _prd(1000),
      _tmr(0),
      _tmr_set(0),
      _mode(E7_SHOW),
      _state(4),
      _set_delay(set_delay * 1000),
      _btnMode(A1),
      _btnMinus(A2),
      _btnPlus(A3) {}

  void begin() {
    _rtc.begin();
    _lcd.init();
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

  String getTempStr() {
    int8_t iTemp = getTempInt();
    String sTemp(iTemp);

    if (iTemp > 0) {
      sTemp = "+" + sTemp;
    }
    sTemp.concat("\xDF");
    sTemp.concat("C");  // °C
    return sTemp;
  }

  int8_t getTempInt() {
    return round(_rtc.getTemperature());
  }

  String getFullDateTime() {
    char format[] = FULL_DATE_FORMAT;
    return _now.toString(format);
  }
};
