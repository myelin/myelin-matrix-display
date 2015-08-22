/*
 * ATTINY85 LED belt code
 * Copyright (C) 2012-2015 Phillip Pearson
 * You may use this under a CC Attribution license.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "ws2801.h"

uint8_t pixel_data[WS2801_BUFFER_LEN];

void setup() {
  ws2801_setup();
  // for some reason the program hangs if we don't start with this
  memset(pixel_data, 127, WS2801_BUFFER_LEN);
  led_show(pixel_data);
}

extern void run_effects();

void loop() {
  run_effects();
}

int main() {
  setup();
  while (1) loop();
}
