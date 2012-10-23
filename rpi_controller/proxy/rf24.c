#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "mx2_board.h"

#define DEBUG 1

#define CONFIG 0
#define PWR_UP 2
#define PRIM_RX 1

#define EN_AA 1
#define EN_RXADDR 2
#define SETUP_AW 3
#define SETUP_RETR 4
#define RF_CH 5

#define RF_SETUP 6
#define CONT_WAVE 0x80
#define RF_DR_LOW 0x20
#define PLL_LOCK 0x10
#define RF_DR_HIGH 0x08
#define RF_PWR_H 0x04
#define RF_PWR_L 0x02

#define STATUS 7
#define OBSERVE_TX 8
#define RPD 9
#define RX_ADDR_P0 0xA
#define RX_ADDR_P1 0xB
#define RX_ADDR_P2 0xC
#define RX_ADDR_P3 0xD
#define RX_ADDR_P4 0xE
#define RX_ADDR_P5 0xF
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17
//ACK_PLD
//TX_PLD
//RX_PLD
#define DYNPD 0x1C
#define FEATURE 0x1D

typedef unsigned char uint8_t;
typedef unsigned long long uint64_t;

void enable_spi_pins() {
  printf("enable SPI pins\n");
  for (int pin = 7; pin <= 11; ++pin) {
    pinMode(pin, SPI_ALT);
  }
}

class nRF24L01 {
 public:
  int _channel;
  int _pin_ce;
  int _pin_irq;

  nRF24L01(int channel, int pin_ce, int pin_irq) {
    _channel = channel;
    _pin_ce = pin_ce;
    _pin_irq = pin_irq;
  }

  void setup() {
    printf("setup CE\n");
    pinMode(_pin_ce, OUTPUT);
    printf("write CE low\n");
    digitalWrite(_pin_ce, LOW); // start in standby mode
    printf("setup IRQ\n");
    pinMode(_pin_irq, INPUT);

    enable_spi_pins();
    printf("setup SPI\n");
    if (wiringPiSPISetup(_channel, 1000000) < 0) {
      fprintf(stderr, "SPI Setup failed: %s\n", strerror (errno));
      return;
    }

    printf("configure radio\n");
    write_reg(RF_SETUP, (read_reg(RF_SETUP) & ~(RF_DR_HIGH | RF_DR_LOW)) | RF_PWR_H | RF_PWR_L); // set data rate 1MHz, output power 0dBm
    write_reg(RF_CH, 76); // set channel 76 for compatibility with AVR + RF24
    //? set auto-retry delay ARD (SETUP_RETR)
    //? set RX_ADDR_P0
    //? set TX_ADDR - addr of other station

    printf("power up radio\n");
    write_reg(CONFIG, read_reg(CONFIG) | PWR_UP);
    digitalWrite(_pin_ce, HIGH);
    delayMicroseconds(4500); // abs max Tpd2stby is 4.5 ms
  }

  void receive() {
    // enable reception
    write_reg(CONFIG, read_reg(CONFIG) | PRIM_RX);
    delayMicroseconds(130); // RX settling
  }

  void transmit() {
    // disable reception
    write_reg(CONFIG, read_reg(CONFIG) & ~PRIM_RX);

    // put something in TX fifo
    //todo

    // allow transmission
    delayMicroseconds(130); // TX settling

    // check up on transmission???
    //? OBSERVE_TX reg? ARC_CNT PLUS_CNT
  }

  void set_retry_delay(int delay) {
    printf("setting retry delay to %d: ", delay);
    write_reg(SETUP_RETR, (read_reg(SETUP_RETR) & 0x0f) | ((delay & 0xf) << 4));
  }

  void set_retries(int retries) {
    printf("setting retry count to %d: ", retries);
    write_reg(SETUP_RETR, (read_reg(SETUP_RETR) & 0xf0) | (retries & 0xf));
  }

  int write_tx_addr(uint8_t* addr, int len) { return write_reg(TX_ADDR, addr, len); }
  int write_rx_addr(int pipe, uint8_t* addr, int len) {
    assert(pipe >= 0 && pipe <= 5);
    return write_reg(RX_ADDR_P0 + pipe, addr, len);
  }

  int write_reg(int reg, uint8_t d) {
    return exec_command_with_buffer(0x20 | (reg & 0x1f), &d, 1);
  }

  int write_reg(int reg, uint8_t* data, int len) {
    return exec_command_with_buffer(0x20 | (reg & 0x1f), data, len);
  }

  // return the first byte of a (presumably single-byte) register
  int read_reg(int reg) {
    int reg_size = (reg == 0x0a || reg == 0x0b || reg == 0x10) ? 5 : 1;
    uint8_t data[reg_size];
    exec_read_with_buffer(reg & 0x1f, data, reg_size);
    return data[0];
  }

  /*  uint64_t read_long_reg(int reg, uint8_t* data, int len) {
    int reg_size = (reg == 0x0a || reg == 0x0b || reg == 0x10) ? 5 : 1;
    if (len != reg_size) {
      printf("attempt to read register %02x with buffer length %d; should be %d\n", reg, len, reg_size);
      exit(1);
    }
    exec_read_with_buffer(reg & 0x1f, data, len);
    uint64_t r;
    for (size_t i = 0; i < sizeof(data); ++i) {
      r <<= 8;
      r |= data[i];
    }
    return r;
    }*/

  int status() { return exec_command(0xff /* nop */); }
  int flush_tx() { return exec_command(0xe1); }
  int flush_rx() { return exec_command(0xe2); }

