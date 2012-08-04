/*
 * Copyright (C) 2012 Phillip Pearson <pp@myelin.co.nz>
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdlib.h>
#include <linux/serial.h>

/* Arduino FT232RL serial port on Phil's NSLU2 */
#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD 500000

int setup_serial(void) {
  int fd;
  struct termios options;
  struct serial_struct serialinfo;

  printf("Opening serial port %s\n", SERIAL_PORT);
  fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    perror("open_port: Unable to open "SERIAL_PORT" - ");
    exit(1);
  }

  /* set blocking -- fcntl(fd, F_SETFL, FNDELAY); to make read() return 0 if no chars are available */
  fcntl(fd, F_SETFL, 0);

  printf("- Setting termios options\n");
  tcgetattr(fd, &options);
  cfsetispeed(&options, B38400);
  cfsetospeed(&options, B38400);
  options.c_cflag |= (CLOCAL | CREAD);
  tcsetattr(fd, TCSANOW, &options);

  printf("- Setting serial_struct options\n");
  ioctl(fd, TIOCGSERIAL, &serialinfo);
  serialinfo.custom_divisor = serialinfo.baud_base / BAUD;
  serialinfo.flags = (serialinfo.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
  ioctl(fd, TIOCSSERIAL, &serialinfo);
  printf("- Custom divisor set to %d (baud_base is %d), which should give us %d baud\n",
	 (int)serialinfo.custom_divisor, (int)serialinfo.baud_base,
	 (int)(serialinfo.baud_base / serialinfo.custom_divisor));

  printf("- Serial port open\n");
  return fd;
}

int millitime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

int main(void) {
  int portfd;
  int buf_size = 1024;
  unsigned char buf[buf_size];
  unsigned int total_bytes_sent = 0;
  int frame;
  int start_millis = millitime();
  int last_frame_start_millis = start_millis;

  printf("Want It! matrix display (C) 2012 Phillip Pearson, pp@myelin.co.nz\n");

  portfd = setup_serial();
  // fcntl(portfd, F_SETFL, FNDELAY); // set nonblocking
  for (frame = 0; frame < 1000; ++frame) {
    int frame_start_millis = millitime();
    int ready_for_next = 0;
    int ms_since_start = frame_start_millis - start_millis;
    int bytes_per_sec = ms_since_start ? (total_bytes_sent * 1000 / ms_since_start) : 0;
    printf("Waiting for ready indicator (%d ms since last frame / %d bytes in %d ms / %d bytes/sec)\n",
	   frame_start_millis - last_frame_start_millis,
	   total_bytes_sent,
	   ms_since_start,
	   bytes_per_sec);
    last_frame_start_millis = frame_start_millis;
    printf("Waiting a little bit\n");
    usleep(20000); // 20 ms

    while (!ready_for_next) {
      putchar('.');
      ssize_t i;
      ssize_t bytes_read = read(portfd, buf, buf_size);
      if (bytes_read == 0) {
	usleep(1000); // sleep 1 ms
	continue;
      }

      if (bytes_read == -1) {
	usleep(10000); // 10 ms
	continue;
	perror("error reading from serial port");
      }

      printf("%d: ", bytes_read);
      for (i = 0; i < bytes_read; ++i) {
	if (buf[i] < 32) printf("<%02x>", buf[i]);
	else putchar(buf[i]);
      }
      if (bytes_read >= 3 && buf[bytes_read-3] == 'O' && buf[bytes_read-2] == 'K' && buf[bytes_read-1] == '\n') {
	ready_for_next = 1;
      }
      printf("\n");
    }

    if (frame == 0) {
      // arduino just initialized; reset timers
      last_frame_start_millis = frame_start_millis = start_millis = millitime();
    }

    {
      printf("Sending test frame %d\n", frame);
      unsigned char frame[300 * 3];
      ssize_t bytes_written = write(portfd, frame, 300 * 3);
      total_bytes_sent += bytes_written;
      printf("wrote %d bytes\n", bytes_written);
      if (bytes_written != 900) {
	printf("not 900?\n");
	exit(1);
      }
    }
  }

  close(portfd);

  return 0;
}
