#include "matrix.h"

class Ball {
 public:
  int height, v;
  Ball() {
    height = (HEIGHT - 1) * 10;
    v = 0;
  }
};

void draw_frame(int frame) {
  /*  static Ball balls[10];

  printf("height %d v %d\n", height, v);

  blank();
  rect(0, HEIGHT-1-(height/10), WIDTH, HEIGHT-1-(height/10)+1, random_color());

  height += v;
  if (height < 0) {
    height = -height;
    v = -v - 1;
    if (v < 10) {
      // reset!
      height = (HEIGHT - 1) * 10;
      v = 0;
      return;
    }
  }
  v --;*/
}
