#ifndef __COMMANDS_H
#define __COMMANDS_H

#include "ws2801.h"

// GRAPHIC + COMMAND TYPES

// mode / command enum

enum mode_t {
  MODE_NONE = 0,
  MODE_FADE = 2,
  MODE_STROBE = 3,
  MODE_CHASE = 4,
  MODE_RAINBOW = 5
};

// primitives

// used by FADE mode
struct fade_info_t {
  color_t start, finish; // colours to fade between
  uint8_t speed; // how many steps to fade each frame.  0 means jump instantly, 1 means take 256 frames for the fade, 2 means 128 frames, etc.
};

struct fade_mode_state_t {
  uint8_t n_fade; // index into payload.fades
  uint8_t fade_pos; // how many steps (0-255) through we are
};

struct fade_mode_payload_t {
  uint8_t n_fades; // # of fades to perform
  fade_info_t fades[4]; // start / finish / speed info for each fade
};

// used by STROBE mode

struct strobe_mode_state_t {
  uint8_t n_color; // which colour we should show
};

struct strobe_mode_payload_t {
  uint8_t n_colors; // # of colours to strobe through (max 9)
  color_t colors[9]; // colours to strobe through
};

// used by CHASE and RAINBOW modes

struct chase_mode_state_t {
  uint8_t pos;
};

#endif

