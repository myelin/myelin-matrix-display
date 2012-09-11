#include "matrix.h"

static int pos;

void setup_animation() {
  pos = 0;
}

void draw_frame(int frame) {
	set_frame_rate(5);
  blank();
	line(pos, 0, pos, HEIGHT, random_color());
	pos = (pos + 1) % 8;
}
