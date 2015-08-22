/**
 * Some effects for WS2801 strands/strips
 * Copyright (C) 2012 Phillip Pearson
 * You may use this code under a CC Attribution license.
 *
 * ... except for Color() and Wheel(), which are from the Adafruit_WS2081 library's "strandtest" example:
 * https://github.com/adafruit/Adafruit-WS2801-Library
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 * BSD license, all text above must be included in any redistribution
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "ws2801.h"

extern uint8_t* pixel_data;

static void show() {
  led_show(pixel_data);
}

#define delay _delay_ms

#define randbyte() ((uint8_t)random())

static long random(long a, long b) {
  return (random() % (b - a)) + a;
}

static void point(uint16_t p, uint8_t r, uint8_t g, uint8_t b) {
  pixel_data[p * 3] = r;
  pixel_data[p * 3 + 1] = g;
  pixel_data[p * 3 + 2] = b;
}

static void point(uint16_t p, uint32_t c) {
  pixel_data[p * 3] = (uint8_t)(c >> 16);
  pixel_data[p * 3 + 1] = (uint8_t)(c >> 8);
  pixel_data[p * 3 + 2] = (uint8_t)c;
}

static void point(unsigned char x, unsigned char y, uint32_t c) {
  if (y & 1) {
    x = WIDTH - x; // odd numbered line - need to flip x
  }
  point((int)y * WIDTH + (int)x, c);
}

static void rect(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, uint32_t c) {
//  Serial.print("rect color "); Serial.print(c, HEX); Serial.println("");
  for (unsigned char y = y0; y < y1; ++y) {
    for (unsigned char x = x0; x < x1; ++x) {
      point(x, y, c);
    }
  }
}

static void blank() {
  for (int i = 0; i < N_PIXELS; ++i) point(i, 0);
}

static void dim(int factor) {
  for (int i = 0; i < N_PIXELS * 3; ++i) pixel_data[i] = pixel_data[i] * factor / 256;
}

static void flip() {
  show();
  blank();
}
// Create a 24 bit color value from R,G,B
uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(uint8_t WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 0, 255 - WheelPos * 3);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, WheelPos * 3, 0);
  } else {
   WheelPos -= 170;
   return Color(0, 255 - WheelPos * 3, WheelPos * 3);
  }
}

static void set_color(int r, int g, int b) {
  for (int i = 0; i < N_PIXELS; ++i) point(i, r, g, b);
}

static int random_color() {
  return Color(randbyte(), randbyte(), randbyte());
}

static void sweep(int wait) {
  for (char x = 0; x < WIDTH; ++x) {
    blank();
    rect(x, 0, x+1, HEIGHT, random_color());
    show();
    delay(wait);
  }
}

static void lines(int wait) {
  char x = 24;
  char v = 0;
  for (char tmp = 0; tmp < 100; ++tmp) {
    rect(x, 0, WIDTH /*min(x+3, WIDTH)*/, HEIGHT, 0xff0000/*random_color()*/);
    //Serial.print((int)strip.pixels[30], HEX); Serial.print(" "); Serial.print((int)strip.pixels[31], HEX); Serial.print(" "); Serial.print((int)strip.pixels[32], HEX); Serial.println(" <- pixel 0");
    // 1-128 = blue
    // 0x0100 = green
    // 0x4000 = bright green
    // 0x8000 = dark yellow
    // 0xf000 = yellow
    // 0xff00 = yellow
    // 0x100000 = nothing
    x += v;
    if (x < 0) {
      x = -x;
      v = -v * 9 / 10;
    }
    v -= 1;
    flip();
    delay(wait);
  }
}

static void stripes(int wait) {
  for (char x0 = 0; x0 < WIDTH; ++x0) {
    for (char x = 0; x < WIDTH; x += 5) {
      int c = random_color();
      for (char y = 0; y < HEIGHT; ++y) {
        point((x0+x+y) % WIDTH, y, c);
      }
    }
    flip();
    delay(wait*5);
  }
}

static void horizontal_rainbow(int wait) {
  for (int pos = 0; pos < 256; ++pos) {
    for (int x = 0; x < WIDTH; ++x) {
      uint32_t c = Wheel((pos + x) % 256);
      rect(x, 0, x+1, HEIGHT, c);
    }
    flip();
    delay(wait);
  }
}

static void flashes() {
  for (int tmp = 0; tmp < 100; ++tmp) {
    for (int c = 0; c < 0; ++c) {
      point(random(0, WIDTH), random(0, HEIGHT), Color(255, 255, 255));
    }
    if (random(0, 10) == 0) rect(0, 0, WIDTH, HEIGHT, random_color());
    flip();
    delay(20);
  }
}

static void bright_flash() {
  for (uint8_t i = 0; i < 100; ++i) {
    rect(0, 0, WIDTH, HEIGHT, random_color());
    delay(50);
    flip();
  }
}

static void rectangles(int wait) {
  // rectangles
  for (uint8_t tmp = 0; tmp < 100; ++tmp) {
    blank();
    char x0 = random(0, WIDTH), x1 = random(0, WIDTH), y0 = random(0, HEIGHT), y1 = random(0, HEIGHT);
    if (x0 > x1) { char _ = x0; x0 = x1; x1 = _; }
    if (y0 > y1) { char _ = y0; y0 = y1; y1 = _; }
    rect(x0, y0, x1, y1, random_color());
    show();
    delay(wait);
  }
}

