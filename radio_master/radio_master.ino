#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "deadbeef_rand.h"

// nRF24L01+ radio; uses RF24 lib and hardware SPI pins plus CE on D8 and CSN on D10
RF24 radio(8,10);

// controller takes this address
#define MASTER_ADDRESS 0xF0F0F0F0E1LL

uint8_t n_remotes = 2;
//TODO store as 6 byte arrays rather than 8-byte uint64_ts
uint64_t remote_addresses[] = {
  //0x651E3111A3LL, // phil board ONE
  //0x659A5234D1LL, // phil board TWO
  0xD3DFAAF79BLL, // lamp board 3 (green terminal block power connector)
  0x3663D8A1B5LL // lamp board 4 (green terminal block power connector)
  //0xF8D45E390FLL, // lamp board 5 (home made power connector)
};

#define N_PIXELS 50
#define PIXEL_SIZE 3

// random numbers are hard
static uint32_t deadbeef_seed;
static uint32_t deadbeef_beef = 0xdeadbeef;

uint32_t deadbeef_rand() {
	deadbeef_seed = (deadbeef_seed << 7) ^ ((deadbeef_seed >> 25) + deadbeef_beef);
	deadbeef_beef = (deadbeef_beef << 7) ^ ((deadbeef_beef >> 25) + 0xdeadbeef);
	return deadbeef_seed;
}

void deadbeef_srand(uint32_t x) {
	deadbeef_seed = x;
	deadbeef_beef = 0xdeadbeef;
}

static void select_slave(uint8_t idx) {
//  printf("select slave %d\r\n", (int)idx);
  radio.openWritingPipe(remote_addresses[idx]);
}

void setup(void)
{
  Serial.begin(115200);
  printf_begin();

  radio.begin();
  radio.setRetries(15, 0); // no retries - they don't seem to work for me

  // we're sending to one of the slaves, and listening on the master address
  select_slave(0);
  radio.openReadingPipe(1, MASTER_ADDRESS);

  // We're not even going to listen to the slave nodes
  radio.stopListening();

  radio.printDetails();
}

long last = 0, last_frame = 0;

static bool send_command_buffer(uint8_t* buffer) {
  bool ok;
  uint8_t giveup = 0;
  do {
    long start = millis();
    ok = radio.write( buffer, 32 );
    long now = millis();
    if (ok) {
      printf("ok %d %d ", (int)(now - start), (int)(now - last));
    } else {
      printf("FAIL %d ", (int)(now - last));
      if (++giveup > 10) {
        // too many failures
        ok = 1;
        return false;
      }
    }
    last = now;
  } while (!ok);
  return true;
}

static bool transmit_single_command(uint8_t slave, uint8_t* command) {
  select_slave(slave);
  return send_command_buffer(command);
}

static void transmit_buffer(uint8_t slave, uint8_t* display) {
  // set TX_ADDR to point to the correct slave
  select_slave(slave);
  printf("    %d ", (int)slave);

  // send the pixel values in 5 packets
  uint8_t outbuf[32];
  for (int bank = 0; bank < 5; ++bank) {
    outbuf[0] = (bank == 4) ? 1 : 0; // change and update
    outbuf[1] = bank;
    for (uint8_t p = 0; p < 10 * PIXEL_SIZE; ++p) outbuf[2 + p] = display[bank * 10 * PIXEL_SIZE + p];
    //memcpy(outbuf + 2, display + bank * 10 * PIXEL_SIZE, 10 * PIXEL_SIZE);
    // send until we get an ack
    if (!send_command_buffer(outbuf)) break;
  }
  printf("\r\n");
}

uint8_t modes_to_access[] = {
  2, // chase
  3 // rainbow
};

void loop(void)
{
  // loop through all modes
  uint8_t command[32];
  command[0] = 3;
  uint8_t mode = 0;
  while (true) {
    command[0] = 3;
    command[1] = modes_to_access[mode];
    memset(command+2, 0, 30);
    transmit_single_command(0, command);
    mode = (mode + 1) % sizeof(modes_to_access);
    delay(10000);
  }
  
  static uint8_t current_slave = 0;
  static uint8_t counter = 0;
  
  if (counter++ > 10) {
    counter = 0;
    current_slave = (current_slave + 1) % n_remotes;
  }

  uint8_t display[N_PIXELS * PIXEL_SIZE];
#define ALL_RANDOM
//#define FLASH
//#define FADE

#ifdef ALL_RANDOM
  // make something random to send
  for (uint8_t p = 0; p < N_PIXELS * PIXEL_SIZE; ++p) {
    display[p] = deadbeef_rand();
  }
#endif
#ifdef FLASH
  static bool flash;
  if (flash) flash = 0; else flash = ((deadbeef_rand() % 4) == 0);
  for (uint8_t p = 0; p < N_PIXELS * PIXEL_SIZE; ++p) {
    display[p] = flash ? 0x0F : 0;
  }
#endif
#ifdef FADE
  static uint8_t fade_pos = 0;
  for (uint8_t p = 0; p < N_PIXELS; ++p) {
    display[p * 3] = 255 - fade_pos;
    display[p * 3 + 1] = 255 - fade_pos;
    display[p * 3 + 2] = 0;
  }
  fade_pos += 25;
#endif

  for (uint8_t slave = 0; slave < n_remotes; ++slave) {
    transmit_buffer(slave, display);
  }
  
  long now = millis();
  printf("-> %d ms\r\n", (int)(now - last_frame));

#define FRAME_TIME 75
  // delay before next frame
  if ((int)(now - last_frame) < FRAME_TIME) delay(FRAME_TIME - (int)(now - last_frame));
  last_frame = millis();
}

