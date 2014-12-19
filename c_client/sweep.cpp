#include "matrix.h"

static int direction, pos;

void setup_animation() {
  direction = pos = 0;
}

void draw_frame(int frame) {
  blank();
  //dim(128);

  // horizontal line
  // sweep to center
  printf("%d\n", pos);
  int hslowdown = 1;
  rect(0, pos/hslowdown, WIDTH, pos/hslowdown+1, random_color());
  rect(0, HEIGHT-1-pos/hslowdown, WIDTH, HEIGHT-1-pos/hslowdown+1, random_color());
  // vertical lines
  // sweep to center
  rect(pos, 0, pos+1, HEIGHT, random_color());
  rect(WIDTH-1-pos, 0, WIDTH-1-pos+1, HEIGHT, random_color());
  if (++pos > WIDTH/2) {
    pos = 0;
  }
  return;

  if (direction) {
    // horizontal line
    /* // sweep down
    rect(0, pos, WIDTH, pos+1, random_color());
    if (++pos >= HEIGHT) {
      pos = 0;
      direction = 0;
    }*/
    // sweep to center
    int hslowdown = 1;
    rect(0, pos/hslowdown, WIDTH, pos/hslowdown+1, random_color());
    rect(0, HEIGHT-1-pos/hslowdown, WIDTH, HEIGHT-1-pos/hslowdown+1, random_color());
    if (++pos > HEIGHT/2*hslowdown) {
      pos = 0;
      direction = 0;
    }
  } else {
    // vertical lines
    // sweep to center
    rect(pos, 0, pos+1, HEIGHT, random_color());
    rect(WIDTH-1-pos, 0, WIDTH-1-pos+1, HEIGHT, random_color());
    if (++pos > WIDTH/2) {
      pos = 0;
      direction = 1;
    }
  }
}
