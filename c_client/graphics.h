#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint32_t color_t;

#ifndef min
// arduino compatibility
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

class Animation;
class ScreenBuffer;

class Animation {
public:
	Animation() {}
	void draw(ScreenBuffer *screen, int frame); // abstract
};

#define DECLARE_DISPLAY_CLASS(class_name) \
	static class_name mode; \
	void setup_animation() {} \
	void draw_frame(ScreenBuffer *screen, int frame) { mode.draw(screen, frame); }

class ScreenBuffer {
public:
	const int width, height;

	ScreenBuffer(int w, int h, uint8_t* pixels = NULL);
	~ScreenBuffer();

	int pixel_count() { return width * height; }
	int buf_size() { return width * height * 3; }
	unsigned char* pixels() const { return draw_buf; }

	void blank();
	void dim(int factor);
	void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	void point(int x, int y, color_t c);
	void add(int x, int y, color_t c);
	void add_subpix(int x, int y, color_t c, double subpix);
	void point_clip(int x, int y, color_t c);
	void point_clip(int x, int y, color_t c, int subpix);
	void corner_points(color_t c);
	void line(int x0, int y0, int x1, int y1, color_t c);
	void rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b);
	void rect(int x0, int y0, int x1, int y1, color_t c);
	void circle(int cx, int cy, int r, color_t c);
	// plots text and returns the x coordinate of the end of the last char in the string
	int text(int x, int y, color_t c, const char* s, double subpix = 0);

	void copy(ScreenBuffer *dest);
	void vertical_snake_transform(ScreenBuffer *dest);
	void chunked_vertical_snake_transform(ScreenBuffer *dest);
	void downsample(ScreenBuffer *dest, int oversampling);

protected:
	unsigned char *draw_buf;
	bool should_free_draw_buf;

	unsigned char* ptr_for_point(int x, int y);
	color_t getpoint(int x, int y);
};

inline color_t color(uint8_t r, uint8_t g, uint8_t b) {
  return (r << 16) | (g << 8) | b;
}
inline uint8_t clipping_add(uint8_t a, uint8_t b) {
	int z = a + b;
	if (z > 255) return 255;
	return (uint8_t) z;
}
inline uint8_t red_from_color(color_t c) { return (uint8_t)((c >> 16) & 0xFF); }
inline uint8_t green_from_color(color_t c) { return (uint8_t)((c >> 8) & 0xFF); }
inline uint8_t blue_from_color(color_t c) { return (uint8_t)(c & 0xFF); }
#define WHITE color(255, 255, 255)
#define BLACK color(0, 0, 0)

extern int randint(int min_inclusive, int max_inclusive);

extern color_t random_color();
extern color_t random_greyscale();
extern color_t color_fade(color_t color1, color_t color2, int pos);
inline color_t color_add(color_t c, color_t c2) {
  return color(clipping_add(red_from_color(c), red_from_color(c2)),
	       clipping_add(green_from_color(c), green_from_color(c2)),
	       clipping_add(blue_from_color(c), blue_from_color(c2)));
}
inline color_t color_mult(color_t c, double factor) {
  return color((uint8_t)((double)red_from_color(c) * factor),
	       (uint8_t)((double)green_from_color(c) * factor),
	       (uint8_t)((double)blue_from_color(c) * factor));
}
inline color_t color_mask(color_t c, color_t mask) {
  return color(red_from_color(c) * red_from_color(mask) / 256,
	       green_from_color(c) * green_from_color(mask) / 256,
	       blue_from_color(c) * blue_from_color(mask) / 256);
}
#define WHEEL_MAX 768
extern color_t wheel(uint16_t pos);

#endif // __GRAPHICS_H
