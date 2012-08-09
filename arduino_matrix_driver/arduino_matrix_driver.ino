#include <SPI.h>
#include <Adafruit_WS2801.h>

/*
 * Driver for Want It! matrix display
 * (c) 2012 Phillip Pearson
 */

// Duemilanove has an LED on pin 13 
#define LED 13

// comms speed
#define BAUD 1000000
//#define BAUD 115200

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

  for (uint8_t *stop_ptr = strip.pixels + strip.numPixels() * 3,  *pixel = strip.pixels; pixel != stop_ptr; ++pixel) {
    uint8_t pixel_value = *pixel;
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

// byte available to read from the UART?
#define serial_available() (UCSR0A & (1<<RXC0))

// read a byte from the UART
#define serial_read() (UDR0)

// is the UART ready for us to write a byte to it?
#define serial_can_write() (UCSR0A & (1 << UDRE0))

// write a byte to the UART
void serial_write(uint8_t c) {
  while (!serial_can_write());
  UDR0 = c;
}

// write a string to the UART
void serial_print(const char* s) {
  for (; *s; s++) {
    while (!serial_can_write());
    UDR0 = *s;
  }
}

void setup() {
  // set LEDs weakly on in R/G/B pattern.
  strip.begin();
  uint8_t c = random(0, 255);
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    for (uint8_t x = 0; x < WIDTH; ++x) {
      strip.setPixelColor(y * WIDTH + x, (x + y + c) % 24, (x + y + c + 8) % 24, (x + y + c + 16) % 24);
    }
  }
  fast_show();

  // set up UART
  UCSR0A |= 1<<U2X0;
  UCSR0B |= (1<<RXEN0 | 1<<TXEN0);
  uint16_t divider = (F_CPU / 4 / BAUD - 1) / 2;
  UBRR0H = divider >> 8;
  UBRR0L = divider & 0xFF;

  // say hello
  UDR0 = '*';

  // ready!
  serial_print("OK.\n");
}

void loop() {
  //TODO: integrate patterns from test code and clean this up
  static uint8_t header_pos = 0;
  while (header_pos < 4) {
    while (!serial_available());
    if (serial_read() == (header_pos < 2 ? '*' : '+')) {
      ++header_pos;
    }
  }
  serial_print("#");
  static uint16_t current_pixel = 0;
  while (current_pixel < BUFFER_SIZE) {
    while (!serial_available());
    strip.pixels[current_pixel++] = (uint8_t)serial_read();
  }

  // push current pattern to the LEDs
  fast_show();

  // toggle the LED (debugging)
  digitalWrite(LED, !digitalRead(LED));

  // prepare for next round
  header_pos = 0;
  current_pixel = 0;
  serial_print("OK.\n");
}
