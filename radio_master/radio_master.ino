#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "deadbeef_rand.h"

//#define DEBUG_TRANSMISSION

// nRF24L01+ radio; uses RF24 lib and hardware SPI pins plus CE on D8 and CSN on D10
RF24 radio(8,10);

// controller takes this address
#define MASTER_ADDRESS 0xF0F0F0F0E1LL

//TODO store as 6 byte arrays rather than 8-byte uint64_ts
uint64_t remote_addresses[] = {
  //0x651E3111A3LL, // phil board ONE
  //0x659A5234D1LL, // phil board TWO
  0xD3DFAAF79BLL, // lamp board 3 (green terminal block power connector)
  0x3663D8A1B5LL // lamp board 4 (green terminal block power connector)
  //0xF8D45E390FLL, // lamp board 5 (home made power connector)
};
#define N_REMOTES (sizeof(remote_addresses) / sizeof(uint64_t))

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
#ifdef DEBUG_TRANSMISSION
      printf("ok %d %d ", (int)(now - start), (int)(now - last));
#endif
    } else {
#ifdef DEBUG_TRANSMISSION
      printf("FAIL %d ", (int)(now - last));
#endif
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

uint8_t read_number() {
  while (1) {
    while (!Serial.available());
    uint8_t c = Serial.read();
    if (c == 10 || c == 13) continue;
    return c - '0';
  }
}

void loop(void)
{
  uint8_t lamp_no;
  while (1) {
    printf("\nEnter the number of the lamp you want to control (1-%d).\r\n", (int)N_REMOTES);
    lamp_no = read_number();
    if (lamp_no < 1 || lamp_no > N_REMOTES) {
      printf("Invalid lamp number!\r\n");
      continue;
    }
    printf("Lamp %d selected.\r\n", lamp_no);
    break;
  }
  uint8_t mode;
  while (1) {
    printf("Enter the number of the mode you want to switch it to:\r\n");
    printf("  1 = chase mode\r\n");
    printf("  2 = chill rainbow mode\r\n");
    printf("Or 0 to cancel\r\n");
    switch (read_number()) {
      case 0: mode = 0; break; // cancel
      case 1: mode = 2; break; // chase
      case 2: mode = 3; break; // rainbow
      default:
        printf("Invalid mode number!\r\n");
        continue;
    }
    break;
  }
  if (!mode) return; // 0 to cancel

  uint8_t command[32];
  command[0] = 3;
  command[1] = mode;
  memset(command+2, 0, 30);
  if (transmit_single_command(lamp_no - 1, command)) {
    printf("Command sent successfully\r\n");
  } else {
    printf("Unable to send command to lamp %d -- is it turned on and within range?\r\n", lamp_no);
  }
}

