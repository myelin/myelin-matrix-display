/*
 * ATTINY85 code to bit-bang WS2801 strands/strips.
 * Copyright (C) 2012 Phillip Pearson
 * You may use this under a CC Attribution license.
 */

// settings for ws2801 driver - local to individual project
#include "ws2801_configuration.h"

// data = attiny85 pin 2 (PB3)
#define WS2801_DATA_PORT PORTB
#define WS2801_DATA_DDR DDRB
#define WS2801_DATA_PIN PB3

// clock = attiny85 pin 3 (PB4)
#define WS2801_CLOCK_PORT PORTB
#define WS2801_CLOCK_DDR DDRB
#define WS2801_CLOCK_PIN PB4

// declarations; don't change these!
#define WS2801_BUFFER_LEN (N_PIXELS * 3)
extern void ws2801_setup();
extern void ws2801_show(const uint8_t* data);
