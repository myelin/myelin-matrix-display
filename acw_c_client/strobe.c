#include "matrix.h"

void setup_animation() {
}

void draw_frame(int frame) {
  rect(0, 0, WIDTH, HEIGHT, ((frame % 2) == 0) ? 0xFFFFFF : 0);
}
