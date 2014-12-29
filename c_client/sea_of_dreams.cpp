#include "matrix.h"
#include "bbc_font.h"

class SeaOfDreams {
 public:
  int update(ScreenBuffer *s) {
  	return 0;
  }
};

class SeaOfDreamsRenderer : public Animation {
public:
	int x;

	SeaOfDreamsRenderer(): Animation() {
		x = 0;
	}

	void draw(ScreenBuffer *screen, int frame) {
		set_frame_rate(5);
		screen->dim(100);
		screen->point(randint(0, screen->width), randint(0, screen->height), random_color());
		screen->text(x, 1, 0xffffffL, "hello!  this is a TEST!  rawr :P bahaha");
		x--;
	}
};

DECLARE_DISPLAY_CLASS(SeaOfDreamsRenderer);
