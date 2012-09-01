#include "matrix.h"

class Ball {
 public:
  int x, y, xv, v, c, delay;
  Ball() {
    reset();
  }
  void reset() {
    x = rand() % WIDTH;
    y = HEIGHT * 10 - 5;
    xv = (rand() % 3) - 1;
    v = 0;
    c = random_color();
  }
  void fall() {
    if (delay) {
      --delay;
      return;
    }
    x += xv;
    if (x < 0) { x = -x; xv = -xv; }
    if (x >= WIDTH) { x = WIDTH - 1 - (x - (WIDTH - 1)); xv = -xv; }
    y += v;
    if (y < 0) {
      // hit the ground
      y = -y;
      v = -v - 1;
      if (v < 10) {
	reset();
      }
    }
    v --;

    int yy = HEIGHT - 1 - (y/10);
    //rect(0, HEIGHT-1-(y/10), WIDTH, HEIGHT-1-(y/10)+1, c);
    point(x, yy, c);
  }
};

#define N_BALLS 20
#define BALL_SEPARATION 10

static Ball balls[N_BALLS];

void setup_animation() {
  for (int i = 0; i < N_BALLS; ++i) {
    balls[i].delay = rand() % 100;
  }
}

void draw_frame(int frame) {
  dim(150);
  for (int i = 0; i < N_BALLS; ++i) {
    //printf("ball %d: height %d v %d delay %d\n", i, balls[i].y, balls[i].v, balls[i].delay);
    balls[i].fall();
  }
}
