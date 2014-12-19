#include "matrix.h"
#include <math.h>
#include <list>
using namespace std;

#define INITIAL_MAX 1
#define FADE_RATE 5

class Twinkle {
 public:
  int x, y;
  color_t c;
  int phase;
  Twinkle() { x = y = 0; c = 0; phase = 0; }
  int update(ScreenBuffer *s) {
    double brightness = sin((double)phase / 255.0 * M_PI);
    s->point(x, y, color_mult(c, brightness));
    phase += FADE_RATE;
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
  r.c = color_mult(random_color(), INITIAL_MAX);
  twinkles.push_back(r);
}

void setup_animation() {
}

void draw_frame(ScreenBuffer *s, int frame) {
  set_frame_rate(25);
  s->dim(200);

  if (--frames_until_next <= 0) {
    add_twinkle();
    frames_until_next = randint(5, 30);
    printf("next twinkle in %d frames\n", frames_until_next);
  }

  printf("%d twinkles\n", (int)twinkles.size());
  for (Twinkle_list::iterator it = twinkles.begin(); it != twinkles.end();) {
    Twinkle_list::iterator next = it; ++next;
    if (it->update(s)) {
      twinkles.erase(it); // remove twinkle
    }
    it = next;
  }
}
