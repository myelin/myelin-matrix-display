#include "matrix.h"
#include <stdlib.h>
/*
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
*/
#include "brittany_leds.h"
#include "graphics.h"

int oversampling = OVERSAMPLING;
unsigned char *main_draw_buf;
unsigned char *draw_buf;
unsigned char *downsampled_buf = NULL;

int frame_rate = 30;

void set_frame_rate(int rate) {
  frame_rate = rate;
}

void dim(int factor) {
  for (unsigned char* ptr = draw_buf; ptr < draw_buf + BUF_SIZE; ++ptr) {
    *ptr = *ptr * factor / 256;
  }
}

unsigned char* ptr_for_point(int x, int y) {
  int pos = (RGB ? 3 : 0) * (y * WIDTH + x);
  if (pos > BUF_SIZE) return NULL;
  unsigned char* ptr = draw_buf + pos;
  if (ptr < draw_buf || ptr > draw_buf + BUF_SIZE) return NULL;
  return ptr;
}

void point(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
  unsigned char* ptr = ptr_for_point(x, y);
  if (ptr == NULL) return;
  if (RGB) {
    *ptr++ = r;
    *ptr++ = g;
    *ptr++ = b;
  } else {
    *ptr++ = (r + g + b) / 3;
  }
}

void point(int x, int y, color_t c) {
  point(x, y, (unsigned char)(c >> 16), (unsigned char)(c >> 8), (unsigned char)c);
}

void point_clip(int x, int y, color_t c) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    point(x, y, c);
  }
}

color_t getpoint(int x, int y) {
  unsigned char* ptr = ptr_for_point(x, y);
  if (ptr == NULL) return 0; // everything off-canvas is black?
  uint8_t r = *ptr++;
  uint8_t g = *ptr++;
  uint8_t b = *ptr++;
  return color(r, g, b);
}

void line(int x0, int y0, int x1, int y1, color_t c) {
  if (x0 == x1) {
    // vertical
    for (int y = min(y0, y1); y < max(y0, y1); ++y) point(x0, y, c);
  } else if (y0 == y1) {
    // horizontal
    for (int x = min(x0, x1); x < max(x0, x1); ++x) point(x, y0, c);
  } else if ((y1 - y0) > (x1 - x0)) {
    // more vertical than horizontal
    for (int y = y0; y < y1; ++y) point(x0 + (y - y0) * (x1 - x0) / (y1 - y0), y, c);
  } else {
    // more horizontal than vertical
    for (int x = x0; x < x1; ++x) point(x, y0 + (x - x0) * (y1 - y0) / (x1 - x0), c);
  }
}

void rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b) {
  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      point(x, y, r, g, b);
    }
  }
}

void rect(int x0, int y0, int x1, int y1, color_t c) {
  rect(x0, y0, x1, y1, (unsigned char)(c >> 16), (unsigned char)(c >> 8), (unsigned char)c);
}

// from wikipedia, distributable under cc-a-sa
void circle(int cx, int cy, int r, color_t c) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  point_clip(cx, cy + r, c);
  point_clip(cx, cy - r, c);
  point_clip(cx + r, cy, c);
  point_clip(cx - r, cy, c);

  while(x < y) {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - r*r + 2*x - y + 1;
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    point_clip(cx + x, cy + y, c);
    point_clip(cx - x, cy + y, c);
    point_clip(cx + x, cy - y, c);
    point_clip(cx - x, cy - y, c);
    point_clip(cx + y, cy + x, c);
    point_clip(cx - y, cy + x, c);
    point_clip(cx + y, cy - x, c);
    point_clip(cx - y, cy - x, c);
  }
}

int randint(int min_inclusive, int max_inclusive) {
  return rand() % (max_inclusive - min_inclusive + 1) + min_inclusive;
}

#ifdef PLAT_DESKTOP
color_t random_color() {
  return rand() & 0xFFFFFF;
}
#endif

color_t color_fade(color_t color1, color_t color2, int pos) {
  int r1 = (color1 >> 16) & 0xFF, g1 = (color1 >> 8) & 0xFF, b1 = color1 & 0xFF;
  int r2 = (color2 >> 16) & 0xFF, g2 = (color2 >> 8) & 0xFF, b2 = color2 & 0xFF;

  int antipos = 255 - pos;
  int r = (r1 * antipos + r2 * pos) / 510, g = (g1 * antipos + g2 * pos) / 510, b = (b1 * antipos + b2 * pos) / 510;

  return (r << 16) | (g << 8) | b;
}

/* wheel(0-767) generates an approximate rainbow.  Based on Wheel()
 * from the Adafruit_WS2801 library, which requires the following
 * text in all distributions:

  Example sketch for driving Adafruit WS2801 pixels!

  Designed specifically to work with the Adafruit RGB Pixels!
  12mm Bullet shape ----> https://www.adafruit.com/products/322
  12mm Flat shape   ----> https://www.adafruit.com/products/738
  36mm Square shape ----> https://www.adafruit.com/products/683

  These pixels use SPI to transmit the color data, and have built in
  high speed PWM drivers for 24 bit color per pixel
  2 pins are required to interface

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

*/

