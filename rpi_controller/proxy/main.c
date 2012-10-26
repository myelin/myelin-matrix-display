#include "mx2_board.h"
#include "rf24.h"

void enable_spi_pins() {
  printf("enable MOSI and SCLK (not others, because we use them for other things)\n");
  //pinMode(7, SPI_ALT); // CE0
  //pinMode(8, SPI_ALT); // CE1
  //pinMode(9, SPI_ALT); // MISO
  pinMode(10, SPI_ALT); // MOSI
  pinMode(11, SPI_ALT); // SCLK
}

// flags so that we don't accidentally kill the rpi by shorting its gpios
static int avr_active = 1, avr_frozen = 0;

static void reset_avr() {
  pinMode(PIN_AVR_RESET, OUTPUT);
  digitalWrite(PIN_AVR_RESET, 0);
  delayMicroseconds(100);

  avr_active = 0;
}

void wake_up_avr() {
  if (!avr_frozen) return;
  digitalWrite(PIN_AVR_SS, 1);
  avr_frozen = 0;
}

void freeze_avr() {
  if (avr_frozen) return;
  digitalWrite(PIN_AVR_SS, 0);
  avr_frozen = 1;
}

// reset the avr, then freeze it once it wakes up
void shut_down_avr() {
  printf("Shutting down AVR\n");
  reset_avr();

  // start it back up, then play with SS
  pinMode(PIN_AVR_SS, OUTPUT);
  digitalWrite(PIN_AVR_RESET, 1);
  freeze_avr();
  delay(100); // let it get through init

  // set up all the other pins
  pinMode(PIN_RF24_CE, OUTPUT);
  pinMode(PIN_RF24_IRQ, INPUT);
  pinMode(PIN_RF24_CSN, OUTPUT);
  pinMode(PIN_RF24_MISO, INPUT);
  pinMode(PIN_RF24_MOSI, OUTPUT);
  pinMode(PIN_RF24_SCK, OUTPUT);
}

void restart_avr() {
  printf("Restarting AVR\n");
  reset_avr();
  delayMicroseconds(100);

  // disable SPI pins
  pinMode(PIN_AVR_SS, INPUT);
  pinMode(PIN_RF24_CSN, INPUT);
  pinMode(PIN_RF24_MISO, INPUT);
  pinMode(PIN_RF24_MOSI, INPUT);
  pinMode(PIN_RF24_SCK, INPUT);

  // disable GPIO pins
  pinMode(PIN_RF24_CE, INPUT);
  pinMode(PIN_RF24_IRQ, INPUT);
  pinMode(PIN_LED_CLK, INPUT);
  pinMode(PIN_LED_DATA, INPUT);

  // bring AVR out of reset
  pinMode(PIN_AVR_RESET, INPUT);

  avr_active = 1;
}

class WS2801 {
 public:
  int _channel;

  WS2801(int channel) {
    _channel = channel;
  }

  void setup() {
    enable_spi_pins();
    if (wiringPiSPISetup(_channel, 1000000) < 0) {
      fprintf(stderr, "SPI Setup failed: %s\n", strerror (errno));
      return;
    }
  }

  static void decode(unsigned char* output, unsigned char* input, int len) {
    if (len != 900) {
      printf("don't know what to do with a %d byte display packet\n", len);
      return;
    }

#define WIDTH 25
#define HEIGHT 12

    for (int y = 0; y < HEIGHT; ++y) {
      for (int x = 0; x < WIDTH; ++x) {
	unsigned char* ptr = input + (y * WIDTH + x) * 3;

	// modular matrix display (sep 2012) - snaking vertically from top left corner, so there's only one connection between each pair of panels, rather than 12.
	int pos = ((x & 1) ? ((x + 1) * HEIGHT - 1 - y) : (x * HEIGHT + y)) * 3;
	unsigned char* dest = output + pos;
	*dest++ = *ptr++;
	*dest++ = *ptr++;
	*dest++ = *ptr++;
      }
    }
  }

  void send(unsigned char* buf, int len) {
    printf("sending frame to leds\n");

    static int last = 0;
    int start = millis();
    unsigned char out[len];
    WS2801::decode(out, buf, len);
    wiringPiSPIDataRW(_channel, out, len);
    int end = millis();
    printf("now - last %d ms; last - start %d ms; start - end %d ms; approx %d kHz and %d fps\n", (int)(end - last), (int)(start - last), (int)(end - start), end == start ? 0 : (int)(900 * 8 / (end - start)), end == last ? 0 : 1000 / (end - last));
    last = end;
  }
};

nRF24L01 rf24(RF24_SPI_CHANNEL, // SPI channel 1 (CSN = P1-26 / CE1)
	      PIN_RF24_CE, // CE on P1-18 (GPIO 24)
	      PIN_RF24_IRQ); // IRQ on P1-22 (GPIO 25)

/*
// to listen to the GettingStarted Arduino test sketch
uint8_t remote_address[5] = {0xE3, 0xF0, 0xF0, 0xF0, 0xF0};
uint8_t local_address[5] = {0xD4, 0xF0, 0xF0, 0xF0, 0xF0};
*/
// to listen to the remote control
uint8_t remote_address[5] = {0xE1, 0xF0, 0xF0, 0xF0, 0xF0};
uint8_t local_address[5] = {0xD2, 0xF0, 0xF0, 0xF0, 0xF0};

WS2801 leds(LED_SPI_CHANNEL);

extern void setup_udp();
extern void close_udp();
extern int check_udp(unsigned char* frame_buf, size_t frame_buf_len);

int main() {
  printf("init wiringPi\n");
  if (wiringPiSetupGpio() < 0) {
    printf("failed to init wiringPi\n");
    return -1;
  }

  shut_down_avr();
  leds.setup();

  // radio
  rf24.setup();
  rf24.set_tx_addr(local_address, 5); // set TX_ADDR and RX_ADDR_P0 (...D2 addr)
  rf24.set_rx_addr(1, remote_address, 5); // set RX_ADDR_P1 (...E1 addr)
  //    rf24.set_retries(15); // retry 15 times
  //    rf24.set_retry_delay(1); // 500 us
  rf24.listen();

  int last_frame_received = millis();
  setup_udp();
  unsigned char buf[900];
  printf("Waiting for UDP frames\n"); fflush(stdout);

  while (1) {
    unsigned char radio_buf[32];
    if (rf24.receive(radio_buf, 32)) {
      printf("received something from the radio: %d\n", (int)radio_buf[0]);
    }

    if (check_udp(buf, 900)) {
      freeze_avr();
      leds.send(buf, 900);
      fflush(stdout);
      last_frame_received = millis();
    } else {
      delay(1);
    }

    if (avr_frozen && millis() - last_frame_received > 1000) {
      printf("waking up avr because the network seems to have gone dead\n");
      wake_up_avr();
    }
  }
  close_udp();

  // and back
  restart_avr();
}
