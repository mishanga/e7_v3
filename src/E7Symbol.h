#pragma once

#include <Arduino.h>

#define E7S_SMALL_SIZE 5
#define E7S_MEDIUM_SIZE 9
#define E7S_BIG_SIZE 11
#define E7S_MAP_SIZE 13

struct SmallSymbol {
  char letter;
  uint8_t glyph[E7S_SMALL_SIZE];
};

struct MediumSymbol {
  char letter;
  uint8_t glyph[E7S_MEDIUM_SIZE];
};

struct BigSymbol {
  char letter;
  uint8_t glyph[E7S_BIG_SIZE];
};

struct E7Symbol {
  static const SmallSymbol smallSymbolMaps[E7S_MAP_SIZE];
  static const MediumSymbol mediumSymbolMaps[E7S_MAP_SIZE];
  static const BigSymbol bigSymbolMaps[E7S_MAP_SIZE];

  static const uint8_t defaultSmallGlyph[E7S_SMALL_SIZE];
  static const uint8_t defaultMediumGlyph[E7S_MEDIUM_SIZE];
  static const uint8_t defaultBigGlyph[E7S_BIG_SIZE];

  static const uint8_t* getSmallSymbolGlyph(char c);
  static const uint8_t* getMediumSymbolGlyph(char c);
  static const uint8_t* getBigSymbolGlyph(char c);

  static void convertMediumGlyphTo8x8(const uint8_t* glyph, uint8_t* matrix);
  static void convertBigGlyphTo8x8(const uint8_t* glyph, uint8_t* matrix);
};
