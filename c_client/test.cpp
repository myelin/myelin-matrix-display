#include "matrix.h"

static int pos;

void setup_animation() {
  pos = 0;
}

void draw_frame(ScreenBuffer *s, int frame) {
	set_frame_rate(5);
	s->blank();
	s->line(pos, 0, pos, HEIGHT, random_color());
	pos = (pos + 1) % WIDTH;
}
