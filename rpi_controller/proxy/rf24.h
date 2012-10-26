#include "mx2_board.h"

class nRF24L01 {
 public:
  int _channel;
  int _pin_ce;
  int _pin_irq;

  nRF24L01(int channel, int pin_ce, int pin_irq);
  void setup();
  int set_tx_addr(uint8_t* addr, int len);
  int set_rx_addr(int pipe, uint8_t* addr, int len);
  void listen();
  int receive(uint8_t* buf, int buf_len);
  void set_retries(int retries);
  void set_retry_delay(int delay);

 private:
  void transmit();
  int write_reg(int reg, uint8_t d);
  int write_reg(int reg, uint8_t* data, int len);
  int read_reg(int reg);
  int status();
  int flush_tx();
  int flush_rx();
  int read_rx_payload(uint8_t* data, int len);
  int write_tx_payload(uint8_t* data, int len);
  int write_tx_payload_no_ack(uint8_t* data, int len);
  int write_ack_payload(int pipe, uint8_t* data, int len);
  int exec_command(uint8_t command);
  int exec_read_with_buffer(uint8_t command, uint8_t* data, int len);
  int exec_command_with_buffer(uint8_t command, uint8_t* data, int len);
};
