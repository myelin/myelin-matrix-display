#include "matrix.h"

void setup_animation() {
}

void draw_frame(int frame) {
  set_frame_rate(70);
  rect(0, 0, WIDTH, HEIGHT, ((frame % 2) == 0) ? 0xFFFFFF : 0);
}
