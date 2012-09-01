#include "matrix.h"

void draw_lines(int frame) {
  set_frame_rate(30);
  dim(240);
  if ((frame % 10) == 0) {
    if (random(0, 2)) {
      // horizontal
      int y = random(0, HEIGHT);
      line(random(0, WIDTH), y, random(0, WIDTH), y, random_color());
    } else {
      // vertical
      int x = random(0, WIDTH);
      line(x, random(0, HEIGHT), x, random(0, HEIGHT), random_color());
    }
  }
}
