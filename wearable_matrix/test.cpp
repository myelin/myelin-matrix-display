#include "matrix.h"

void draw_test(int frame) {
  set_frame_rate(20);
//  blank();
  dim(150);
//  strip.setPixelColor(frame % (WIDTH * HEIGHT), 255, 255, 255);
//  point(2, 2, 255, 255, 255);
  point(frame % WIDTH, (frame / WIDTH) % HEIGHT, 255, 255, 255);
}
