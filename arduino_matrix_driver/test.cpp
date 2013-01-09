#include "matrix.h"

void draw_test(int frame) {
  set_frame_rate(20);
  set_frames_per_mode(300);
  dim(240);
//  strip.setPixelColor(frame % (WIDTH * HEIGHT), 255, 255, 255);
//  point(2, 2, 255, 255, 255);
  point(frame % WIDTH, (frame / WIDTH) % HEIGHT, 255, 255, 255);
}
