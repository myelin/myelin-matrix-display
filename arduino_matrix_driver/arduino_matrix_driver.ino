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
// Pins that connect to the WS2801 string
#define WS2801_DATA 2
#define WS2801_CLOCK 3

// LED control
Adafruit_WS2801 strip = Adafruit_WS2801(PIXEL_COUNT, WS2801_DATA, WS2801_CLOCK);

struct Pixel {
  unsigned char r, g, b;
};

struct VideoFrame {
  Pixel pixels[PIXEL_COUNT];
};

// current video frame (being read from UART0)
VideoFrame g_serial_frame_buffer;
// current position in frame
unsigned short g_serial_frame_buffer_pos;

void setup() {
  // serial comms with NSLU2
  Serial.begin(500000);
  // serial port input frame buffer
  g_serial_frame_buffer_pos = 0;
  // parallel comms with coprocessor
  DDRC |= 0x07; // Set PC0-2 as digital out (PC0-2 are low 3 bits for comms with coprocessor)
  DDRD |= 0xfc; // Set PD2-7 as digital out (PD2 to trigger interrupt on coprocessor, and PD3-7 are high 5 bits for comms with coprocessor)
  // debug
  Serial.print("UBRR ");
  Serial.print(UBRR0H, HEX);
  Serial.print(" ");
  Serial.print(UBRR0L, HEX);
  Serial.print("\n");
  // ready!
  Serial.print("OK\n");
  
  // set LEDs weakly on in R/G/B pattern.
}

// write a byte to the coprocessor on PC0-2 + PD3-7, and toggle PD2 (INT0 on coprocessor)
void write_to_coprocessor(unsigned char d) {
  PORTC = PORTC & 0xf8 | (d & 0x07);
  PORTD = ((PORTD ^ 0x04) & 0x07) | (d & 0xf8);
}

void loop() {
  while (Serial.available()) {
    if (g_serial_frame_buffer_pos < BUFFER_SIZE) {
      *((unsigned char *)&g_serial_frame_buffer.pixels + g_serial_frame_buffer_pos) = (unsigned char)Serial.read();
      if (++g_serial_frame_buffer_pos == BUFFER_SIZE) {
        digitalWrite(LED, !digitalRead(LED));
        g_serial_frame_buffer_pos = 0;
        Serial.print("got "); Serial.print(BUFFER_SIZE); Serial.println(" byte frame");
        Serial.print("OK\n");
      }
    }
  }
}
