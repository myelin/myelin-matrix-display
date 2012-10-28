#include "matrix.h"

class Streamer {
 public:
  int x, y, xv, yv, c, delay;
  Streamer() {
    reset();
  }
  void reset() {
    x = rand() % WIDTH;
    y = rand() % HEIGHT;
    do {
      xv = (rand() % 3) - 1;
      yv = (rand() % 3) - 1;
    } while (!xv && !yv);
    c = random_color();
  }
  void move() {
    if (delay) {
      --delay;
      return;
    }
    point(x, y, c);
    point_clip(x-1, y, c);
    point_clip(x+1, y, c);
    point_clip(x, y-1, c);
    point_clip(x, y+1, c);
    x += xv;
    y += yv;
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
      // off screen
      reset();
    }
  }
};

#define N_STREAMERS 20
#define STREAMER_SEPARATION 10

static Streamer streamers[N_STREAMERS];

void setup_streamers() {
  for (int i = 0; i < N_STREAMERS; ++i) {
    streamers[i].delay = rand() % 100;
  }
}

void draw_streamers(int frame) {
  dim(150);
  set_frame_rate(15);
  for (int i = 0; i < N_STREAMERS; ++i) {
    //printf("streamer %d: height %d v %d delay %d\n", i, streamers[i].y, streamers[i].v, streamers[i].delay);
    streamers[i].move();
  }
}
