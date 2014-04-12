/*
 * ATTINY85 code to play random effects - for LED testing.
 * Copyright (C) 2012 Phillip Pearson
 * You may use this under a CC Attribution license.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "ws2801.h"

uint8_t pixel_data[WS2801_BUFFER_LEN];

void setup() {
  ws2801_setup();
  memset(pixel_data, 127, WS2801_BUFFER_LEN);
  led_show(pixel_data);
  while (1); //DEBUG
}

extern void run_effects();

void loop() {
  run_effects();
}

int main() {
  setup();
  while (1) loop();
}
