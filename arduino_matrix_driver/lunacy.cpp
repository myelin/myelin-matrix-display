#include "matrix.h"

#include "lunacy_logo_mask.h"

static color_t lunacy_pix(int x, int y) {
  if (x < 0 || y < 0 || x >= lunacy_logo_mask_width || y >= lunacy_logo_mask_height) return BLACK;

  uint16_t bit = y * lunacy_logo_mask_width + x;
  uint16_t byte = bit / 8, offset = bit % 8;

  //printf("%d,%d -> bit %d / byte %d offset %d (byte = %02x, mask %02x, and = %02x)\n", x, y, bit, byte, offset, lunacy_logo_mask_data[byte], 0x80 >> offset, lunacy_logo_mask_data[byte] & (0x80 >> offset));
  return lunacy_logo_mask_data[byte] & (0x80 >> offset) ? WHITE : BLACK;
}

static void draw_offset_mult(ScreenBuffer *s, int offset, float blend, float mult, color_t mask) {
  for (uint16_t y = 0; y < min(HEIGHT, lunacy_logo_mask_height); ++y) {
    for (uint16_t x = 0; x < min(WIDTH, lunacy_logo_mask_width); ++x) {
      s->point(x, y, color_add(
		      color_mask(color_mult(lunacy_pix(x + offset, y), mult * (1-blend)), mask),
		      color_mask(color_mult(lunacy_pix(x + offset + 1, y), mult * blend), mask)
		      ));
    }
  }
}

/*
static void fade(ScreenBuffer *s, int offset, float blend, int start, int end, int period) {
  color_t mask = random_color();
  for (int pos = 0; pos < period; ++pos) {
    float mult = float(pos)/period;
    draw_offset_mult(s, offset, blend, start + (end - start) * mult, mask);
  }
}
*/

static enum LunacyState {
  SCROLL_FADE,
  RANDOM_JITTER,
  SCROLL_LOGO,
  RANDOM_JITTER_2
} state = SCROLL_LOGO;
static uint8_t frames_in_current_state = 0;

static void next_state() {
  printf("NEW STATE!  leaving state %d\n", state);
  state = (LunacyState)((int)state + 1);
  frames_in_current_state = 0;
}

#define FRAME_RATE 25
void draw_lunacy(ScreenBuffer *s, int frame) {
  set_frame_rate(FRAME_RATE);
  uint8_t max_offset = lunacy_logo_mask_width - WIDTH - 1;
  static color_t current_color;

  switch (state) {
  case RANDOM_JITTER:
  case RANDOM_JITTER_2: {
#define RANDOM_JITTER_FRAMES (FRAME_RATE * 2)
    if (frames_in_current_state < RANDOM_JITTER_FRAMES) {
      draw_offset_mult(s, rand() % max_offset, 0, 1, random_color());
    } else {
      next_state();
    }
    break;
  }
  case SCROLL_LOGO: {
#define SCROLL_LOGO_FRAMES (max_offset / 2)
    if (frames_in_current_state >= SCROLL_LOGO_FRAMES) {
      next_state();
    } else {
      float pos = (float)frames_in_current_state / SCROLL_LOGO_FRAMES * max_offset;
      uint8_t offset = (uint8_t)pos;
      float blend = pos - offset;
      draw_offset_mult(s, offset, blend, 1.0, wheel(frames_in_current_state * WHEEL_MAX / SCROLL_LOGO_FRAMES));
    }
    break;
  }
  case SCROLL_FADE: {
#define FRAMES_PER_SCROLL_FADE (FRAME_RATE / 2)
    uint8_t fade_offset = (frames_in_current_state / FRAMES_PER_SCROLL_FADE) * 5;
    if (fade_offset > max_offset) {
      next_state();
    } else {
      uint8_t fade_pos = frames_in_current_state % FRAMES_PER_SCROLL_FADE;
      if (!fade_pos) current_color = wheel(fade_offset * WHEEL_MAX / max_offset);
      draw_offset_mult(s, fade_offset, 0, 1.0 - ((float)frames_in_current_state / FRAMES_PER_SCROLL_FADE), current_color);
    }
    break;
  }
  default:
    state = SCROLL_FADE;
    break;
  }

  ++ frames_in_current_state;
}
