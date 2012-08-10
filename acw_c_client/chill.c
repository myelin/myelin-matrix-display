#include "matrix.h"

void draw_frame(int frame) {
  static int from = 0, to = 0;
  static int pos = 0;

  if (!pos) {
    from = to;
    to = random_color();
  }

  int c = color_fade(from, to, pos);

  rect(0, 0, WIDTH, HEIGHT, c);

  if (++pos > 255) pos = 0;
}
