#include "matrix.h"

void draw_epilepsy(int frame) {
  set_frame_rate(30);

  if ((rand() % 6) == 0) {
    uint32_t c = random_color();
    rect(0, 0, WIDTH, HEIGHT, c);
  } else {
    blank();
  }
}
