#include "matrix.h"

void draw_lines(int frame) {
  dim(20);
  if (random(0, 2)) {
    // horizontal
    int y = random(0, HEIGHT);
    line(0, y, WIDTH, y, random_color());
  } else {
    // vertical
    int x = random(0, WIDTH);
    line(x, 0, x, HEIGHT, random_color());
  }
}
