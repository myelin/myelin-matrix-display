//TODO: replace this with avr libc uart_* calls

#include "serial.h"

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

void serial_setup() {
  // set up UART
  UCSR0A |= 1<<U2X0;
  UCSR0B |= (1<<RXEN0 | 1<<TXEN0);
  uint16_t divider = (F_CPU / 4 / BAUD - 1) / 2;
  UBRR0H = divider >> 8;
  UBRR0L = divider & 0xFF;
}

