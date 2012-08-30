#include <Arduino.h>
#ifndef ADAFRUIT_WS2801_INCLUDED
#define ADAFRUIT_WS2801_INCLUDED
#include <Adafruit_WS2801.h>
#endif

extern Adafruit_WS2801 strip;

// Display dimensions
#define WIDTH 25
#define HEIGHT 12
#define PIXEL_COUNT (WIDTH*HEIGHT)
// Bytes per pixel
#define PIXEL_SIZE 3
// Bytes per frame buffer
#define BUF_SIZE (PIXEL_COUNT * PIXEL_SIZE)
#define RGB 1

extern uint32_t random_color();
extern void dim(int factor);
extern void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
extern void point(int x, int y, uint32_t c);
extern void line(int x0, int y0, int x1, int y1, uint32_t c);
extern void rect(int x0, int y0, int x1, int y1, uint32_t c);

extern void serial_write(uint8_t c);

