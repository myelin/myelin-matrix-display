#include "matrix.h"

void setup_animation() {
  set_frame_rate(30);
}

void draw_frame(ScreenBuffer *s, int frame) {
  static int pos = 0;

  s->rect(0, 0, WIDTH, HEIGHT, wheel(pos));

  pos = (pos + 1) % (256 * 3);
}
