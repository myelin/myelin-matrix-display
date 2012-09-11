#include "matrix.h"

void setup_animation() {
	set_frame_rate(10);
}

void draw_frame(int frame) {
  static int pos = 0;

  rect(0, 0, WIDTH, HEIGHT, wheel(pos));

  if (++pos > 255) pos = 0;
}
