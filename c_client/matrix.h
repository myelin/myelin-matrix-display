#ifndef __MATRIX_H
#define __MATRIX_H

#include "project_config.h"
#include "graphics.h"

#ifndef CUSTOM_FRAME_RATE_SETTER
extern int frame_rate;
#endif

extern void set_frame_rate(int rate);

extern ScreenBuffer *graphics_setup(uint8_t* pixels = NULL);
extern ScreenBuffer *graphics_get_buffer();
extern ScreenBuffer *graphics_prep_frame(int frame);

// implemented by the particular animation we're compiling
extern void setup_animation();
extern void draw_frame(ScreenBuffer *screen, int frame);

#endif // __MATRIX_H
