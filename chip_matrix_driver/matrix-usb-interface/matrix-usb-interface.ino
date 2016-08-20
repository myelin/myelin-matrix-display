/*
 * USB-to-SPI interface to let a C.H.I.P. drive the Myelin Matrix Display
 */

#include <SPI.h>
#include "Adafruit_WS2801.h"

#define LED_PIN 17
#define N_PIXELS 300
#define N_PIXEL_BYTES (N_PIXELS * 3)
Adafruit_WS2801 strip(300);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  strip.begin();
  strip.show();
}

uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

uint32_t random_color() {
  return (uint32_t)random(0, 0xFFFFFFL);
}

void set_all_random() {
  for (int i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, random_color());
  }
  strip.show();
}

void loop() {
  static int packet_ptr = -2;
  static long last_update = -1, last_frame = -1;
  while (Serial.available()) {
//    Serial.write("r");
    int b = Serial.read();
//    Serial.write("x");
    if (b == -1) break;

    switch (packet_ptr) {
    case -2:
      if (b == '*') {
	packet_ptr = -1;
	Serial.write(".");
      }
      break;
    case -1:
      switch (b) {
      case '#':
	packet_ptr = 0;
	Serial.write("-");
	break;
      case '*':
	// stay in -1
	Serial.write("_");
	break;
      default:
	packet_ptr = -2;
      }
      break;
    default:
      strip.pixels[packet_ptr++] = (uint8_t) b;
//      Serial.println(packet_ptr);
      if (packet_ptr >= N_PIXEL_BYTES) {
	strip.show();
	Serial.println("got frame");
	packet_ptr = -2;
  last_frame = millis();
  last_update = -1;
      }
    }
//    Serial.print("_");
  }
  long now = millis();
  if (last_frame != -1 && (now - last_frame > 5000)) {
    last_frame = -1;
  }
  if (last_frame == -1 && (last_update == -1 || (now - last_update > 500))) {
    digitalWrite(LED_PIN, digitalRead(LED_PIN) == HIGH ? LOW : HIGH);
    set_all_random();
    last_update = millis();
  }
}
