#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static int udp_fd;

void setup_udp() {
  // prepare UDP socket
  udp_fd = socket(AF_INET, SOCK_DGRAM, 0);

  // set nonblocking
  fcntl(udp_fd, F_SETFL, FNDELAY);

  // local address
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(58082); // listen on port 58082 - Disorient protocol
  inet_aton("0.0.0.0", &addr.sin_addr); // listen externally

  // start receiving on that address
  bind(udp_fd, (struct sockaddr *)&addr, sizeof(addr));
}

void close_udp() {
  close(udp_fd);
}

int check_udp(unsigned char* frame_buf, size_t frame_buf_len) {
  int buf_len = 2048;
  unsigned char buf[buf_len];
  ssize_t received = recvfrom(udp_fd, buf, buf_len, 0, NULL, NULL); // receive packet from anyone

  if (received == -1 && errno != EAGAIN) perror("udp");
  if (received <= 0) return 0;

  //printf("received %d bytes on udp\n", (int)received);

  if (!frame_buf) return 1; // dummy call to make us skip frames rather than queue them up

  if (buf[0] == 1) {
    size_t to_copy = received - 1;
    if (frame_buf_len < to_copy) to_copy = frame_buf_len;
    memcpy(frame_buf, buf + 1, to_copy);
  } else {
    printf("UDP packet header mismatch; expected 1, got %d\n", (int)buf[0]);
    return 0;
  }

  return 1;
}

