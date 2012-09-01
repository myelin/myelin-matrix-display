#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// want it! matrix
#define WIDTH 25
#define HEIGHT 12
#define RGB 1
#define FRAMERATE 30

#define PIXEL_COUNT (WIDTH * HEIGHT)
#define BUF_SIZE (PIXEL_COUNT * (RGB ? 3 : 1))

extern void blank();
extern void dim(int factor);
extern void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
extern void point(int x, int y, int c);
extern void rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b);
extern void rect(int x0, int y0, int x1, int y1, int c);
extern int random_color();
extern int color_fade(int color1, int color2, int pos);
