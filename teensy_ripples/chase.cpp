#ifdef INCLUDE_CHASE
#include "matrix.h"

static int pos;

void setup_animation() {
  pos = 0;
}

void draw_frame(int frame) {
#ifdef PLAT_DESKTOP
  printf("point at %d\n", pos);
#endif
  dim(150);
  point(pos, 0, 0xFFFFFF);
  pos = (pos + 1) % PIXEL_COUNT;
}

#endif /* INCLUDE_CHASE */
