/*
 * ATTINY85 code to bit-bang WS2801 strands/strips.
 * Copyright (C) 2012 Phillip Pearson
 * You may use this under a CC Attribution license.
 */

#include <avr/io.h>
#include <util/delay.h>
#include "ws2801.h"

#define clkpinmask (1 << WS2801_CLOCK_PIN)
#define datapinmask (1 << WS2801_DATA_PIN)

void ws2801_setup() {
  WS2801_DATA_DDR |= 1 << WS2801_DATA_PIN;
  WS2801_CLOCK_DDR |= 1 << WS2801_CLOCK_PIN;
}

static inline void bang_byte(uint8_t c) {
#define BANG_WS2801_BIT(bit) if (c & bit) WS2801_DATA_PORT |= datapinmask; else WS2801_DATA_PORT &= ~datapinmask; WS2801_CLOCK_PORT |= clkpinmask; WS2801_CLOCK_PORT &= ~clkpinmask
    BANG_WS2801_BIT(0x80);
    BANG_WS2801_BIT(0x40);
    BANG_WS2801_BIT(0x20);
    BANG_WS2801_BIT(0x10);
    BANG_WS2801_BIT(0x08);
    BANG_WS2801_BIT(0x04);
    BANG_WS2801_BIT(0x02);
    BANG_WS2801_BIT(0x01);
#undef BANG_WS2801_BIT
}

void ws2801_show(const uint8_t* data) {

  for (const uint8_t* pixel = data; pixel != data + WS2801_BUFFER_LEN; ++pixel) {
    bang_byte(*pixel);
  }

  WS2801_DATA_PORT &= ~datapinmask;
  _delay_ms(1);
}

void lpd8806_show(const uint8_t* data) {
  for (const uint8_t* pixel = data; pixel != data + WS2801_BUFFER_LEN; ++pixel) {
    bang_byte(0x80 | (*pixel >> 1));
  }
  for (uint8_t i = 0; i < 3; ++i) {
    bang_byte(0);
  }

  WS2801_DATA_PORT &= ~datapinmask;
}
