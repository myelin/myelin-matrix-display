#include "matrix.h"

// atmega328p only has 2k of ram, so it can only handle OVERSAMPLE=2
// pc and teensy can do OVERSAMPLE=4, which looks beautiful
#ifdef __AVR__
#define OVERSAMPLE 2
#else
#define OVERSAMPLE 4
#endif
static ScreenBuffer screen(WIDTH * OVERSAMPLE, HEIGHT * OVERSAMPLE);

class Ripple {
 public:
  int x, y; // center
  int d; // distance from center -- starts at 0, increments each frame
  color_t c;
  int update(ScreenBuffer *s) {
    s->circle(x, y, d, c);
    d ++;

    long r = (long)d * 10000 / 14142; // hacky divide by sqrt(2)
    if (x + r >= screen.width && x - r < 0 && y + r >= screen.height && y - r < 0) {
      //printf("invisible\n");
      return 1; // circle no longer visible
    }
    return 0;
  }
};

// no std::list!!
#define MAX_RIPPLES 20
Ripple ripples[MAX_RIPPLES];
int n_ripples = 0;

int frames_until_next = 0;

void add_ripple() {
  // find room in structure
  for (int i = 0; i < MAX_RIPPLES; ++i) {
    if (ripples[i].c) continue;
    //Serial.printf("found space in ripples structure at pos %d\n", i);
    Ripple* r = &ripples[i];
    r->x = (int)rand() % (screen.width-1);
    r->y = (int)rand() % (screen.height-1);
    r->d = 0;
    r->c = random_color();
    //Serial.printf(" -- NEW RIPPLE IS %02x %02x %02x\n", r->c & 0xFF, (r->c >> 8) & 0xFF, (r->c >> 16) & 0xFF);
    n_ripples++;
    return;
  }
}

void setup_animation() {
  memset(ripples, 0, sizeof(ripples));
}

void draw_frame(ScreenBuffer *main_screen, int frame) {
  set_frame_rate(25 * OVERSAMPLE/4);
  screen.dim(200);

  if (--frames_until_next <= 0) {
    add_ripple();
    frames_until_next = randint(5 * OVERSAMPLE/4, 30 * OVERSAMPLE/4);
  }

  for (Ripple* r = ripples; r < ripples + MAX_RIPPLES; ++r) {
    if (!r->c) continue; // no ripple in this struct entry
    if (r->update(&screen)) {
      r->c = 0; // erase ripple
      --n_ripples;
    }
  }

  //screen.point(frame % screen.width, (frame / screen.width) % screen.width, 0xff, 0xff, 0xff);
  ScreenBuffer snake_buf(main_screen->width, main_screen->height);
  screen.downsample(&snake_buf, OVERSAMPLE);
#ifdef __AVR__
  snake_buf.chunked_vertical_snake_transform(main_screen);
#else
  snake_buf.vertical_snake_transform(main_screen);
#endif

}
