#include "matrix.h"

static int pos;

void setup_animation() {
  pos = 0;
}

void draw_frame(int frame) {
  blank();
  point(pos, 0, (unsigned char)rand(), (unsigned char)rand(), (unsigned char)rand());
  pos = (pos + 1) % PIXEL_COUNT;
}
