#include "rf24.h"

#define RADIO_DEBUG 0

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
#define RX_DR 0x40
#define TX_DS 0x20
#define MAX_RT 0x10
#define TX_FULL 1

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
#define RX_EMPTY 1
//ACK_PLD
//TX_PLD
//RX_PLD
#define DYNPD 0x1C
#define FEATURE 0x1D

void wr(int pin, int value) {
  //printf("  write %d %s\n", pin, value ? "HIGH" : "LOW");
  digitalWrite(pin, value);
}

static void softSPIRW(uint8_t* data, int len) {
  int _pin_clk = PIN_RF24_SCK;
  int _pin_mosi = PIN_RF24_MOSI;
  int _pin_miso = PIN_RF24_MISO;
  int _pin_csn = PIN_RF24_CSN;

  // nRF24L01+ uses normal SPI (clock idle low, data read on rising clock edge)
  // bit order: msb -> lsb
  // byte order: lsB -> msB

  // start transaction
  wr(_pin_csn, 0);
  delayMicroseconds(1);

  // write/read bytes
  for (unsigned char* end = data + len; data != end; ++data) {
    uint8_t input_byte;
    for (int bit = 0; bit < 8; ++bit) {
      // set up data on falling edge
      wr(_pin_clk, 0);
      wr(_pin_mosi, (*data & (0x80 >> bit)) ? 1 : 0);
      delayMicroseconds(1);
      // rising edge
      wr(_pin_clk, 1);
      delayMicroseconds(1);
      // sample MISO before falling edge -- slave will update shortly after falling edge
      int input = (digitalRead(_pin_miso) ? 1 : 0);
      //printf("  [miso %d]\n", input);
      input_byte = (input_byte << 1) | input;
    }
    // replace byte from caller with byte read from slave
    *data = input_byte;
  }

  // clear clock and data
  wr(_pin_mosi, 0);
  wr(_pin_clk, 0);

  // end transaction
  delayMicroseconds(1);
  wr(_pin_csn, 1);
}

nRF24L01::nRF24L01(int channel, int pin_ce, int pin_irq) {
  _channel = channel;
  _pin_ce = pin_ce;
  _pin_irq = pin_irq;
}

void nRF24L01::setup() {
  printf("setup CE\n");
  pinMode(_pin_ce, OUTPUT);
  printf("write CE low\n");
  digitalWrite(_pin_ce, LOW); // start in standby mode
  printf("setup IRQ\n");
  pinMode(_pin_irq, INPUT);

  printf("power down radio\n");
  write_reg(CONFIG, 0x0C);

  printf("clear flags and flush fifos\n");
  write_reg(STATUS, 0x70); // clear RX_DR, TX_DS, MAX_RT
  flush_tx();
  flush_rx();

  printf("configure radio\n");
  write_reg(RF_SETUP, RF_PWR_H | RF_PWR_L); // set data rate 1MHz, output power 0dBm
  write_reg(RF_CH, 76); // set channel 76 for compatibility with AVR + RF24
  // set auto-retry delay to 1500 us and retransmit count to 15
  write_reg(SETUP_RETR, 0x5F);
  // enable auto-ack
  write_reg(EN_AA, 0x3F);
  // turn off dynamic payloads
  write_reg(DYNPD, 0);
  write_reg(FEATURE, 0);

  printf("power up radio\n");
  write_reg(CONFIG, read_reg(CONFIG) | PWR_UP);
  digitalWrite(_pin_ce, HIGH);
  delayMicroseconds(4500); // abs max Tpd2stby is 4.5 ms

  read_reg(STATUS);
  read_reg(CONFIG);

  printf("EN_AA: %02x\n", read_reg(EN_AA));
  printf("EN_RXADDR: %02x\n", read_reg(EN_RXADDR));
  printf("RF_CH: %02x\n", read_reg(RF_CH));
  printf("RF_SETUP: %02x\n", read_reg(RF_SETUP));
  printf("CONFIG: %02x\n", read_reg(CONFIG));
  printf("DYNPD: %02x\n", read_reg(DYNPD));
  printf("FEATURE: %02x\n", read_reg(FEATURE));
}

void nRF24L01::listen() {
  printf("listen\n");
  // enable reception
  write_reg(CONFIG, read_reg(CONFIG) | PRIM_RX);
  delayMicroseconds(130); // RX settling
}

int nRF24L01::receive(uint8_t* output, int buf_len) {
  if (!(read_reg(FIFO_STATUS) & RX_EMPTY)) {
    printf("data ready!\n");
    write_reg(STATUS, RX_DR);

    uint8_t buf[32];
    int status = exec_read_with_buffer(0x61, buf, 32);
    if (buf_len > 32) buf_len = 32;
    memcpy(output, buf, buf_len);
    return 1;
  }
  return 0;
}

