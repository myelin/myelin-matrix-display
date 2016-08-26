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
#ifdef PLAT_DESKTOP
      printf("invisible\n");
#endif
      return 1; // circle no longer visible
    }
#if (PLAT_DESKTOP && DEBUG)
    else {
      if (x + r >= WIDTH) printf("x + r >= width; ");
      if (x - r < 0) printf("x - r < 0; ");
      if (y + r >= HEIGHT) printf("y + r >= height; ");
      if (y - r < 0) printf("y - r < 0");
      printf("\n");
      printf("x %d y %d r %d, x + r %d || width %d height %d\n", x, y, r, x + r, WIDTH, HEIGHT);
      // printf("x + r = %d < width %d; x - r = %d >= 0; y + r = %d < height %d; y - r = %d >= 0",
      //   x + r, WIDTH, x - r, y + r, HEIGHT, y - r);
    }
#endif
    return 0;
  }
};

// no std::list!!
#define MAX_RIPPLES 20
Ripple ripples[MAX_RIPPLES];
int n_ripples = 0;

int frames_until_next = 0;

void add_ripple() {
//  static uint8_t hue = 0;
  // find room in structure
  for (int i = 0; i < MAX_RIPPLES; ++i) {
    if (ripples[i].c) continue;
//    Serial.printf("found space in ripples structure at pos %d\n", i);
    Ripple* r = &ripples[i];
    r->x = (int)random16(0, WIDTH-1);
    r->y = (int)random16(0, HEIGHT-1);
    r->d = 0;
//    hue = (rand() % 32 + 256 + 16) & 0xff;
//    hue = rand() & 0xff;
//    CRGB next_color = CHSV(hue, 255, 255);
//    hue += 3;
//    if (hue > 0x4a && hue < 0x83) hue = 0x83; // skip greens
    r->c = random_color(); //color(next_color.r, next_color.g, next_color.b);
//    r->c &= 0xFF0000; // red
//    r->c &= 0x00FF00; // green
//    r->c &= 0x0000FF; // blue
//    Serial.printf(" -- NEW RIPPLE IS %02x %02x %02x (hue %02x)\n", r->c & 0xFF, (r->c >> 8) & 0xFF, (r->c >> 16) & 0xFF, hue);
    n_ripples++;
    return;
  }
}

void setup_animation() {
  memset(ripples, 0, sizeof(ripples));
}

void draw_frame(int frame) {
  set_frame_rate(25);
  dim(200);

  if (n_ripples < MAX_RIPPLES && --frames_until_next <= 0) {
    add_ripple();
    frames_until_next =
      randint(20, 30);
//      randint(25, 50);
//    randint(50, 80);
#ifdef PLAT_DESKTOP
    printf("next ripple in %d frames\n", frames_until_next);
#endif
  }

//  Serial.printf("[%d] %d ripples; next in %d frames... ", frame, n_ripples, frames_until_next);
  for (int r = 0; r < MAX_RIPPLES; ++r) {
    if (ripples[r].c == 0) continue; // not used
    if (ripples[r].update()) {
      ripples[r].c = 0; // kill it
      --n_ripples;
    }
  }
  
//  for (int z = 0; z < 100; ++z) {
//    int rx = (int)random16(0, WIDTH-1), ry = (int)random16(0, HEIGHT-1);
//    if (rx >= 8 * OVERSAMPLING && rx < 16 * OVERSAMPLING && ry >= 8 * OVERSAMPLING && ry < 16 * OVERSAMPLING) continue;
//    point(rx, ry, random_color());
//  }
  Serial.write("drawn\n");

//  vertical_snake_transform();
//  late_vertical_snake_transform();
//  late_chunked_vertical_snake_transform();
}
