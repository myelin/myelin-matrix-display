#include "matrix.h"

static int pos;

void setup_animation() {
  pos = 0;
}

void draw_frame(ScreenBuffer *s, int frame) {
  s->dim(150);
  s->point(pos, 0, 0xFFFFFF);
  pos = (pos + 1) % s->pixel_count();
}
