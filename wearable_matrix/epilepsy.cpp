#include "matrix.h"

void draw_epilepsy(int frame) {
  set_frame_rate(30);

  uint32_t c = random_color();
  if ((rand() % 6) == 0) {
    rect(0, 0, WIDTH, HEIGHT, c);
  } else {
    switch (frame % 2) {
      case 0: {
        blank();
        // horizontal
        int y = random(0, HEIGHT);
        line(0, y, WIDTH, y, c);
        break;
      }
      case 1: {
        blank();
        // vertical
        for (uint8_t i = 0; i < 2; ++i) {
          int x = random(0, WIDTH);
          line(x, 0, x, HEIGHT, c);
        }
        break;
      }
    }
  }
}
