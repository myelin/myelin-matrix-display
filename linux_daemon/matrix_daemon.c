/*
 * Copyright (C) 2012 Phillip Pearson <pp@myelin.co.nz>
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#ifdef __linux__
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#define FAST_RS232
#endif
#include <deque>
using namespace std;

#define WIDTH 25
#define HEIGHT 12
#define LEN (WIDTH * HEIGHT)
//#define BAUD 500000
#define BAUD 1000000

int portfd;
int debuglevel = 2;

int setup_serial(const char* port_path) {
  int fd;
  struct termios options;
#ifdef FAST_RS232
  struct serial_struct serialinfo;
#endif

  printf("Opening serial port %s\n", port_path);
  fd = open(port_path, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    perror("open_port: Unable to open port - ");
    exit(1);
  }
  printf("opened; fd %d\n", fd);

  /* set blocking -- fcntl(fd, F_SETFL, FNDELAY); to make read() return 0 if no chars are available */
  fcntl(fd, F_SETFL, 0);

  printf("- Setting termios options\n");
  tcgetattr(fd, &options);
#ifdef FAST_RS232
#define BASE_BAUD B38400
#else
#define BASE_BAUD B115200
#endif
  if (cfsetispeed(&options, BASE_BAUD)) {
    perror("failed to set port to 115200 bps");
    exit(1);
  }
  if (cfsetospeed(&options, BASE_BAUD)) {
    perror("failed to set port output speed to 115200 bps");
    exit(1);
  }
  options.c_cflag |= (CLOCAL | CREAD);
  // 8N1
  //  options.c_cflag &= ~PARENB;
  //  options.c_cflag &= ~CSTOPB;
  //  options.c_cflag &= ~CSIZE;
  //  options.c_cflag |= CS8;
  //  options.c_cflag |= CRTSCTS; // hardware flow control
  //  options.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control
  //  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw input
  //  options.c_oflag &= ~OPOST; // raw output

  // from http://todbot.com/arduino/host/arduino-serial/arduino-serial.c
  // 8N1
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  // no flow control
  options.c_cflag &= ~CRTSCTS;

  options.c_cflag |= CREAD | HUPCL | CLOCAL;  // turn on READ & ignore ctrl lines
  options.c_iflag = 0; // &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
  options.c_oflag = 0; // &= ~OPOST; // make raw

  // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
  options.c_cc[VMIN]  = 255;
  options.c_cc[VTIME] = 10;

  // set termios options
  tcsetattr(fd, TCSANOW, &options);

#ifdef FAST_RS232
  printf("- Setting Linux serial_struct options\n");
  ioctl(fd, TIOCGSERIAL, &serialinfo);
  serialinfo.custom_divisor = serialinfo.baud_base / BAUD;
  serialinfo.flags = (serialinfo.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
  ioctl(fd, TIOCSSERIAL, &serialinfo);
  printf("- Custom divisor set to %d (baud_base is %d), which should give us %d baud\n",
	 (int)serialinfo.custom_divisor, (int)serialinfo.baud_base,
	 (int)(serialinfo.baud_base / serialinfo.custom_divisor));
#endif

  printf("- Serial port open\n");
  return fd;
}

int millitime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

struct Frame {
  unsigned char buffer[LEN * 3];
  void blank() {
    for (int i = 0; i < LEN * 3; ++i) buffer[i] = 0;
  }
  void set(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    int pos = y * WIDTH + (y & 1 ? (25 - x) : x);
    buffer[pos * 3] = r;
    buffer[pos * 3 + 1] = g;
    buffer[pos * 3 + 2] = b;
  }
  void rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b) {
    for (int y = y0; y < y1; ++y) {
      for (int x = x0; x < x1; ++x) {
	set(x, y, r, g, b);
      }
    }
  }
};

// set port blocking (or not)
void set_blocking(int blocking) {
  fcntl(portfd, F_SETFL, blocking ? 0 : FNDELAY);
}

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
	  write(portfd, &c, 1);
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

