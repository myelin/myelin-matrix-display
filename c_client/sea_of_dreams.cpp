#include "sea_of_dreams.h"

SeaOfDreamsOverlay::SeaOfDreamsOverlay(ScreenBuffer *main) {
	output = new ScreenBuffer(main->width, main->height);
  tx = 0;
}

ScreenBuffer *SeaOfDreamsOverlay::overlay(ScreenBuffer *screen, int frame) {
	screen->copy(output);

#define OVERLAY_C 0xffffffL
//#define OVERLAY_C 0x808080L

#define FRAMES_PER_PIXEL 3
  if (!(frame % FRAMES_PER_PIXEL)) {
    //printf("MOVE\n");
    tx--;
  }
  double subpix = 1.0 - (double)(frame % FRAMES_PER_PIXEL) / FRAMES_PER_PIXEL;
  //printf("%f\n", subpix);
  if (output->text(tx, 2, OVERLAY_C,
    "             live your dream             in your dreams             dream believe receive             what's your dream?             dream on             dream big             sea of dreams   ",
    subpix) < 0)
  {
    tx = 0;
  }
  //screen->add_subpix(10, 0, 0xffffffL, subpix);

  return output;
}
