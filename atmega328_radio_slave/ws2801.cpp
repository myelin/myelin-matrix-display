#include "ws2801.h"
#include <Adafruit_WS2801.h>

/*
 * Driver for various LED displays
 * (c) 2012 Phillip Pearson
 */

// LED control
Adafruit_WS2801 strip = Adafruit_WS2801(PIXEL_COUNT, WS2801_DATA, WS2801_CLOCK);

void ws2801_setup() {
  strip.begin();
  // set LEDs weakly on in R/G/B pattern.
  uint8_t c = random(0, 255);
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    for (uint8_t x = 0; x < WIDTH; ++x) {
      strip.setPixelColor(y * WIDTH + x, (x + y + c) % 24, (x + y + c + 8) % 24, (x + y + c + 16) % 24);
    }
  }
  ws2801_show();
}

void ws2801_blit(ws2801_address_t first_led, uint8_t* data, ws2801_address_t n_leds) {
  uint8_t* dest = strip.pixels + first_led * PIXEL_SIZE;
  uint8_t* data_end = data + n_leds * PIXEL_SIZE;

  while (data != data_end) *dest++ = *data++;
}

void ws2801_point(ws2801_address_t pos, uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(pos, r, g, b);
}

void ws2801_point(ws2801_address_t pos, color_t c) {
  strip.setPixelColor(pos, c.r, c.g, c.b);
}

void ws2801_line(ws2801_address_t x1, ws2801_address_t x2, color_t c) {
  for (ws2801_address_t x = x1; x < x2; ++x) {
    ws2801_point(x, c);
  }
}

// quickly (6.25ms) bit-bang 900 bytes into the ws2801 strip
void ws2801_show() {
  Serial.println("SHOW");
// clock pin PORTD4 (avr pin 6, arduino pin 4)
#define clkpinmask 0x10
// data pin PORTD2 (avr pin 4, arduino pin 2)
#define datapinmask 0x04

  for (uint8_t *stop_ptr = strip.pixels + strip.numPixels() * 3,  *pixel = strip.pixels; pixel != stop_ptr; ++pixel) {
    uint8_t pixel_value = *pixel;
#define BANG_WS2801_BIT(bit) if (pixel_value & bit) PORTD |= datapinmask; else PORTD &= ~datapinmask; PORTD |= clkpinmask; PORTD &= ~clkpinmask
    BANG_WS2801_BIT(0x80);
    BANG_WS2801_BIT(0x40);
    BANG_WS2801_BIT(0x20);
    BANG_WS2801_BIT(0x10);
    BANG_WS2801_BIT(0x08);
    BANG_WS2801_BIT(0x04);
    BANG_WS2801_BIT(0x02);
    BANG_WS2801_BIT(0x01);
  }

  PORTD &= ~datapinmask;
  delay(1);
}

