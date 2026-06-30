# CLAUDE.md

Emulation/implementation of an **"Elektronika 7-06"** clock on Arduino. It shows time,
date and temperature on an array of 4×8×8 MAX7219 LED matrices, with auto-brightness
driven by a photoresistor and a blinking dot LED. Time is kept in an RTC, and there is a
set mode controlled by three buttons.

## Hardware (real target)

- **MCU:** Arduino **Nano** (ATmega328P). `platformio.ini` says `board = uno` — that's
  fine, Uno and Nano share the same ATmega328P.
- **RTC:** **DS3231** (`RTC_DS3231`, RTClib), over I2C on A4/A5. The DS3231 has a
  built-in temperature sensor — that's the value shown in temperature mode.
- **Display:** 4× **MAX7219** 8×8 modules (`GyverMAX7219`), pins DIN=7, CLK=5, CS=6,
  `setRotation(3)`. Overall field is 32×8: one 8×8 cell per character, 4 characters.
- **LDR** (photoresistor): A7 — auto-brightness.
- **LED** (dot/colon): pin 3 (PWM), brightness proportional to the matrix brightness.
- **Buttons:** MODE=A1, PLUS=A2, MINUS=A3 (`EncButton` library, `Button` class).

## Build and run

Two PlatformIO envs (see `platformio.ini`). The firmware is one code base; the ESP
serial port is selected at compile time per board.

- **Arduino IDE:** the project also builds in the Arduino IDE. `e7_v3.ino` is almost
  empty — the real entry point lives in `src/main.cpp`.
- Library dependencies: `EncButton`, `GyverMAX7219`, `RTClib` (see
  `libraries.txt`; in PlatformIO they come from `~/Documents/Arduino/libraries` via
  `lib_extra_dirs`).

## Code layout (`src/`)

- **`main.cpp`** — `setup()`/`loop()`. Creates the global `E7Clock clock({30,5,5,5})`
  (screen durations in seconds: time / date / temperature / set-mode timeout).
  `loop()` just calls `clock.tick()`.
- **`E7Clock` (.h/.cpp)** — main logic: button polling, mode switching, screen
  rotation, time setting, reading the RTC, delegating rendering to `E7Matrix`.
- **`E7Matrix` (.h/.cpp)** — renders a 4-character string onto the matrices,
  auto-brightness from the LDR (`map`/`constrain`, `E7M_MIN/MAX_LEVEL` →
  `E7M_MIN/MAX_BRIGHT` 0..15), and drives the dot LED.
- **`E7Symbol` (.h/.cpp)** — bitmap fonts for the digits and `+ - C 0..9` in three
  sizes: `small` (5 rows), `medium` (9), `big` (11). Glyph lookup by character is a
  linear scan; `convertMediumGlyphTo8x8` / `convertBigGlyphTo8x8` pack a "tall" glyph
  into an 8×8 matrix (the lower part of the character is distributed across free bits —
  this mimics the digit shapes of the original "Elektronika"). The schematic comments
  next to the glyph tables show the character shapes.

### Modes (`_mode`) and screens (`_state`)

- **`_mode == 0` — display.** Cycles: time `hhmm` (dot blinks via `second()%2`) →
  date `DDMM` → temperature (` +NN` / ` -NN` / `  0C`). Switching is timer-driven by
  `_delay[_state]`. Ticks once per 1000 ms.
- **`_mode == 1` — set.** Entered with a short MODE press. Fields by `_state`:
  0=minutes, 1=hours, 2=day, 3=month, 4=year. MODE cycles fields, PLUS/MINUS change the
  value (`press()`/`step()` — supports auto-repeat), the active field blinks. Changes
  are written to the RTC immediately (`_rtc.adjust`). Auto-exit on the `_delay[3]`
  timeout. Ticks once per 100 ms (for blinking).

### Brightness / night mode

In `E7Matrix::_updateView`, when `_bright > 1` it draws **big** glyphs; otherwise (dark
room) it draws **medium** glyphs (a "thinner" night font that is less glaring).

## Style and conventions

- Formatting follows `.clang-format` (in the repo root). Keep the existing style.
- Private fields/methods use a `_` prefix (`_mode`, `_updateView`).
- Constants are `#define`s with module prefixes (`E7M_*`, `E7S_*`, `EB_*`, `BTN_*_PIN`).
- Target platform is AVR/ATmega328P: **conserve RAM/Flash**, avoid heavy abstractions
  and needless allocations (note that Arduino `String` is currently used).
- `EncButton` options (`EB_*`) are `#define`d in `E7Clock.h` **before** the `#include`.

## Priorities (owner's focus)

1. **Code cleanliness and style** — follow `.clang-format` and existing idioms.
2. **Timekeeping accuracy and logic** — modes, buttons, time setting, RTC handling.

## Misc

- Online Wokwi version: https://wokwi.com/projects/435810683682989057
- `.vscode/c_cpp_properties.json` and `.pio/` are in `.gitignore`.
- Default branch: `master`.
