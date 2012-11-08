#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#ifndef SLOW_REMOTE
#include <SoftwareSerial.h>
#endif
#include "Keypad.h"
#include <avr/power.h>

/*
 * Driver for remote control for matrix display
 * (c) 2012 Phillip Pearson
 */

#ifndef SLOW_REMOTE
// Serial port to talk to computer on 11 (MOSI used as RX) and 12 (MISO for TX)
SoftwareSerial spiSerial(11, 12);
#endif

// nRF24L01+ radio; uses RF24 lib and hardware SPI pins plus CE on PB0/8, IRQ on PB1, CSN on PB2/10
RF24 radio(8, 10);

// controller (us) takes this address
#define MASTER_ADDRESS 0xF0F0F0F0E1LL

uint64_t remote_addresses[] = {
  //0x651E3111A3LL, // phil board ONE
  //0x659A5234D1LL, // phil board TWO
  0xD3DFAAF79BLL, // lamp board 3 (green terminal block power connector)
  0x3663D8A1B5LL, // lamp board 4 (green terminal block power connector)
  0xF8D45E390FLL // lamp board 5 (home made power connector)
};
#define N_REMOTES (sizeof(remote_addresses) / sizeof(uint64_t))

// Adafruit keypad (http://adafru.it/419)
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {0, 4, 8},
  {1, 5, 9},
  {2, 6, 10},
  {3, 7, 11}
};
// row pins go to m328p pins 28/pc5/A5, 27/pc4/A4, 26/pc3/A3, 2/pd0/0, col pins go to 3/pd1/1, 4/pd2/2, 5/pd3/3
byte rowPins[ROWS] = {A5, A4, A3, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {1, 2, 3}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Duemilanove has an LED on pin 13
#define LED 13

#ifndef SLOW_REMOTE
int serial_putc(char c, FILE *) {
  uint8_t saved_spcr = SPCR;
  SPCR &= ~(1<<SPE);
  spiSerial.write(c);
  SPCR = saved_spcr;
  return c;
}

void printf_begin(void) {
  fdevopen(&serial_putc, 0);
}
#endif

static void select_slave(uint8_t idx) {
//  printf("select slave %d\r\n", (int)idx);
  radio.openWritingPipe(remote_addresses[idx]);
}

void setup() {
#ifdef SLOW_REMOTE
  clock_prescale_set(clock_div_8); // if changing this, you also need to change F_CPU in the makefile
#endif

  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
#ifndef SLOW_REMOTE
  spiSerial.begin(38400);
  printf_begin();
#endif

  radio.begin();
  radio.setRetries(15, 0);

  // we're sending to one of the slaves, and listening on the master address
  select_slave(0);
  radio.openReadingPipe(1, MASTER_ADDRESS);
  radio.stopListening(); // save power

#ifndef SLOW_REMOTE
  radio.printDetails();
#endif
  radio.powerDown();
}

#define DEBUG_TRANSMISSION
long last = 0;
static bool send_command_buffer(uint8_t* buffer) {
  radio.powerUp();
  bool ok, r = false;
  uint8_t giveup = 0;
  do {
    long start = millis();
    ok = radio.write(buffer, 2);
    long now = millis();
    if (ok) {
#ifdef DEBUG_TRANSMISSION
      printf("ok %d %d ", (int)(now - start), (int)(now - last));
#endif
      r = true;
    } else {
#ifdef DEBUG_TRANSMISSION
      printf("FAIL %d ", (int)(now - last));
#endif
      if (++giveup > 10) {
        // too many failures
        ok = 1;
      }
    }
    last = now;
  } while (!ok);
  radio.powerDown();
  return r;
}

static bool transmit_single_command(uint8_t slave, uint8_t* command) {
  select_slave(slave);
  return send_command_buffer(command);
}

#ifdef FLASH_LED
uint8_t led = 0;
#endif

void loop() {
  // save SPI settings
  uint8_t old_spcr = SPCR;
  SPCR &= ~(1<<SPE);

  // flash
  pinMode(LED, OUTPUT);
#ifdef FLASH_LED
  led = ~led;
  digitalWrite(LED, led);
#else
  digitalWrite(LED, 0); // save power
#endif

  // check keypad and output over MISO if anything happened
  char key = keypad.getKey();
  if (key) {
    pinMode(12, OUTPUT); // set MISO as serial output
#ifndef SLOW_REMOTE
    spiSerial.print("key: ");
    spiSerial.println(key);
#endif
  }

  // wait a bit - because we'll lose the LED as soon as we re-enable SPI
  delay(50);
  // re-enable SPI
  SPCR = old_spcr;

  // send key over radio if necessary
  if (key) {
    uint8_t lamp_no = key / 4, mode = key % 4;

    uint8_t command[2];
    command[0] = 3;
    command[1] = mode;
    transmit_single_command(lamp_no, command);
  }
}
