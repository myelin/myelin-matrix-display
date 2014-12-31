#include "matrix.h"
#include <list>
using namespace std;

class Star {
public:
	int x, y, v;
	color_t c;
	
	Star(int w, int h) {
		reset(w, h);
	}
	
	void reset(int w, int h) {
		//printf("reset star\n");
		x = randint(w, w + 10);
		y = randint(0, h);
		v = -randint(1, 3);
		c = random_greyscale();
	}

	int show(ScreenBuffer *screen) {
		//printf("show star at %d, %d, %lx\n", x, y, c);
		screen->point_clip(x, y, c);
		x += v;
		if (x < 0) return 1;
		return 0;
	}
};

typedef list<Star> StarList;
StarList stars;

void setup_animation() {
}

void draw_frame(ScreenBuffer *screen, int frame) {
	set_frame_rate(15);
	screen->blank();

	while (stars.size() < 30) {
		stars.push_back(Star(screen->width, screen->height));
	}
	for (StarList::iterator it = stars.begin(); it != stars.end(); ++it) {
		if (it->show(screen)) it->reset(screen->width, screen->height);
	}
}