int main(int argc, char *argv[]) {
  int buf_size = 1024;
  unsigned char buf[buf_size];
  unsigned int total_bytes_sent = 0;
  Frame frame;
  int n_frame;
  int start_millis = millitime();
  int last_frame_start_millis = start_millis;
  std::deque<unsigned char> input;
  const char* preamble = "OK.";

  printf("Want It! matrix display (C) 2012 Phillip Pearson, pp@myelin.co.nz\n");

  if (argc < 2) {
    printf("Syntax: %s </path/to/serial/port>\n", argv[0]);
    exit(1);
  }

  portfd = setup_serial(argv[1]);

  //test_loop_corruption();

  for (n_frame = 0; ; ++n_frame) {
    int frame_start_millis = millitime();
    int ready_for_next = 0;
    int preamble_read_pos = 0;
    int ms_since_start = frame_start_millis - start_millis;
    int bytes_per_sec = ms_since_start ? (total_bytes_sent * 1000 / ms_since_start) : 0;
    if (debuglevel >= 1) {
      printf("%d ms since last frame / %d bytes in %d ms / %d bytes/sec\n",
	     frame_start_millis - last_frame_start_millis,
	     total_bytes_sent,
	     ms_since_start,
	     bytes_per_sec);
    }
    last_frame_start_millis = frame_start_millis;

    while (!ready_for_next) {
      ssize_t bytes_read = read(portfd, buf, 1);
      if (bytes_read == 0) {
	printf("write junk data to kick the avr\n");
	write(portfd, ".", 1);
	continue;
      }

      if (bytes_read == -1) {
	perror("error reading from serial port");
	continue;
      }

      if (debuglevel >= 10) printf("%d: ", (int)bytes_read);
      for (ssize_t i = 0; i < bytes_read; ++i) {
	input.push_back(buf[i]);
	if (debuglevel >= 10) {
	  if (buf[i] < 32) printf("<%02x>", buf[i]);
	  else putchar(buf[i]);
	}
      }
      if (debuglevel >= 10) printf("\n");

      while (input.size()) {
	if (preamble_read_pos == strlen(preamble)) {
	  ready_for_next = 1;
	  break;
	}
	unsigned char c = input.front();
	input.pop_front();
	if (c == preamble[preamble_read_pos]) ++preamble_read_pos;
      }
    }

    if (n_frame == 0) {
      // arduino just initialized; reset timers
      last_frame_start_millis = frame_start_millis = start_millis = millitime();
    }

    if (debuglevel >= 2) printf("[waited %d milliseconds]\n", millitime() - frame_start_millis);

    // do something
    frame.blank();
    //frame.rect(0, 0, WIDTH, HEIGHT, rand() % 256, rand() % 256, rand() % 256);
    frame.set(n_frame % LEN, 0, 0xff, 0xff, 0xff); //rand() & 0xff, rand() & 0xff, rand() & 0xff);

    if (debuglevel >= 1) printf("Sending test frame %d\n", n_frame);
    //set_blocking(1);
    buf[0] = buf[1] = '*';
    buf[2] = buf[3] = '+';
    if (write(portfd, buf, 4) != 4) {
      printf("failed to write packet start\n");
    }
    //set_blocking(0);
    while (1) {
      unsigned char c;
      while (read(portfd, &c, 1) < 1) {
	if (debuglevel >= 10) printf("nothing\n");
	usleep(10000);
      }
      if (c == '#') {
	if (debuglevel >= 5) printf("received start signal from avr\n");
	break;
      } else if (debuglevel >= 10) {
	if (c < 32)
	  printf("received junk byte <%02x> from avr\n", (int)c);
	else
	  printf("received junk byte '%c' from avr\n", (int)c);
      }
    }
    set_blocking(1);
    ssize_t bytes_written = write(portfd, frame.buffer, 900);
    /*ssize_t bytes_written = 0;
    for (int blk = 0; blk < LEN * 3; blk += 100) {
      int ct = write(portfd, frame.buffer + blk, 100);
      bytes_written += ct;
      printf("wrote %d bytes\n", ct);
    }
    */
    total_bytes_sent += bytes_written;
    if (debuglevel >= 5) printf("(total %d bytes)\n", (int)bytes_written);
    if (bytes_written != LEN * 3) {
      printf("not 900?\n");
      exit(1);
    }
  }

  close(portfd);

  return 0;
}
