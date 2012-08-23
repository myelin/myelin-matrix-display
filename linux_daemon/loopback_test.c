#include "matrix_daemon.h"

// debugging: send data down the wire and see what comes back (and how fast we can do it).
// program device with test_fast_serial_reception sketch first, making sure to match baud rates with here.
void test_loop_corruption() {
  // long packet test
  int packet_size = 255;
  unsigned char packet[packet_size];
  for (int i = 0; i < packet_size; ++i) packet[i] = (unsigned char)i;

  set_blocking(1);

  while (1) {
    unsigned char c;
    while (read(portfd, &c, 1) < 1);
    if (c == '*') {
      printf("received start signal from avr\n");
      break;
    } else {
      printf("received junk byte <%02x> from avr\n", (int)c);
    }
  }

  if (0) {
    while (1) {
      unsigned char c = (unsigned char)rand();
      ssize_t n_written = write(portfd, &c, 1);
      unsigned char r;
      ssize_t n_read = read(portfd, &r, 1);
      printf("wrote %d bytes <%02x>, read %d bytes <%02x>\n", (int)n_written, c, (int)n_read, r);
    }
  }

  if (1) {
    unsigned char rxbuf[packet_size];
    for (int loop_no = 0; ; ++loop_no) {
      ssize_t n_written = write(portfd, packet, packet_size);
      printf("[%d] wrote %d bytes\n", loop_no, (int)n_written);
      ssize_t n_read = read(portfd, rxbuf, n_written);
      int n_errors = 0;
      for (int i = 0; i < min(n_read, n_written); ++i) {
	if (packet[i] != rxbuf[i]) {
	  ++n_errors;
	  printf("  got <%02x> instead of <%02x> at pos %d\n", rxbuf[i], packet[i], i);
	} //else printf("  got <%02x> as expected at pos %d\n", rxbuf[i], i);
      }
      printf("wrote %d bytes (trying to write %d), read %d back -- with %d mismatched bytes\n", (int)n_written, packet_size, (int)n_read, n_errors);
      if (n_written != n_read) {
	printf("wrote/read mismatch\n");
	exit(1);
      }
      if (n_errors) {
	printf("resyncing\n");
	while (read(portfd, rxbuf, packet_size) > 0);
	while (1) {
	  unsigned char c = (unsigned char)rand();
	  if (write(portfd, &c, 1) != 1) {
	    printf("  (odd return from write())");
	  }
	  unsigned char d;
	  if (read(portfd, &d, 1) == 1 && d == c) {
	    printf("  sync achieved\n");
	    break;
	  }
	  printf("  mismatch\n");
	}
      }
    }
  }
}

