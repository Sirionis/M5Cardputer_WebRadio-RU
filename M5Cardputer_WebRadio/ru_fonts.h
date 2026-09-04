/*
 * ru_fonts.h - кириллические шрифты и работа с UTF-8.
 *
 * M5GFX сам по себе кириллицу не рисует: встроенные шрифты (FreeMono*, Font0…)
 * содержат только ASCII. Здесь два компактных u8g2-шрифта из ru_fonts.c
 * оборачиваются в lgfx::U8g2font — их понимает и Display, и Unit GLASS2.
 */
#pragma once
#include <M5GFX.h>

extern "C" {
extern const uint8_t u8g2_font_9x15_t_cyrillic[];
extern const uint8_t u8g2_font_6x12_t_cyrillic[];
}

// Основной экран 240x135: моноширинный 9x15, 26 символов в строке.
static const lgfx::U8g2font fontRU(u8g2_font_9x15_t_cyrillic);
// GLASS2 OLED 128x64: 6x12, 21 символ в строке.
static const lgfx::U8g2font fontRUsmall(u8g2_font_6x12_t_cyrillic);

// ---------------------------------------------------------------------------
// ICY-метаданные российских станций приходят в UTF-8 (проверено на nashe1,
// gpm, radiorecord, rusradio), и M5GFX декодирует UTF-8 сам. Но отдельные
// icecast-серверы до сих пор отдают CP1251, а невалидный UTF-8 превратится на
// экране в мусор. Поэтому строку сначала проверяем, и только если это не
// UTF-8 — считаем её CP1251 и конвертируем.
// ---------------------------------------------------------------------------
inline bool isValidUtf8(const char* s) {
  const uint8_t* p = (const uint8_t*)s;
  while (*p) {
    uint8_t c = *p++;
    if (c < 0x80) continue;
    int extra;
    if      ((c & 0xE0) == 0xC0) extra = 1;
    else if ((c & 0xF0) == 0xE0) extra = 2;
    else if ((c & 0xF8) == 0xF0) extra = 3;
    else return false;
    while (extra--) {
      if ((*p & 0xC0) != 0x80) return false;
      p++;
    }
  }
  return true;
}

// CP1251 -> Unicode для диапазона 0x80..0xFF (0 = нет символа).
static const uint16_t CP1251_HIGH[128] = {
  0x0402,0x0403,0x201A,0x0453,0x201E,0x2026,0x2020,0x2021,
  0x20AC,0x2030,0x0409,0x2039,0x040A,0x040C,0x040B,0x040F,
  0x0452,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
  0x0000,0x2122,0x0459,0x203A,0x045A,0x045C,0x045B,0x045F,
  0x00A0,0x040E,0x045E,0x0408,0x00A4,0x0490,0x00A6,0x00A7,
  0x0401,0x00A9,0x0404,0x00AB,0x00AC,0x00AD,0x00AE,0x0407,
  0x00B0,0x00B1,0x0406,0x0456,0x0491,0x00B5,0x00B6,0x00B7,
  0x0451,0x2116,0x0454,0x00BB,0x0458,0x0405,0x0455,0x0457,
  0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
  0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
  0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
  0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
  0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
  0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
  0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
  0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
};

// Возвращает строку, гарантированно пригодную для drawString().
inline String toUtf8(const char* s) {
  if (!s) return String();
  if (isValidUtf8(s)) return String(s);

  String out;
  out.reserve(strlen(s) * 2);
  for (const uint8_t* p = (const uint8_t*)s; *p; ++p) {
    if (*p < 0x80) {
      out += (char)*p;
      continue;
    }
    uint16_t u = CP1251_HIGH[*p - 0x80];
    if (u == 0) continue;
    if (u < 0x800) {
      out += (char)(0xC0 | (u >> 6));
      out += (char)(0x80 | (u & 0x3F));
    } else {
      out += (char)(0xE0 | (u >> 12));
      out += (char)(0x80 | ((u >> 6) & 0x3F));
      out += (char)(0x80 | (u & 0x3F));
    }
  }
  return out;
}

// Обрезка строки по границе UTF-8-символа: strncpy() посреди двухбайтного
// символа даёт на экране мусор.
inline String utf8Truncate(const String& src, size_t maxChars) {
  size_t chars = 0, i = 0;
  while (i < src.length()) {
    uint8_t c = (uint8_t)src[i];
    size_t len = (c < 0x80) ? 1 : ((c & 0xE0) == 0xC0) ? 2 : ((c & 0xF0) == 0xE0) ? 3 : 4;
    if (chars + 1 > maxChars) break;
    i += len;
    chars++;
  }
  return src.substring(0, i);
}
