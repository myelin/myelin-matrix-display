#include <SPI.h>
#include <Adafruit_WS2801.h>

/*
 * Driver for Want It! matrix display
 * (c) 2012 Phillip Pearson
 */

// Duemilanove has an LED on pin 13 
#define LED 13

// Display dimensions
#define WIDTH 25
#define HEIGHT 12
#define PIXEL_COUNT (WIDTH*HEIGHT)
// Bytes per pixel
#define PIXEL_SIZE 3
// Bytes per frame buffer
#define BUFFER_SIZE (PIXEL_COUNT * PIXEL_SIZE)
// Pins that connect to the WS2801 string (must be on PORTD)
#define WS2801_DATA PORTD2
#define WS2801_CLOCK PORTD4

// LED control
Adafruit_WS2801 strip = Adafruit_WS2801(PIXEL_COUNT, WS2801_DATA, WS2801_CLOCK);

// quickly (6.25ms) bit-bang 900 bytes into the ws2801 strip
void fast_show() {
/*
  uint8_t clkpinmask  = digitalPinToBitMask(clockPin);
  uint8_t datapinmask = digitalPinToBitMask(dataPin);
  uint8_t resetmask = ~(clkpinmask | datapinmask);
*/
#define clkpinmask 0x10
#define datapinmask 0x04
//#define resetmask 0xeb

  for (uint8_t *stop_ptr = strip.pixels + strip.numPixels() * 3,  *pixel = strip.pixels; pixel != stop_ptr; ++pixel) {
    uint8_t pixel_value = *pixel;
//#define BANG_WS2801_BIT(bit) PORTD = (PORTD & resetmask) | (pixel_value & bit ? datapinmask : 0); PORTD |= clkpinmask // this results in some odd flickering - too fast?
#define BANG_WS2801_BIT(bit) if (pixel_value & bit) PORTD |= datapinmask; else PORTD &= ~datapinmask; PORTD |= clkpinmask; PORTD &= ~clkpinmask
    BANG_WS2801_BIT(0x80);
    BANG_WS2801_BIT(0x40);
    BANG_WS2801_BIT(0x20);
    BANG_WS2801_BIT(0x10);
    BANG_WS2801_BIT(0x08);
    BANG_WS2801_BIT(0x04);
    BANG_WS2801_BIT(0x02);
    BANG_WS2801_BIT(0x01);
  }

  delay(1);
}

void setup() {
  // serial comms with host laptop
//  Serial.begin(500000);
  Serial.begin(115200);
  // ready!
  Serial.print("OK.\n");

  // set LEDs weakly on in R/G/B pattern.
  strip.begin();
  uint8_t c = random(0, 255);
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    for (uint8_t x = 0; x < WIDTH; ++x) {
      strip.setPixelColor(y * WIDTH + x, (x + y + c) % 24, (x + y + c + 8) % 24, (x + y + c + 16) % 24);
    }
  }
  fast_show();
}

void loop() {
  static uint8_t header_pos = 0;
  static uint16_t current_pixel = 0;
  while (Serial.available()) {
    // are we overflowing the buffer on the arduino?  send something when we start the loop / first time we get something on serial after a quiet spell?
    if (header_pos < 4) {
      if (Serial.read() == (header_pos < 2 ? '*' : '+')) ++header_pos;
      Serial.println(header_pos, DEC);
      if (header_pos == 4) {
        current_pixel = 0;
      }
    } else if (current_pixel < BUFFER_SIZE) {
      while (Serial.available() && current_pixel < BUFFER_SIZE) {
        strip.pixels[current_pixel++] = (uint8_t)Serial.read();
//        if (!(current_pixel % 100)) { Serial.println(current_pixel, DEC); }
      }
    
      if (current_pixel == BUFFER_SIZE) {
        Serial.println("frame!");
        fast_show();
        digitalWrite(LED, !digitalRead(LED));
        header_pos = 0;
        current_pixel = 0;
        Serial.print("\ngot "); Serial.print(BUFFER_SIZE); Serial.println(" byte frame");
        Serial.print("OK.\n");
      }
    }
  }
}
