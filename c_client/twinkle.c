#include "matrix.h"
#include <math.h>

class Twinkle {
 public:
  int x, y;
  color_t c;
  int phase;
  Twinkle() { x = y = 0; c = 0; phase = 0; }
  int update() {
    double brightness = sin((double)phase / 255.0 * M_PI);
    point(x, y, color_mult(c, brightness));
    phase += 10;
    if (phase > 255) return 1;
    return 0;
  }
};

typedef list<Twinkle> Twinkle_list;
Twinkle_list twinkles;

int frames_until_next = 0;

void add_twinkle() {
  printf("add twinkle\n");
  Twinkle r;
  r.x = randint(0, WIDTH-1);
  r.y = randint(0, HEIGHT-1);
  r.c = random_color();
  twinkles.push_back(r);
}

void setup_animation() {
}

void draw_frame(int frame) {
  set_frame_rate(25);
  dim(200);

  if (--frames_until_next <= 0) {
    add_twinkle();
    frames_until_next = randint(5, 30);
    printf("next twinkle in %d frames\n", frames_until_next);
  }

  printf("%d twinkles\n", (int)twinkles.size());
  for (Twinkle_list::iterator it = twinkles.begin(); it != twinkles.end();) {
    Twinkle_list::iterator next = it; ++next;
    if (it->update()) {
      twinkles.erase(it); // remove twinkle
    }
    it = next;
  }
}
