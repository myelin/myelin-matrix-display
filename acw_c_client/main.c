#include "matrix.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static int udp_fd;

void setup_display() {
  // prepare UDP socket
  udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (!udp_fd) {
    printf("setup_display(): unable to create socket\n");
    exit(1);
  }
}

void send_to_display(unsigned char* buffer, int buffer_len) {
  int expected_len = BUF_SIZE;
  if (buffer_len != expected_len) {
    printf("send_to_display(): incorrect buffer length %d; expected %d\n", buffer_len, expected_len);
    exit(1);
  }

  // reformat buffer
  unsigned char output[expected_len + 1];
  output[0] = 1; // protocol header
  memcpy(output + 1, buffer, expected_len); // ADDRESSING_HORIZONTAL_NORMAL

  // set output address
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(58082); // send on port 58082 - Disorient protocol

  // send udp frame to emulator on localhost
  inet_aton("127.0.0.1", &addr.sin_addr); // localhost
  sendto(udp_fd, output, expected_len + 1, 0, (const struct sockaddr *)&addr, sizeof(addr));

  // and to want it! matrix
  inet_aton("192.168.1.78", &addr.sin_addr); // localhost
  sendto(udp_fd, output, expected_len + 1, 0, (const struct sockaddr *)&addr, sizeof(addr));
}

extern void draw_frame(int frame);

static unsigned char draw_buf[BUF_SIZE];

void point(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
  int pos = (RGB ? 3 : 0) * (y * WIDTH + x);
  if (pos > BUF_SIZE) return;
  unsigned char* ptr = draw_buf + pos;
  if (RGB) {
    *ptr++ = r;
    *ptr++ = g;
    *ptr++ = b;
  } else {
    *ptr++ = (r + g + b) / 3;
  }
}

void rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b) {
  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      point(x, y, r, g, b);
    }
  }
}

void rect(int x0, int y0, int x1, int y1, int c) {
  rect(x0, y0, x1, y1, (unsigned char)(c >> 16), (unsigned char)(c >> 8), (unsigned char)c);
}

int random_color() {
  return rand() & 0xFFFFFF;
}

int color_fade(int color1, int color2, int pos) {
  int r1 = (color1 >> 16) & 0xFF, g1 = (color1 >> 8) & 0xFF, b1 = color1 & 0xFF;
  int r2 = (color2 >> 16) & 0xFF, g2 = (color2 >> 8) & 0xFF, b2 = color2 & 0xFF;

  int antipos = 255 - pos;
  int r = (r1 * antipos + r2 * pos) / 510, g = (g1 * antipos + g2 * pos) / 510, b = (b1 * antipos + b2 * pos) / 510;

  return (r << 16) | (g << 8) | b;
}

void blank() {
  bzero(draw_buf, sizeof(draw_buf));
}

int main() {
  setup_display();
  for (int frame = 0; ; ++frame) {
    draw_frame(frame);
    printf("sending frame %d to display\n", frame);
    send_to_display(draw_buf, sizeof(draw_buf));
    usleep(1000000 / FRAMERATE); //TODO: take into account generate/send time
  }
}
