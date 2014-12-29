#include <Arduino.h>
#include "Adafruit_WS2801.h"

#define CHILL_MODE

// Display dimensions
#define WIDTH 25
#define HEIGHT 12
#define RGB 1
#define VERTICAL_ADDRESSING

#define PIXEL_SIZE 3
#define BUF_SIZE PIXEL_SIZE * WIDTH * HEIGHT

#define CUSTOM_FRAME_RATE_SETTER

extern void serial_write(uint8_t c);
extern void set_frames_per_mode(uint16_t frames_per_mode);
extern void next_mode();
