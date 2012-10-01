#include <Arduino.h>

#define HW_LPD8806

#ifndef LED_DRIVER_INCLUDED
#define LED_DRIVER_INCLUDED
#include <LPD8806.h>
#endif

extern LPD8806 strip;

// Display dimensions
#define WIDTH 8
#define HEIGHT 5
#define PIXEL_COUNT (WIDTH*HEIGHT)
// Bytes per pixel
#define PIXEL_SIZE 3
// Bytes per frame buffer
#define BUF_SIZE (PIXEL_COUNT * PIXEL_SIZE)
#define RGB 1

// original addressing style, used at burning man 2012
#define HORIZONTAL_ADDRESSING
// new addressing style, which reduces the number of cable connections between panels down to 1
//#define VERTICAL_ADDRESSING
// rotated 180 degrees - as on the wearable mini-matrix
#define ROTATE_180

typedef uint32_t color_t;
inline color_t color(uint8_t r, uint8_t g, uint8_t b) {
  return ((color_t)r << 16) | ((color_t)g << 8) | (color_t)b;
}

extern uint32_t random_color();
extern void dim(int factor);
extern void blank();
extern void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
extern void point(int x, int y, color_t c);
extern void line(int x0, int y0, int x1, int y1, color_t c);
extern void rect(int x0, int y0, int x1, int y1, color_t c);
extern color_t wheel(uint16_t pos);

extern void serial_write(uint8_t c);

extern void set_frame_rate(uint8_t frame_rate);
