#include <Arduino.h>
#include "Adafruit_WS2801.h"

#define CHILL_MODE

extern Adafruit_WS2801_PP strip;

// Display dimensions
#define WIDTH 25
#define HEIGHT 12
#define PIXEL_COUNT (WIDTH*HEIGHT)
// Bytes per pixel
#define PIXEL_SIZE 3
// Bytes per frame buffer
#define BUF_SIZE (PIXEL_COUNT * PIXEL_SIZE)
#define RGB 1

// original addressing style, used at burning man 2012
//#define HORIZONTAL_ADDRESSING
// new addressing style, which reduces the number of cable connections between panels down to 1
#define VERTICAL_ADDRESSING

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

typedef uint32_t color_t;
inline color_t color(uint8_t r, uint8_t g, uint8_t b) {
  return ((color_t)r << 16) | ((color_t)g << 8) | (color_t)b;
}
#define WHITE color(255, 255, 255)
#define BLACK color(0, 0, 0)
inline uint8_t red_from_color(color_t c) { return (uint8_t)((c >> 16) & 0xFF); }
inline uint8_t green_from_color(color_t c) { return (uint8_t)((c >> 8) & 0xFF); }
inline uint8_t blue_from_color(color_t c) { return (uint8_t)(c & 0xFF); }
inline color_t color_add(color_t c, double c2) {
  return color(red_from_color(c) + red_from_color(c2),
               green_from_color(c) + green_from_color(c2),
               blue_from_color(c) + blue_from_color(c2));
}
inline color_t color_mult(color_t c, double factor) {
  return color((uint8_t)((double)red_from_color(c) * factor),
               (uint8_t)((double)green_from_color(c) * factor),
	       (uint8_t)((double)blue_from_color(c) * factor));
}
inline color_t color_mask(color_t c, color_t mask) {
  return color(red_from_color(c) * red_from_color(mask) / 256,
               green_from_color(c) * green_from_color(mask) / 256,
               blue_from_color(c) * blue_from_color(mask) / 256);
}

extern uint32_t random_color();
extern void dim(int factor);
extern void blank();
extern void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
extern void point(int x, int y, color_t c);
extern void point_clip(int x, int y, color_t c);
extern void line(int x0, int y0, int x1, int y1, color_t c);
extern void rect(int x0, int y0, int x1, int y1, color_t c);
#define WHEEL_MAX 768
extern color_t wheel(uint16_t pos);

extern void serial_write(uint8_t c);

extern void set_frame_rate(uint8_t frame_rate);
extern uint8_t frame_rate;

extern void next_mode();
