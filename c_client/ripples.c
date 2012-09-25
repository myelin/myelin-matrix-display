#include "matrix.h"

class Ripple {
 public:
  int x, y; // center
  int d; // distance from center -- starts at 0, increments each frame
  color_t c;
  int update() {
    circle(x, y, d, c);
    d ++;

    int r = d * 10000 / 14142; // hacky divide by sqrt(2)
    if (x + r >= WIDTH && x - r < 0 && y + r >= HEIGHT && y - r < 0) {
      printf("invisible\n");
      return 1; // circle no longer visible
    }
    return 0;
  }
};

typedef list<Ripple> Ripple_list;
Ripple_list ripples;

int frames_until_next = 0;

void add_ripple() {
  printf("add ripple\n");
  Ripple r;
  r.x = randint(0, WIDTH-1);
  r.y = randint(0, HEIGHT-1);
  r.d = 0;
  r.c = random_color();
  ripples.push_back(r);
}

void setup_animation() {
}

void draw_frame(int frame) {
  set_frame_rate(25);
  dim(200);

  if (--frames_until_next <= 0) {
    add_ripple();
    frames_until_next = randint(5, 30);
    printf("next ripple in %d frames\n", frames_until_next);
  }

  printf("%d ripples\n", (int)ripples.size());
  for (Ripple_list::iterator it = ripples.begin(); it != ripples.end();) {
    Ripple_list::iterator next = it; ++next;
    if (it->update()) {
      ripples.erase(it); // remove ripple
    }
    it = next;
  }
}
