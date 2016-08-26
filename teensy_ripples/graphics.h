// implemented by the particular animation we're compiling
extern void setup_animation();
extern void draw_frame(int frame);

// in graphics.cpp
extern void graphics_setup();
extern unsigned char *draw_buf;
extern unsigned char *main_draw_buf;
extern void graphics_prep_frame(int frame);
