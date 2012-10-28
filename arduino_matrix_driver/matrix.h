#include <Arduino.h>
#include "Adafruit_WS2801.h"

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

typedef uint32_t color_t;
inline color_t color(uint8_t r, uint8_t g, uint8_t b) {
  return ((color_t)r << 16) | ((color_t)g << 8) | (color_t)b;
}

extern uint32_t random_color();
extern void dim(int factor);
extern void blank();
extern void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
extern void point(int x, int y, color_t c);
extern void point_clip(int x, int y, color_t c);
extern void line(int x0, int y0, int x1, int y1, color_t c);
extern void rect(int x0, int y0, int x1, int y1, color_t c);
extern color_t wheel(uint16_t pos);

extern void serial_write(uint8_t c);

extern void set_frame_rate(uint8_t frame_rate);
