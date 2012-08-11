#include "matrix.h"

static int direction, pos;

void setup_animation() {
  direction = pos = 0;
}

void draw_frame(int frame) {
  blank();

  if (direction) {
    // horizontal line
    rect(0, pos, WIDTH, pos+1, random_color());
    if (++pos >= HEIGHT) {
      pos = 0;
      direction = 0;
    }
  } else {
    // vertical lines
    rect(pos, 0, pos+1, HEIGHT, random_color());
    rect(WIDTH-1-pos, 0, WIDTH-1-pos+1, HEIGHT, random_color());
    if (++pos > WIDTH/2) {
      pos = 0;
      direction = 1;
    }
  }
}
