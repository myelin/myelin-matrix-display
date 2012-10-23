#ifndef __MATRIX_H
#define __MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <list>
using namespace std;

// experimental - scale large canvas down to display
#ifndef OVERSAMPLING
#define OVERSAMPLING 1
#endif
extern int oversampling; // defaults to 1

#define OUT_W 25
#define OUT_H 12
#define OUT_BUF_SIZE (OUT_W * OUT_H * (RGB ? 3 : 1))

// arduino compatibility
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

// want it! matrix
#define WIDTH (OUT_W * oversampling)
#define HEIGHT (OUT_H * oversampling)
#define RGB 1
extern int frame_rate;

#define PIXEL_COUNT (WIDTH * HEIGHT)
#define BUF_SIZE (PIXEL_COUNT * (RGB ? 3 : 1))

typedef uint32_t color_t;
inline color_t color(uint8_t r, uint8_t g, uint8_t b) {
  return (r << 16) | (g << 8) | b;
}
inline uint8_t red_from_color(color_t c) { return (uint8_t)((c >> 16) & 0xFF); }
inline uint8_t green_from_color(color_t c) { return (uint8_t)((c >> 8) & 0xFF); }
inline uint8_t blue_from_color(color_t c) { return (uint8_t)(c & 0xFF); }
#define WHITE color(255, 255, 255)
#define BLACK color(0, 0, 0)

extern int randint(int min_inclusive, int max_inclusive);

extern void blank();
extern void dim(int factor);
extern void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
extern void point(int x, int y, color_t c);
extern void line(int x0, int y0, int x1, int y1, color_t c);
extern void rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b);
extern void rect(int x0, int y0, int x1, int y1, color_t c);
extern color_t random_color();
extern color_t color_fade(color_t color1, color_t color2, int pos);
inline color_t color_mult(color_t c, double factor) {
  return color((uint8_t)((double)red_from_color(c) * factor),
	       (uint8_t)((double)green_from_color(c) * factor),
	       (uint8_t)((double)blue_from_color(c) * factor));
}
extern void set_frame_rate(int rate);
extern color_t wheel(uint16_t pos);
extern void circle(int cx, int cy, int r, color_t c);

extern void vertical_snake_transform();
extern void late_vertical_snake_transform();

#endif // __MATRIX_H
