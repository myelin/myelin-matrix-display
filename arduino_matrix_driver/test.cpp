#include "matrix.h"

void draw_test(ScreenBuffer *s, int frame) {
  set_frame_rate(20);
  set_frames_per_mode(300);
  s->dim(240);
//  strip.setPixelColor(frame % (WIDTH * HEIGHT), 255, 255, 255);
//  point(2, 2, 255, 255, 255);
  s->point(frame % WIDTH, (frame / WIDTH) % HEIGHT, 255, 255, 255);
}
