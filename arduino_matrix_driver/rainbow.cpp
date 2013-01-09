#include "matrix.h"

void draw_rainbow(int frame) {
  set_frame_rate(30);
  static uint16_t pos = 0;

  for (uint8_t y = 0; y < HEIGHT; y += 2) {
    rect(0, y, WIDTH, y + 1, wheel(pos));
  }

  pos = (pos + 1) % (256 * 3);
  
  if (frame < 128) dim(frame * 2);
}
