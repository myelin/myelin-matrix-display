#include <FastLED.h>
#include "brittany_leds.h"
#include "matrix.h"
#include "graphics.h"


#define DATA_PIN 11
#define CLOCK_PIN 13
#define NUM_LEDS    300
#define BRIGHTNESS  255
#define LED_TYPE    WS2801
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

color_t random_color() {
//  CRGB c = CHSV(random8(), 255, random8(128, 255));
//  return color(c.r, c.g, c.b);
return color(random8(), random8(), random8());
}

void send_to_display(int frame, unsigned char* buffer, int buffer_len) {
  int expected_len = OUT_BUF_SIZE;
  if (buffer_len != expected_len) {
    // do something noisy and horrible so we know we have a bug
    for (int i = 0; i < NUM_LEDS; ++i) leds[i] = rand();
    FastLED.show();
    return;
  }

  uint8_t hbt = frame % 25;
  uint8_t heartbeat = (hbt < 2 || hbt == 4 || hbt == 5);
  CRGB heart_c = CHSV(
    //0 // white
//    rand() & 0xff // crazy colour
    0xbd // pink
  ,
    0 // white
    //255 // colour
  ,
    sin8((uint8_t)frame * 5) // heartbeat
  );
//  heart_c = CRGB(255, 255, 255); // all white heart
//  for (int i = 0; i < NUM_LEDS; ++i) leds[i] = c;

  // buffer is row-based and contains entire image
  for (int led = 0; led < 300; ++led) leds[led] = CRGB(buffer[led*3], buffer[led*3+1], buffer[led*3+2]);
//  int led = 0;
//  for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
//    for (int x = 0; x < DISPLAY_WIDTH; ++x) {
//      unsigned char* ptr = buffer + (x + y * OUT_W) * 3;
//      leds[led++] = CRGB(ptr[0], ptr[1], ptr[2]);
//    }
//  }

  FastLED.show();
}

void display_main() {
  graphics_setup();
  setup_animation();
  uint32_t last_t, start_t, now_t;
  last_t = micros();
  for (int frame = 0; ; ++frame) {
    start_t = micros();
    graphics_prep_frame(frame);

    now_t = micros();
    long us_elapsed = now_t - last_t;
    long us_drawing = now_t - start_t;
    long desired_delay = 1000000 / frame_rate;
//    Serial.printf("frame took %ld us (%ld to draw), c.f. max frame time %ld, so we should delay %ld us\n", us_elapsed, us_drawing, desired_delay, desired_delay - us_elapsed);
    if (desired_delay > us_elapsed) {
      long delay_us = desired_delay - us_elapsed;
//      Serial.printf("[delay %d us]\n", delay_us);
      delayMicroseconds(delay_us);
    }
    last_t = micros();

    // now that we're hopefully in sync, update the display
    send_to_display(frame, draw_buf, OUT_BUF_SIZE);
  }
}

void setup() {
  delay( 500 ); // power-up safety delay
  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  Serial.begin(9600); // usb-serial
}

void loop()
{
  display_main();
}
