#include "sea_of_dreams.h"

SeaOfDreamsOverlay::SeaOfDreamsOverlay(ScreenBuffer *main) {
	output = new ScreenBuffer(main->width, main->height);
  tx = 0;
}

ScreenBuffer *SeaOfDreamsOverlay::overlay(ScreenBuffer *screen, int frame) {
	screen->copy(output);

#define FRAMES_PER_PIXEL 3
  if (!(frame % FRAMES_PER_PIXEL)) {
    //printf("MOVE\n");
    tx--;
  }
  double subpix = 1.0 - (double)(frame % FRAMES_PER_PIXEL) / FRAMES_PER_PIXEL;
  //printf("%f\n", subpix);
  output->text(tx, 2, 0xffffffL, "dream believe receive    live your dreams    SEA OF DREAMS", subpix);
  //screen->add_subpix(10, 0, 0xffffffL, subpix);

  return output;
}
