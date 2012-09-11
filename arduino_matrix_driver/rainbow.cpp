#include "matrix.h"

void draw_rainbow(int frame) {
  set_frame_rate(30);
  static uint16_t pos = 0;

  rect(0, 0, WIDTH, HEIGHT, wheel(pos));

  pos = (pos + 1) % (256 * 3);
}
