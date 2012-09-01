//TODO: replace this with avr libc uart_* calls

#include <avr/io.h>

// comms speed
//#define BAUD 1000000
#define BAUD 57600

// byte available to read from the UART?
#define serial_available() (UCSR0A & (1<<RXC0))

// read a byte from the UART
#define serial_read() (UDR0)

// is the UART ready for us to write a byte to it?
#define serial_can_write() (UCSR0A & (1 << UDRE0))

// write a byte to the UART
extern void serial_write(uint8_t c);

// write a string to the UART
extern void serial_print(const char* s);

extern void serial_setup();
