#include "matrix.h"

ScreenBuffer *main_draw_buf;

int frame_rate = 30;

void set_frame_rate(int rate) {
  frame_rate = rate;
}

ScreenBuffer::ScreenBuffer(int w, int h)
  : width(w), height(h)
{
  draw_buf = new uint8_t[buf_size()];
}

ScreenBuffer::~ScreenBuffer() {
  delete draw_buf;
}

void ScreenBuffer::dim(int factor) {
  for (unsigned char* ptr = draw_buf; ptr < draw_buf + buf_size(); ++ptr) {
    *ptr = *ptr * factor / 256;
  }
}

unsigned char* ScreenBuffer::ptr_for_point(int x, int y) {
  int pos = 3 * (y * width + x);
  if (pos > buf_size()) return NULL;
  unsigned char* ptr = draw_buf + pos;
  if (ptr < draw_buf || ptr > draw_buf + buf_size()) return NULL;
  return ptr;
}

void ScreenBuffer::point(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
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

void ScreenBuffer::point(int x, int y, color_t c) {
  point(x, y, (unsigned char)(c >> 16), (unsigned char)(c >> 8), (unsigned char)c);
}

void ScreenBuffer::point_clip(int x, int y, color_t c) {
  if (x >= 0 && x < width && y >= 0 && y < height) {
    point(x, y, c);
  }
}

color_t ScreenBuffer::getpoint(int x, int y) {
  unsigned char* ptr = ptr_for_point(x, y);
  if (ptr == NULL) return 0; // everything off-canvas is black?
  uint8_t r = *ptr++;
  uint8_t g = *ptr++;
  uint8_t b = *ptr++;
  return color(r, g, b);
}

void ScreenBuffer::line(int x0, int y0, int x1, int y1, color_t c) {
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

void ScreenBuffer::rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b) {
  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      point(x, y, r, g, b);
    }
  }
}

void ScreenBuffer::rect(int x0, int y0, int x1, int y1, color_t c) {
  rect(x0, y0, x1, y1, (unsigned char)(c >> 16), (unsigned char)(c >> 8), (unsigned char)c);
}

// from wikipedia, distributable under cc-a-sa
void ScreenBuffer::circle(int cx, int cy, int r, color_t c) {
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

void ScreenBuffer::blank() {
  memset(draw_buf, 0, buf_size());
}

void ScreenBuffer::copy(ScreenBuffer *dest) {
  memcpy(dest->draw_buf, draw_buf, min(dest->buf_size(), buf_size()));
}

// some effects look amazing if we send an image intended for a progressive scan display
// to a vertically snaked display without transformation.  this will imitate that.
void ScreenBuffer::vertical_snake_transform(ScreenBuffer *dest) {
  unsigned char* ptr = draw_buf;
  for (int x = 0; x < width; ++x) {
    if (x & 1) {
      // draw upwards
      for (int y = height - 1; y >= 0; --y, ptr += 3) {
        dest->point(x, y, ptr[0], ptr[1], ptr[2]);
      }
    } else {
      // draw downward
      for (int y = 0; y < height; ++y, ptr += 3) {
        dest->point(x, y, ptr[0], ptr[1], ptr[2]);
      }
    }
  }
}

void ScreenBuffer::chunked_vertical_snake_transform(ScreenBuffer *dest) {
  unsigned char* ptr = draw_buf;
#define N_CHUNKS 4
#define CHUNK_HEIGHT (height / N_CHUNKS)
  for (int chunk = 0; chunk < N_CHUNKS; ++chunk) {
    for (int x = 0; x < width; ++x) {
      if (x & 1) {
        // draw upwards
        for (int y = CHUNK_HEIGHT * (chunk + 1) - 1; y >= CHUNK_HEIGHT * chunk; --y, ptr += 3) {
          dest->point(x, y, ptr[0], ptr[1], ptr[2]);
        }
      } else {
        // draw downward
        for (int y = CHUNK_HEIGHT * chunk; y < CHUNK_HEIGHT * (chunk + 1); ++y, ptr += 3) {
          dest->point(x, y, ptr[0], ptr[1], ptr[2]);
        }
      }
    }
  }
}

void ScreenBuffer::downsample(ScreenBuffer* dest, int oversampling) {
  for (int y = 0; y < dest->height; ++y) {
    for (int x = 0; x < dest->width; ++x) {
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
      dest->point(x, y, r / oversampling / oversampling, g / oversampling / oversampling, b / oversampling / oversampling);
    }
  }
}

int randint(int min_inclusive, int max_inclusive) {
  return rand() % (max_inclusive - min_inclusive + 1) + min_inclusive;
}

color_t random_color() {
  return rand() & 0xFFFFFF;
}

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

ScreenBuffer *graphics_setup() {
  main_draw_buf = new ScreenBuffer(WIDTH, HEIGHT);
  return main_draw_buf;
}

ScreenBuffer *graphics_prep_frame(int frame) {
  draw_frame(main_draw_buf, frame);
  return main_draw_buf;
}