void nRF24L01::transmit() {
  // disable reception
  write_reg(CONFIG, read_reg(CONFIG) & ~PRIM_RX);

  // put something in TX fifo
  //todo

  // allow transmission
  delayMicroseconds(130); // TX settling

  // check up on transmission???
  //? OBSERVE_TX reg? ARC_CNT PLUS_CNT
}

void nRF24L01::set_retry_delay(int delay) {
  printf("setting retry delay to %d: ", delay);
  write_reg(SETUP_RETR, (read_reg(SETUP_RETR) & 0x0f) | ((delay & 0xf) << 4));
}

void nRF24L01::set_retries(int retries) {
  printf("setting retry count to %d: ", retries);
  write_reg(SETUP_RETR, (read_reg(SETUP_RETR) & 0xf0) | (retries & 0xf));
}

int nRF24L01::set_tx_addr(uint8_t* addr, int len) {
  if (len > 5) { printf("tx addr too long\n"); exit(1); }
  printf("set tx addr (and rx p0 addr)\n");
  write_reg(RX_ADDR_P0, addr, len); // rx addr p0 = tx addr
  write_reg(RX_PW_P0, 32); // pipe 0: 32 bytes per packet
  write_reg(EN_RXADDR, read_reg(EN_RXADDR) | 1); // enable pipe 0
  return write_reg(TX_ADDR, addr, len);
}

int nRF24L01::set_rx_addr(int pipe, uint8_t* addr, int len) {
  if (len > 5) { printf("rx addr too long\n"); exit(1); }
  printf("set rx addr for pipe %d\n", pipe);
  assert(pipe >= 0 && pipe <= 5);
  write_reg(RX_PW_P0 + pipe, 32);
  write_reg(EN_RXADDR, read_reg(EN_RXADDR) | (1<<pipe));
  return write_reg(RX_ADDR_P0 + pipe, addr, len);
}

int nRF24L01::write_reg(int reg, uint8_t d) {
  return exec_command_with_buffer(0x20 | (reg & 0x1f), &d, 1);
}

int nRF24L01::write_reg(int reg, uint8_t* data, int len) {
  return exec_command_with_buffer(0x20 | (reg & 0x1f), data, len);
}

// return the first byte of a (presumably single-byte) register
int nRF24L01::read_reg(int reg) {
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

int nRF24L01::status() { return exec_command(0xff /* nop */); }
int nRF24L01::flush_tx() { return exec_command(0xe1); }
int nRF24L01::flush_rx() { return exec_command(0xe2); }

int nRF24L01::read_rx_payload(uint8_t* data, int len) { return exec_read_with_buffer(0x61 /* r_rx_payload */, data, len); }
int nRF24L01::write_tx_payload(uint8_t* data, int len) { return exec_command_with_buffer(0xa0 /* w_tx_payload */, data, len); }
int nRF24L01::write_tx_payload_no_ack(uint8_t* data, int len) { return exec_command_with_buffer(0xb0 /* w_tx_payload_no_ack */, data, len); }
int nRF24L01::write_ack_payload(int pipe, uint8_t* data, int len) { return exec_command_with_buffer(0xa8 | (pipe & 7) /* w_ack_payload */, data, len); }

int nRF24L01::exec_command(uint8_t command) {
  if (RADIO_DEBUG) printf("command %02x ->", (int)command);
  softSPIRW(&command, 1);
  if (RADIO_DEBUG) printf(" status %02x\n", (int)command);
  return command; // status
}

int nRF24L01::exec_read_with_buffer(uint8_t command, uint8_t* data, int len) {
  if (len && !data) { printf("exec_read_with_buffer() called with NULL 'data' arg"); exit(1); }

  uint8_t buf[len + 1];
  buf[0] = command;
  bzero(buf + 1, len);
  if (RADIO_DEBUG) {
    printf("read command %02x ->", (int)command);
  }
  softSPIRW(buf, sizeof(buf));
  if (len) { memcpy(data, buf + 1, len); }
  if (RADIO_DEBUG) {
    for (int i = 0; i < len; ++i) printf(" %02x", (int)data[i]);
    printf(" (status %02x)\n", (int)buf[0]);
  }
  return buf[0]; // status
}

int nRF24L01::exec_command_with_buffer(uint8_t command, uint8_t* data, int len) {
  if (len && !data) { printf("exec_read_with_buffer() called with NULL 'data' arg"); exit(1); }

  uint8_t buf[len + 1];
  buf[0] = command;
  if (len) memcpy(buf + 1, data, len);
  if (RADIO_DEBUG) {
    printf("write command %02x:", (int)command);
    for (int i = 0; i < len; ++i) printf(" %02x", (int)data[i]);
    printf(" ->");
  }
  softSPIRW(buf, sizeof(buf));
  //if (len) memcpy(data, buf + 1, len);
  if (RADIO_DEBUG) {
    for (int i = 0; i < len; ++i) printf(" %02x", (int)buf[i+1]);
    printf(" (status %02x)\n", (int)buf[0]);
  }
  return buf[0]; // status
}
