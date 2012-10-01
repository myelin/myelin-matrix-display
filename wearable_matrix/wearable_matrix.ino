/*
 * Driver for wearable mini-matrix
 * (c) 2012 Phillip Pearson
 */

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <LPD8806.h>
#define LED_DRIVER_INCLUDED
#include "matrix.h"

// Duemilanove has an LED on pin 13 
#define LED 13

// comms speed
//#define BAUD 1000000
#define BAUD 115200

// LED control
LPD8806 strip(PIXEL_COUNT);

void setup() {
  Serial.begin(BAUD);

  // cls
  strip.begin();
  blank();
  strip.show();
}

long last_frame = 0;
int current_frame = 0;
uint8_t ms_per_frame = 30;

void set_frame_rate(uint8_t frame_rate) {
  ms_per_frame = 1000 / frame_rate;
}

int current_mode = 0;
extern void draw_test(int frame);
extern void draw_bounce(int frame);
extern void draw_lines(int frame);
extern void draw_insane_lines(int frame);
extern void draw_epilepsy(int frame);
extern void draw_rainbow(int frame);

void loop() {
  unsigned long now = millis();

#define FRAMES_PER_MODE 300

  if ((now - last_frame) > ms_per_frame) {
    uint8_t handled;
    do {
      handled = 1;
      switch (current_mode) {
//        case 0: draw_test(current_frame); break;
        case 1: draw_lines(current_frame); break;
//        case 2: draw_epilepsy(current_frame); break;
//        case 3: draw_insane_lines(current_frame); break;
        case 4: draw_bounce(current_frame); break;
//        case 5: draw_rainbow(current_frame); break;
        default:
          handled = 0;
          if (++current_mode > 10) current_mode = 0;
          break;
      }
    } while (!handled);
    strip.show();
    if (++current_frame >= FRAMES_PER_MODE) {
      ++current_mode;
      current_frame = 0;
    }
    last_frame = now;
  }

}
