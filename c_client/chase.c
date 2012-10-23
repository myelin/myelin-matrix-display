#include "matrix.h"

static int pos;

void setup_animation() {
  pos = 0;
}

void draw_frame(int frame) {
  dim(150);
  point(pos, 0, 0xFFFFFF);
  pos = (pos + 1) % PIXEL_COUNT;
}