  int read_rx_payload(uint8_t* data, int len) { return exec_read_with_buffer(0x61 /* r_rx_payload */, data, len); }
  int write_tx_payload(uint8_t* data, int len) { return exec_command_with_buffer(0xa0 /* w_tx_payload */, data, len); }
  int write_tx_payload_no_ack(uint8_t* data, int len) { return exec_command_with_buffer(0xb0 /* w_tx_payload_no_ack */, data, len); }
  int write_ack_payload(int pipe, uint8_t* data, int len) { return exec_command_with_buffer(0xa8 | (pipe & 7) /* w_ack_payload */, data, len); }

  int exec_command(uint8_t command) {
    if (DEBUG) printf("command %02x ->", (int)command);
    wiringPiSPIDataRW(_channel, &command, 1);
    if (DEBUG) printf(" status %02x\n", (int)command);
    return command; // status
  }

  int exec_read_with_buffer(uint8_t command, uint8_t* data, int len) {
    if (len && !data) { printf("exec_read_with_buffer() called with NULL 'data' arg"); exit(1); }

    uint8_t buf[len + 1];
    buf[0] = command;
    bzero(buf + 1, len);
    if (DEBUG) {
      printf("read command %02x ->", (int)command);
    }
    wiringPiSPIDataRW(_channel, buf, sizeof(buf));
    if (len) { memcpy(data, buf + 1, len); }
    if (DEBUG) {
      for (int i = 0; i < len; ++i) printf(" %02x", (int)data[i]);
      printf(" (status %02x)\n", (int)buf[0]);
    }
    return buf[0]; // status
  }

  int exec_command_with_buffer(uint8_t command, uint8_t* data, int len) {
    if (len && !data) { printf("exec_read_with_buffer() called with NULL 'data' arg"); exit(1); }

    uint8_t buf[len + 1];
    buf[0] = command;
    if (len) memcpy(buf + 1, data, len);
    if (DEBUG) {
      printf("write command %02x:", (int)command);
      for (int i = 0; i < len; ++i) printf(" %02x", (int)data[i]);
      printf(" ->");
    }
    wiringPiSPIDataRW(_channel, buf, sizeof(buf));
    if (len) memcpy(data, buf + 1, len);
    if (DEBUG) {
      for (int i = 0; i < len; ++i) printf(" %02x", (int)data[i]);
      printf(" (status %02x)\n", (int)buf[0]);
    }
    return buf[0]; // status
  }
};

// flag so that we don't accidentally kill the rpi by shorting its gpios
static int avr_active = 1;

static void reset_avr() {
  pinMode(PIN_AVR_RESET, OUTPUT);
  digitalWrite(PIN_AVR_RESET, 0);
  delayMicroseconds(100);

  avr_active = 0;
}

// put the avr into reset, which will tristate all its outputs (except possibly MISO, if we push $AC on MOSI, because it'll get confused and think it's in programming mode)
void shut_down_avr() {
  printf("Shutting down AVR\n");
  reset_avr();
}

void restart_avr() {
  printf("Restarting AVR\n");
  reset_avr();
  delayMicroseconds(100);

  // disable SPI pins
  pinMode(PIN_AVR_CE0, INPUT);
  pinMode(PIN_RF24_CE1, INPUT);
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
    shut_down_avr();
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

  /*
  void bit_bang(unsigned char* data, int len) {
    for (unsigned char* end = data + len; data != end; ++data) {
      for (int bit = 0; bit < 8; ++bit) {
	digitalWrite(_pin_clk, 0);
	digitalWrite(_pin_data, (*data & (0x80 >> bit)) ? 1 : 0);
	delayMicroseconds(1);
	digitalWrite(_pin_clk, 1);
	delayMicroseconds(1);
      }
    }
    digitalWrite(_pin_clk, 0);
    digitalWrite(_pin_data, 0);
    delayMicroseconds(500);
  }
  */
};

class AVR {
 public:

  AVR() {
  }

  void setup() {
    restart_avr();
    enable_spi_pins();
  }
};

nRF24L01 rf24(RF24_SPI_CHANNEL, // SPI channel 1 (CSN = P1-26 / CE1)
	      PIN_RF24_CE, // CE on P1-18 (GPIO 24)
	      PIN_RF24_IRQ); // IRQ on P1-22 (GPIO 25)

uint8_t tx_addr[5] = {0xE1, 0xF0, 0xF0, 0xF0, 0xF0};
uint8_t rx_addr[5] = {0xD2, 0xF0, 0xF0, 0xF0, 0xF0};

WS2801 leds(LED_SPI_CHANNEL);

AVR avr;

extern void setup_udp();
extern void close_udp();
extern int check_udp(unsigned char* frame_buf, size_t frame_buf_len);

int main() {
  printf("init wiringPi\n");
  if (wiringPiSetupGpio() < 0) {
    printf("failed to init wiringPi\n");
    return -1;
  }

  //avr.setup();
  leds.setup();

  setup_udp();
  unsigned char buf[900];
  printf("Waiting for UDP frames\n"); fflush(stdout);
  //int start_time = millis();
  while (1) { //millis() - start_time < 10000) {
    if (check_udp(buf, 900)) {
      leds.send(buf, 900);
      fflush(stdout);
    } else {
      delay(1);
    }
  }
  close_udp();

  shut_down_avr();
  // radio
  if (avr_active) {
    printf("can't init rf24 with avr active\n");
  } else {
    rf24.setup();
    rf24.read_reg(STATUS);
    rf24.read_reg(CONFIG);
    rf24.write_tx_addr(tx_addr, 5);
    rf24.write_rx_addr(0, rx_addr, 5);
    rf24.set_retries(15); // retry 15 times
    rf24.set_retry_delay(1); // 500 us
  }

  // and back
  restart_avr();
}
