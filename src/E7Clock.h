#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <math.h>

#define E7_LCD_COLS 16
#define E7_LCD_ROWS 2

char FULL_DATE_FORMAT[] = "DD.MM.YYYY hh:mm";
char FULL_DATE_FORMAT_NO_DOT[] = "DD.MM.YYYY hh mm";
char DATE_FORMAT[] = "DD.MM";
char SHORT_DATE_FORMAT[] = "DDMM";
char SHORT_TIME_FORMAT[] = "hhmm";

enum E7_Mode {
  E7_SHOW,
  E7_SET
};

enum E7_ShowMode {
  E7_TIME,
  E7_DATE,
  E7_WEATHER
};

class E7Clock {

private:
  RTC_DS3231 _rtc;
  LiquidCrystal_I2C _lcd;
  uint16_t _prd;
  uint32_t _tmr;
  E7_Mode _mode;
  uint16_t _time_delay;
  uint16_t _date_delay;
  uint16_t _temp_delay;
  E7_ShowMode _show;
  DateTime _now;

public:
  E7Clock(uint16_t time_delay = 10000, uint16_t date_delay = 3000, uint16_t temp_delay = 3000)
    : _lcd(0x3F, E7_LCD_COLS, E7_LCD_ROWS),
      _prd(1000),
      _tmr(0),
      _mode(E7_SHOW),
      _time_delay(time_delay),
      _date_delay(date_delay),
      _temp_delay(temp_delay) {}

  void begin() {
    _rtc.begin();
    _lcd.init();

    _now = _rtc.now();
  }

  void updateState() {
    Serial.println("update state");
    _tmr = millis();
  }

  void updateView() {
    Serial.println("update view");

    if (_mode == E7_SHOW) {
      _lcd.setCursor(0, 0);
      _lcd.print(_now.toString((_now.second() % 2) ? FULL_DATE_FORMAT : FULL_DATE_FORMAT_NO_DOT));

      _lcd.setCursor(0, 1);
      _lcd.print(_now.toString(DATE_FORMAT));

      String sTemp = getTemp();
      _lcd.setCursor(E7_LCD_COLS - sTemp.length(), 1);
      _lcd.print(getTemp());
    }

    if (_mode == E7_SET) {
      _lcd.setCursor(0, 0);
      _lcd.print(getFullDateTime());

      //_lcd.setCursor(5, 1);
      //_lcd.print();
    }
  }

  bool tick() {
    if (millis() - _tmr >= _prd) {
      _tmr = millis();
      _now = _rtc.now();
      updateState();
      updateView();
      return true;
    }
    return false;
  }

  /*
  String getLcdTime() {
    char format[6];
    snprintf(format, 6, (_now.second() % 2) ? "hh:mm" : "hh mm");
    Serial.print("lcd time format ");
    Serial.println(format);
    return _now.toString(format);
  }
    */

  String getTemp() {
    int8_t iTemp = round(_rtc.getTemperature());
    String sTemp(iTemp);

    if (iTemp > 0) {
      sTemp = '+' + sTemp;
    }
    sTemp += " C";
    return sTemp;
  }

  String getFullDateTime() {
    return _now.toString(FULL_DATE_FORMAT);
  }
};
