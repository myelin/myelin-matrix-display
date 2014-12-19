#include "matrix.h"

#define RATE 1
#define FADE_STEPS 30

#define MAX_COLOR 255

class Board {
 public:
  int cells[HEIGHT][WIDTH];
};

Board last, next;
int pos = 0;

void evolve() {
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      int neighbours = 0;
      for (int yy = max(y - 1, 0); yy < min(y + 2, HEIGHT); ++yy)
	for (int xx = max(x - 1, 0); xx < min(x + 2, WIDTH); ++xx)
	  if (!(xx == x && yy == y) && last.cells[yy][xx]) ++neighbours;
      next.cells[y][x] = (last.cells[y][x]) ? (neighbours == 2 || neighbours == 3) : (neighbours == 3);
    }
  }
}

void setup_animation() {
  for (int y = 0; y < HEIGHT; ++y)
    for (int x = 0; x < WIDTH; ++x)
      last.cells[y][x] = randint(0, 1);
  evolve();
}

void draw_frame(ScreenBuffer *s, int frame) {
  set_frame_rate(RATE * FADE_STEPS);
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      int c = last.cells[y][x] * MAX_COLOR * (FADE_STEPS - pos) / FADE_STEPS
	+ next.cells[y][x] * MAX_COLOR * pos / FADE_STEPS;
      s->point(x, y, color(c, c, c));
    }
  }
  if (++pos >= FADE_STEPS) {
    last = next;
    evolve();
    pos = 0;
  }
}
