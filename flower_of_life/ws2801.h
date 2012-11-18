#ifndef __WS2801_H
#define __WS2801_H

#include <avr/io.h>

// Display dimensions
#define WIDTH 55
#define HEIGHT 1
#define PIXEL_COUNT (WIDTH*HEIGHT)
#if (PIXEL_COUNT < 256)
#define ws2801_address_t uint8_t
#else
#define ws2801_address_t uint16_t
#endif
// Bytes per pixel
#define PIXEL_SIZE 3
// Bytes per frame buffer
#define BUFFER_SIZE (PIXEL_COUNT * PIXEL_SIZE)
// Pins that connect to the WS2801 string (must be on PORTD)
#define WS2801_DATA PORTD2
#define WS2801_CLOCK PORTD4

struct color_t {
  uint8_t r, g, b;
};

extern void ws2801_setup();
extern void ws2801_show();
extern void ws2801_blit(ws2801_address_t first_led, uint8_t* data, ws2801_address_t n_leds);
extern void ws2801_point(ws2801_address_t pos, uint8_t r, uint8_t g, uint8_t b);
extern void ws2801_point(ws2801_address_t pos, color_t c);
extern void ws2801_line(ws2801_address_t x1, ws2801_address_t x2, color_t c);

#endif

