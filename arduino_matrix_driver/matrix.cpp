#include "matrix.h"

ScreenBuffer *main_draw_buf;

ScreenBuffer *graphics_setup(uint8_t* pixels) {
  main_draw_buf = new ScreenBuffer(WIDTH, HEIGHT, pixels);
  return main_draw_buf;
}

ScreenBuffer *graphics_get_buffer() {
  return main_draw_buf;
}

ScreenBuffer *graphics_prep_frame(int frame) {
  draw_frame(main_draw_buf, frame);
  return main_draw_buf;
}

#ifndef CUSTOM_FRAME_RATE_SETTER

int frame_rate = 30;

void set_frame_rate(int rate) {
  frame_rate = rate;
}

#endif // !CUSTOM_FRAME_RATE_SETTER
