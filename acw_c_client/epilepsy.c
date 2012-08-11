#include "matrix.h"

void setup_animation() {
}

void draw_frame(int frame) {
  int c = ((rand() % 6) == 0) ? (rand() & 0xFFFFFF) : 0;
  rect(0, 0, WIDTH, HEIGHT, c);
}