static void bounce() {
  // bounce
  for (uint8_t tmp = 0; tmp < 100; ++tmp) {
    for (char y = 0; y < 12; ++y) {
      blank();
      for (char x = 0; x < 25; ++x) {
        point(y * 25 + x, random_color());
      }
      show();
      delay(50);
    }
    for (char x = 0; x < 25; ++x) {
      blank();
      for (char y = 0; y < 12; ++y) {
        point(y * 25 + x, random_color());
      }
      show();
      delay(24);
    }
  }
}

static void chase() {
  for (int pos = 0; pos < N_PIXELS; ++pos) {
    point((pos+N_PIXELS-1)%N_PIXELS, 0, 0, 0);
    point(pos, 255, 255, 255);
    show();
    delay(20);
  }
  for (int pos = 0; pos < N_PIXELS; ++pos) {
    point((pos+N_PIXELS-1)%N_PIXELS, 0, 0, 0);
    point(pos, randbyte(), randbyte(), randbyte());
    show();
    delay(20);
  }
}

static void trailchase() {
  uint8_t r = randbyte(), g = randbyte(), b = randbyte();
  for (int pos = 0; pos < N_PIXELS; ++pos) {
    dim(200);
    point(pos, r, g, b);
    for (int gp = 0; gp < N_PIXELS; ++gp) {
      if (!(rand() % 60)) point(gp, rand() % 32, rand() % 32, rand() % 32);
    }
    show();
    delay(60);
  }
}

static void paparazzi() {
	for (int frame = 0; frame < N_PIXELS * 2; ++frame) {
		point(randbyte() % N_PIXELS, randbyte(), randbyte(), randbyte());
		flip();
		delay(20);
	}
}

void run_effects() {
  trailchase();
  //paparazzi();
  //chase();
  //flashes();
  return;

  for (int p = 0; p < N_PIXELS; ++p) {
    point(p, randbyte(), randbyte(), randbyte());
  }
  show();
  return;

  int tmp;
//  for (tmp = 0; tmp < 100; ++tmp) { set_color(255, 255, 0); show(); delay(10); } return;
//  for (tmp = 0; tmp < 100; ++tmp) { set_color(255, 255, 255); show(); delay(10); } return;
  // Some example procedures showing how to display to the pixels
  int wait = 10;
//  bright_flash(); return;
//  sweep(wait); return;
  flashes(); return;
//  horizontal_rainbow(wait); return;
//  stripes(wait); return;
//  lines(wait); return;
//  rectangles(wait); return;
/*  // static
  for (tmp = 0; tmp < 200; ++tmp) {
    //unsigned char grey = 1 << random(0, 8);
    //unsigned char grey = randbyte();
    //Color(grey, grey, grey)
    int c = random_color();
    for (char x = 0; x < N_PIXELS; ++x) {
      point(x, c);
    }
    show();
  } */
//  bounce(); return;
//  rainbowCycle(wait); return;
/*
  // slow colour fade
  for (tmp = 0; tmp < 64*3; ++tmp) {
    for (int pos = 0; pos < N_PIXELS; ++pos) {
      point(pos,
        (tmp < 64 ? tmp : (tmp < 128 ? 128 - tmp : 0)),
        (tmp > 64 ? (tmp < 128 ? tmp - 64 : 192 - tmp) : 0),
        (tmp < 64 ? 64 - tmp : (tmp > 128 ? tmp - 128 : 0))
       );
    }
    show();
    delay(wait);
  }
  return;
*/
  chase(); return;
/*  for (tmp = 0; tmp < 1; ++tmp) {
    for (int pos = 0; pos < N_PIXELS; ++pos) {
      blank();
      point(pos, random_color());
      show();
      delay(wait);
    }
  }*/
  for (tmp = 0; tmp < 100; ++tmp) {
    blank();
//    set_color(255, 255, 255); // FULL WHITE
    for (int x = 0; x < 30; ++x) point(random(0, N_PIXELS), Color(255, 255, 255));
    show();
    delay(wait);
  }
  for (tmp = 0; tmp < 100; ++tmp) {
    blank();
    for (int x = 0; x < 30; ++x) point(random(0, N_PIXELS), random_color());
    show();
    delay(wait);
  }
  return;

    // random blink
  /*  if (pos < 25) random_color = 0;
    for (i = 0; i < N_PIXELS; ++i) point(i, random_color);
    show();
    return;*/



  //  for (int i = 0; i < 256; ++i) {
      //int random_color = Color(255, 255, 255);
      //int random_color = Color(0, 0, randbyte()); //, randbyte(), randbyte());
      //int brightness = 255; int random_color = Color(brightness, brightness, brightness);
      //point(random(0, N_PIXELS), random_color);
  //  }
    for (int x = 0; x < 10; ++x) point(random(0, N_PIXELS), random_color());
    //point(random(0, N_PIXELS), Color(255, 255, 255));
    //point(pos, random_color);
    show();
  //  delay(50);
  //}

/*  colorWipe(Color(64, 0, 0), 50);
  colorWipe(Color(0, 64, 0), 50);
  colorWipe(Color(0, 0, 64), 50);*/
//  rainbow(20);
//  rainbowCycle(20);
}

static void rainbow(uint8_t wait) {
  int i, j;

  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < N_PIXELS; i++) {
      point(i, Wheel( (i + j) % 255));
    }
    show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
static void rainbowCycle(uint8_t wait) {
  int i, j;

  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < N_PIXELS; i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / N_PIXELS part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      point(i, Wheel( ((i * 256 / N_PIXELS) + j) % 256) );
    }
    show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
static void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < N_PIXELS; i++) {
      point(i, c);
      show();
      delay(wait);
  }
}
