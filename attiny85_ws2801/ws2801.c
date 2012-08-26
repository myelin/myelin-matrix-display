/*
 * ATTINY85 code to bit-bang WS2801 strands/strips.
 * Copyright (C) 2012 Phillip Pearson
 * You may use this under a CC Attribution license.
 */

#include <avr/io.h>
#include <util/delay.h>
#include "ws2801.h"

void ws2801_setup() {
  WS2801_DATA_DDR |= 1 << WS2801_DATA_PIN;
  WS2801_CLOCK_DDR |= 1 << WS2801_CLOCK_PIN;
}

void ws2801_show(const uint8_t* data) {
#define clkpinmask (1 << WS2801_CLOCK_PIN)
#define datapinmask (1 << WS2801_DATA_PIN)

  for (const uint8_t* pixel = data; pixel != data + WS2801_BUFFER_LEN; ++pixel) {
    uint8_t pixel_value = *pixel;
#define BANG_WS2801_BIT(bit) if (pixel_value & bit) WS2801_DATA_PORT |= datapinmask; else WS2801_DATA_PORT &= ~datapinmask; WS2801_CLOCK_PORT |= clkpinmask; WS2801_CLOCK_PORT &= ~clkpinmask
    BANG_WS2801_BIT(0x80);
    BANG_WS2801_BIT(0x40);
    BANG_WS2801_BIT(0x20);
    BANG_WS2801_BIT(0x10);
    BANG_WS2801_BIT(0x08);
    BANG_WS2801_BIT(0x04);
    BANG_WS2801_BIT(0x02);
    BANG_WS2801_BIT(0x01);
  }

  WS2801_DATA_PORT &= ~datapinmask;
  _delay_ms(1);
}
