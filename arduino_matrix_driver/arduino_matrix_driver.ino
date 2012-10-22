/*
 * Driver for Myelin matrix display
 * (c) 2012 Phillip Pearson
 * Ethernet additions by Philip Lindsay
 *
 * Matrix display versions:
 * V1 (nothing defined) - powered by a Freetronics Etherten at 16MHz.  Accepts frames over serial port or Ethernet, or generates an animation internally.
 * V2 (MATRIX_V2 defined) - powered by a Raspberry Pi, connected to a daughterboard with an ATMEGA328, which takes over while the RPi is booting (after which it puts the AVR into reset)
 */

#ifndef MATRIX_V2
// Matrix V2 doesn't have serial or Ethernet interfaces
#define MX_USE_SERIAL
#define MX_USE_ETHERNET
#endif

#include <SPI.h>         // needed for Arduino versions later than 0018
#include "Adafruit_WS2801.h"
#include "matrix.h"
#ifdef MX_USE_ETHERNET
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008

// Our MAC and IP address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 99);

unsigned int localPort = 58082;      // local port to listen on

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
#endif

#ifndef MATRIX_V2
// Arduino boards have an LED on pin 13 (SCK)
#define LED 13
#endif

// comms speed
//#define BAUD 1000000
#define BAUD 115200
// Pins that connect to the WS2801 string (must be on PORTD)
#ifdef MATRIX_V2
#define WS2801_PORT PORTC
#define WS2801_DATA PORTC1
#define WS2801_CLOCK PORTC2
#define WS2801_ARDUINO_DATA A1
#define WS2801_ARDUINO_CLOCK A2
#else
#define WS2801_PORT PORTD
#define WS2801_DATA PORTD2
#define WS2801_CLOCK PORTD4
#define WS2801_ARDUINO_DATA 4
#define WS2801_ARDUINO_CLOCK 6
#endif

// LED control
Adafruit_WS2801_PP strip = Adafruit_WS2801_PP(PIXEL_COUNT, WS2801_ARDUINO_DATA, WS2801_ARDUINO_CLOCK);

// quickly (6.25ms) bit-bang 900 bytes into the ws2801 strip
void fast_show() {
#define clkpinmask (1 << WS2801_CLOCK)
#define datapinmask (1 << WS2801_DATA)

  for (uint8_t *stop_ptr = strip.pixels + strip.numPixels() * 3,  *pixel = strip.pixels; pixel != stop_ptr; ++pixel) {
    uint8_t pixel_value = *pixel;
#define BANG_WS2801_BIT(bit) if (pixel_value & bit) WS2801_PORT |= datapinmask; else WS2801_PORT &= ~datapinmask; WS2801_PORT |= clkpinmask; WS2801_PORT &= ~clkpinmask
    BANG_WS2801_BIT(0x80);
    BANG_WS2801_BIT(0x40);
    BANG_WS2801_BIT(0x20);
    BANG_WS2801_BIT(0x10);
    BANG_WS2801_BIT(0x08);
    BANG_WS2801_BIT(0x04);
    BANG_WS2801_BIT(0x02);
    BANG_WS2801_BIT(0x01);
  }

  WS2801_PORT &= ~datapinmask;
  delay(1);

#undef datapinmask
#undef clkpinmask
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
#ifdef MX_USE_ETHERNET
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
#endif

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

long last_serial_frame = 0, last_ethernet_frame = 0, last_frame = 0;
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

#ifdef MX_USE_SERIAL
  // handle serial input first, because we need to busy-wait on that
  if (serial_available()) {
    // at this point we block while reading the data from the host, because otherwise we'll miss characters.
    //TODO: set a watchdog timer to reset if we block for too long (200 ms?)
    for (uint8_t header_pos = 0; header_pos < 4; ++header_pos) {
      while (!serial_available());
      if (serial_read() != (header_pos < 2 ? '*' : '+')) return; // anything other than the preamble will kick us back out so we can pick up udp packets again

      // let the host know we received the preamble
      serial_print("#");

      // read pixel data
      for (uint16_t current_pixel = 0; current_pixel < BUF_SIZE; ++current_pixel) {
        while (!serial_available());
        strip.pixels[current_pixel] = (uint8_t)serial_read();
      }

      // push current pattern to the LEDs
      fast_show();

#ifdef LED
      // toggle the LED (debugging)
      digitalWrite(LED, !digitalRead(LED));
#endif

      // let the host know we're ready again
      serial_print("OK.\n");
      last_serial_frame = now; // don't listen on ethernet for 500 ms or so
    }
  }
#endif // MX_USE_SERIAL

#ifdef MX_USE_ETHERNET
#define ETH_WIDTH 25
#define ETH_HEIGHT 12
#define ETH_BUF_SIZE (ETH_WIDTH * ETH_HEIGHT * PIXEL_SIZE)
  if ((now - last_serial_frame) > 500) {
    // if there's data available on the ethernet interface, read a packet
    int packetSize = Udp.parsePacket();
    uint8_t valid_packet = 0;
    if (packetSize == ETH_WIDTH * ETH_HEIGHT * PIXEL_SIZE + 1) {
      valid_packet = 1;
      // skip first byte
      Udp.read();
    } else if (packetSize == ETH_WIDTH * ETH_HEIGHT * PIXEL_SIZE) {
      valid_packet = 1;
    }
    if (valid_packet) {
      serial_print("udp\n");
#ifdef HORIZONTAL_ADDRESSING
      for (int row = 0; row < 12; row+=2) {
        // read one row forward
        Udp.read(strip.pixels+(row*75), 75);
        // and one row backward
        for (int c=0; c<25; c++) {
          Udp.read(strip.pixels+((row+1)*75) + ((24-c) * 3), 3);
        }
      }
#elif defined VERTICAL_ADDRESSING
      // read a row at a time
      uint8_t packet_buffer[ETH_WIDTH * PIXEL_SIZE];
      for (int y = 0; y < HEIGHT; ++y) {
        Udp.read(packet_buffer, ETH_WIDTH * PIXEL_SIZE);
        uint8_t *in = packet_buffer;
        for (int x = 0; x < WIDTH; ++x) {
          point(x, y, *in++, *in++, *in++);
        }
      }
#endif

      // update LEDs
      fast_show();
      last_ethernet_frame = now;
    }
  }
#endif

#define FRAMES_PER_MODE 300

//#define CHILL
//#define INSANE
//#define WANT_IT_2012
#define JUST_BOUNCE

  if ((now - last_ethernet_frame) > 500 && (now - last_frame) > ms_per_frame) {
    uint8_t handled;
    do {
      handled = 1;
      switch (current_mode) {
#if defined(WANT_IT_2012)
        case 0: draw_test(current_frame); break;
        case 1: draw_lines(current_frame); break;
#endif
#if defined(INSANE)
        case 2: draw_epilepsy(current_frame); break;
//        case 3: draw_insane_lines(current_frame); break;
#endif
#if defined(WANT_IT_2012) || defined(JUST_BOUNCE)
        case 4: draw_bounce(current_frame); break;
#endif
#if defined(CHILL)
        case 5: draw_rainbow(current_frame); break;
#endif
        default:
          handled = 0;
          if (++current_mode > 10) current_mode = 0;
          break;
      }
    } while (!handled);
    fast_show();
    if (++current_frame >= FRAMES_PER_MODE) {
      ++current_mode;
      current_frame = 0;
    }
    last_frame = now;
  }

}
