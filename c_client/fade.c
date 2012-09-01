#include "matrix.h"

void setup_animation() {
}

void draw_frame(int frame) {
  static unsigned char r, g, b;
  int phase = frame % 16;
  if (!phase) {
    r = (unsigned char)rand();
    g = (unsigned char)rand();
    b = (unsigned char)rand();
  }
  rect(0, 0, WIDTH, HEIGHT, r * phase / 16, g * phase / 16, b * phase / 16);
}
