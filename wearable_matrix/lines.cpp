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

void draw_insane_lines(int frame) {
  set_frame_rate(30);

  uint32_t c = random_color();
/*  if (frame & 1) {
    // invert
    rect(0, 0, WIDTH, HEIGHT, c);
    c = 0;
  } else {*/
    blank();
//  }
  if (frame & 1) {
    // horizontal
    int y = random(0, HEIGHT);
    line(0, y, WIDTH, y, c);
  } else {
    // vertical
    for (uint8_t i = 0; i < 2; ++i) {
      int x = random(0, WIDTH);
      line(x, 0, x, HEIGHT, c);
    }
  }
}

