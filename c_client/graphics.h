#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint32_t color_t;

// arduino compatibility
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

class ScreenBuffer {
public:
	const int width, height;

	ScreenBuffer(int w, int h);
	~ScreenBuffer();

	int pixel_count() { return width * height; }
	int buf_size() { return width * height * 3; }
	unsigned char* pixels() const { return draw_buf; }

	void blank();
	void dim(int factor);
	void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	void point(int x, int y, color_t c);
	void point_clip(int x, int y, color_t c);
	void line(int x0, int y0, int x1, int y1, color_t c);
	void rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b);
	void rect(int x0, int y0, int x1, int y1, color_t c);
	void circle(int cx, int cy, int r, color_t c);

	void copy(ScreenBuffer *dest);
	void vertical_snake_transform(ScreenBuffer *dest);
	void chunked_vertical_snake_transform(ScreenBuffer *dest);
	void downsample(ScreenBuffer *dest, int oversampling);

protected:
	unsigned char *draw_buf;

	unsigned char* ptr_for_point(int x, int y);
	color_t getpoint(int x, int y);
};

inline color_t color(uint8_t r, uint8_t g, uint8_t b) {
  return (r << 16) | (g << 8) | b;
}
inline uint8_t red_from_color(color_t c) { return (uint8_t)((c >> 16) & 0xFF); }
inline uint8_t green_from_color(color_t c) { return (uint8_t)((c >> 8) & 0xFF); }
inline uint8_t blue_from_color(color_t c) { return (uint8_t)(c & 0xFF); }
#define WHITE color(255, 255, 255)
#define BLACK color(0, 0, 0)

extern int randint(int min_inclusive, int max_inclusive);

extern color_t random_color();
extern color_t color_fade(color_t color1, color_t color2, int pos);
inline color_t color_add(color_t c, double c2) {
  return color(red_from_color(c) + red_from_color(c2),
	       green_from_color(c) + green_from_color(c2),
	       blue_from_color(c) + blue_from_color(c2));
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

// in graphics.cpp
extern ScreenBuffer *graphics_setup();
extern ScreenBuffer *graphics_prep_frame(int frame);