color_t wheel(uint16_t pos) {
  if (pos < 256) {
    return color(255 - pos, pos, 0);
  } else if (pos < 512) {
    pos -= 256;
    return color(0, 255 - pos, pos);
  } else {
    pos -= 512;
    return color(pos, 0, 255 - pos);
  }
}

void blank() {
  memset(draw_buf, 0, BUF_SIZE);
}

// some effects look amazing if we send an image intended for a progressive scan display
// to a vertically snaked display without transformation.  this will imitate that.
static unsigned char *vert_snake_buf;
void vertical_snake_transform(int w, int h, int buf_size) {
  unsigned char input[buf_size];
  memcpy(input, draw_buf, buf_size);

  // we're transforming, so we don't want to do this on main_draw_buf
  draw_buf = vert_snake_buf;

  unsigned char* ptr = input;
  for (int x = 0; x < w; ++x) {
    if (x & 1) {
      // draw upwards
      for (int y = h - 1; y >= 0; --y, ptr += 3) {
	point(x, y, ptr[0], ptr[1], ptr[2]);
      }
    } else {
      // draw downward
      for (int y = 0; y < h; ++y, ptr += 3) {
	point(x, y, ptr[0], ptr[1], ptr[2]);
      }
    }
  }
}
void vertical_snake_transform() { vertical_snake_transform(WIDTH, HEIGHT, BUF_SIZE); }

// hacked up version from the new graphics code -- replace with that if possible sometime!
void chunked_vertical_snake_transform(int w, int h, int buf_size) {
  unsigned char input[buf_size];
  memcpy(input, draw_buf, buf_size);

  // we're transforming, so we don't want to do this on main_draw_buf
  draw_buf = vert_snake_buf;

  unsigned char* ptr = input;
#define N_CHUNKS 4
#define CHUNK_HEIGHT (h / N_CHUNKS)
  for (int chunk = 0; chunk < N_CHUNKS; ++chunk) {
    for (int x = 0; x < w; ++x) {
      if (x & 1) {
        // draw upwards
        for (int y = CHUNK_HEIGHT * (chunk + 1) - 1; y >= CHUNK_HEIGHT * chunk; --y, ptr += 3) {
          point(x, y, ptr[0], ptr[1], ptr[2]);
        }
      } else {
        // draw downward
        for (int y = CHUNK_HEIGHT * chunk; y < CHUNK_HEIGHT * (chunk + 1); ++y, ptr += 3) {
          point(x, y, ptr[0], ptr[1], ptr[2]);
        }
      }
    }
  }
}



// flag to say whether we should do a vertical snake transform before display
static int late_vertical_snake = 0, late_chunked_vertical_snake = 0;
void late_vertical_snake_transform() { late_vertical_snake = 1; }
void late_chunked_vertical_snake_transform() { late_chunked_vertical_snake = 1; }

void graphics_setup() {
  main_draw_buf = new unsigned char[BUF_SIZE];
  vert_snake_buf = new unsigned char[BUF_SIZE];
  if (OVERSAMPLING > 1) {
    downsampled_buf = new unsigned char[OUT_BUF_SIZE];
  }
}

void graphics_prep_frame(int frame) {
    oversampling = OVERSAMPLING;
    draw_buf = main_draw_buf;
    late_vertical_snake = late_chunked_vertical_snake = 0;
    draw_frame(frame);
#ifdef PLAT_DESKTOP
    printf("sending frame %d to display\n", frame);
#endif
    if (oversampling > 1) {
      for (int y = 0; y < OUT_H; ++y) {
  for (int x = 0; x < OUT_W; ++x) {
    // average oversampling x oversampling block of pixels from draw_buf into one pixel in downsampled_buf
    int r = 0, g = 0, b = 0;
    for (int yd = 0; yd < oversampling; ++yd) {
      for (int xd = 0; xd < oversampling; ++xd) {
        color_t c = getpoint(x * oversampling + xd, y * oversampling + yd);
        r += (c >> 16) & 0xFF;
        g += (c >> 8) & 0xFF;
        b += c & 0xFF;
      }
    }
    unsigned char* ptr = downsampled_buf + (y * OUT_W + x) * 3;
    *ptr++ = r / oversampling / oversampling;
    *ptr++ = g / oversampling / oversampling;
    *ptr++ = b / oversampling / oversampling;
  }
      }
      draw_buf = downsampled_buf;
      oversampling = 1; // here's where it gets complicated...
    }
    if (late_vertical_snake) vertical_snake_transform(OUT_W, OUT_H, OUT_BUF_SIZE);
    if (late_chunked_vertical_snake) chunked_vertical_snake_transform(OUT_W, OUT_H, OUT_BUF_SIZE);
}
