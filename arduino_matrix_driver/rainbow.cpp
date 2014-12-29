#include "matrix.h"

void draw_rainbow(ScreenBuffer *s, int frame) {
  set_frame_rate(30);
  static uint16_t pos = 0;

  for (uint8_t y = 0; y < HEIGHT; y += 2) {
    s->rect(0, y, WIDTH, y + 1, wheel(pos));
  }

  pos = (pos + 1) % (256 * 3);
  
  if (frame < 128) s->dim(frame * 2);
}
