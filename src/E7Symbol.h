#pragma once

#include <Arduino.h>
#include <math.h>

#define E7S_SMALL_SIZE 5
#define E7S_BIG_SIZE 11
#define E7S_MAP_SIZE 13

struct SmallSymbol {
  char letter;
  uint8_t glyph[E7S_SMALL_SIZE];
};

struct BigSymbol {
  char letter;
  uint8_t glyph[E7S_BIG_SIZE];
};

struct E7Symbol {
  static const SmallSymbol smallSymbolMaps[E7S_MAP_SIZE];
  static const BigSymbol bigSymbolMaps[E7S_MAP_SIZE];
  static const uint8_t defaultSmallGlyph[E7S_SMALL_SIZE];
  static const uint8_t defaultBigGlyph[E7S_BIG_SIZE];

  static const uint8_t* getSmallSymbolGlyph(char c) {
    for (const auto& symbol : smallSymbolMaps) {
      if (symbol.letter == c) {
        return symbol.glyph;
      }
    }

    return defaultSmallGlyph;
  }

  static const uint8_t* getBigSymbolGlyph(char c) {
    for (const auto& symbol : bigSymbolMaps) {
      if (symbol.letter == c) {
        return symbol.glyph;
      }
    }
    return defaultBigGlyph;
  }

  static void convertBigGlyphTo8x8(const uint8_t* glyph, uint8_t* matrix) {
    // переносим верхнюю (8 строк) часть символа в матрицу as is
    for (uint8_t i = 0; i < 8; i++) {
      matrix[i] = glyph[i];
    }

    // переносим нижнюю часть, распихивая по свободным местам
    // group 0 - +/-:
    bitWrite(matrix[6], 7, bitRead(glyph[3], 3));
    bitWrite(matrix[7], 7, bitRead(glyph[6], 3));

    // group 1:
    bitWrite(matrix[0], 7, bitRead(glyph[8], 5));
    bitWrite(matrix[1], 7, bitRead(glyph[9], 5));
    bitWrite(matrix[2], 7, bitRead(glyph[10], 5));

    // group 2:
    bitWrite(matrix[3], 7, bitRead(glyph[8], 6));
    bitWrite(matrix[4], 7, bitRead(glyph[9], 6));
    bitWrite(matrix[5], 7, bitRead(glyph[10], 6));

    // group 3:
    bitWrite(matrix[2], 2, bitRead(glyph[9], 2));
    bitWrite(matrix[3], 2, bitRead(glyph[10], 2));
    bitWrite(matrix[2], 3, bitRead(glyph[9], 3));
    bitWrite(matrix[3], 3, bitRead(glyph[10], 3));
    bitWrite(matrix[2], 4, bitRead(glyph[9], 4));
    bitWrite(matrix[3], 4, bitRead(glyph[10], 4));

    // group 4:
    bitWrite(matrix[6], 2, bitRead(glyph[8], 0));
    bitWrite(matrix[7], 2, bitRead(glyph[8], 1));
    bitWrite(matrix[6], 3, bitRead(glyph[9], 0));
    bitWrite(matrix[7], 3, bitRead(glyph[9], 1));
    bitWrite(matrix[6], 4, bitRead(glyph[10], 0));
    bitWrite(matrix[7], 4, bitRead(glyph[10], 1));
  }
};

const uint8_t E7Symbol::defaultSmallGlyph[E7S_SMALL_SIZE] = { 0 };
const uint8_t E7Symbol::defaultBigGlyph[E7S_BIG_SIZE] = { 0 };

// 0: 000
// 1: 001
// 2: 010
// 3: 011
// 4: 100
// 5: 101
// 6: 110
// 7: 111
const SmallSymbol E7Symbol::smallSymbolMaps[] = {
  { '+', { 0, 2, 7, 2, 0 } },
  { '-', { 0, 0, 7, 0, 0 } },
  { 'C', { 3, 4, 4, 4, 3 } },
  { '0', { 7, 5, 5, 5, 7 } },
  { '1', { 1, 1, 1, 1, 1 } },
  { '2', { 7, 1, 7, 4, 7 } },
  { '3', { 7, 1, 3, 1, 7 } },
  { '4', { 5, 5, 7, 1, 1 } },
  { '5', { 7, 4, 7, 1, 7 } },
  { '6', { 7, 4, 7, 5, 7 } },
  { '7', { 7, 1, 1, 1, 1 } },
  { '8', { 7, 5, 7, 5, 7 } },
  { '9', { 7, 5, 7, 1, 7 } }
};

//   0: 0000000 - 0
//   3: 0000011 - 1
//  28: 0011100 - 2
//  31: 0011111 - 3
//  96: 1100000 - 4
//  99: 1100011 - 5
// 124: 1111100 - 6
// 127: 1111111 - 7
const BigSymbol E7Symbol::bigSymbolMaps[] = {
  { '+', { 0, 0, 12, 12, 63, 63, 12, 12, 0, 0, 0 } },
  { '-', { 0, 0, 0, 0, 63, 63, 0, 0, 0, 0, 0 } },
  { 'C', { 31, 31, 96, 96, 96, 96, 96, 96, 96, 31, 31 } },
  { '0', { 127, 127, 99, 99, 99, 99, 99, 99, 99, 127, 127 } },
  { '1', { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 } },
  { '2', { 127, 127, 3, 3, 127, 127, 96, 96, 96, 127, 127 } },
  { '3', { 127, 127, 3, 3, 31, 31, 3, 3, 3, 127, 127 } },
  { '4', { 99, 99, 99, 99, 127, 127, 3, 3, 3, 3, 3 } },
  { '5', { 127, 127, 96, 96, 127, 127, 3, 3, 3, 127, 127 } },
  { '6', { 127, 127, 96, 96, 127, 127, 99, 99, 99, 127, 127 } },
  { '7', { 127, 127, 3, 3, 3, 3, 3, 3, 3, 3, 3 } },
  { '8', { 127, 127, 99, 99, 127, 127, 99, 99, 99, 127, 127 } },
  { '9', { 127, 127, 99, 99, 127, 127, 3, 3, 3, 127, 127 } }
};
