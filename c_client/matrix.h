#ifndef __MATRIX_H
#define __MATRIX_H

#include "graphics.h"

#define RGB 1
#define WIDTH 25
#define HEIGHT 12

extern int frame_rate;
extern void set_frame_rate(int rate);

// implemented by the particular animation we're compiling
extern void setup_animation();
extern void draw_frame(ScreenBuffer *screen, int frame);

#endif // __MATRIX_H
