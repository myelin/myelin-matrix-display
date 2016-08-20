#include "matrix.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
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

#define ESTIMATE_POWER_USAGE

void send_to_display(ScreenBuffer* buffer) {
  // reformat buffer
  int packet_size = buffer->buf_size() + 1;
  unsigned char output[packet_size];
  const unsigned char* pixels = buffer->pixels();
  output[0] = 1; // protocol header
  memcpy(output + 1, pixels, packet_size - 1); // ADDRESSING_HORIZONTAL_NORMAL

#ifdef ESTIMATE_POWER_USAGE
  uint32_t total_brightness = 0;
  for (int i = 0; i < buffer->buf_size(); ++i) {
    total_brightness += pixels[i];
  }
  int max_brightness = buffer->buf_size() * 256;
  float percentage = (float)total_brightness * 100.0 / max_brightness;
  float active_milliamps = (float)total_brightness / 256.0 * 20.0;
  float idle_milliamps = (float)buffer->buf_size() / 3.0 * 1.5;
  float milliamps = active_milliamps + idle_milliamps;
  printf("total brightness %d/%d (%.1f%%); estimated power usage %.2f mA idle + %.2f mA active = %.2f mA @ 5V (%.2f W)\n",
	 total_brightness,
	 max_brightness,
	 percentage,
	 idle_milliamps,
	 active_milliamps,
	 milliamps,
	 milliamps * 5 / 1000
	 );
#endif

  // set output address
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(58082); // send on port 58082 - Disorient protocol

  // send udp frame to emulator on localhost (or display proxy, if on the RPi)
  inet_aton("127.0.0.1", &addr.sin_addr); // localhost
  sendto(udp_fd, output, packet_size, 0, (const struct sockaddr *)&addr, sizeof(addr));

#ifdef PLAT_DESKTOP
  // and to laptop
  //  inet_aton("192.168.1.78", &addr.sin_addr); // localhost
  //  sendto(udp_fd, output, packet_size, 0, (const struct sockaddr *)&addr, sizeof(addr));

  // and to want it! matrix
  inet_aton("192.168.1.99", &addr.sin_addr); // localhost
  sendto(udp_fd, output, packet_size, 0, (const struct sockaddr *)&addr, sizeof(addr));

  // to matrix v2 (raspberry pi)
  //  inet_aton("192.168.1.105", &addr.sin_addr); // localhost
  //  sendto(udp_fd, output, packet_size, 0, (const struct sockaddr *)&addr, sizeof(addr));
#endif
}

#ifdef SEA_OF_DREAMS
#include "sea_of_dreams.h"
#endif

int main() {
  graphics_setup();
  setup_display();
  setup_animation();

#ifdef SEA_OF_DREAMS
  SeaOfDreamsOverlay sea_of_dreams_overlay(graphics_get_buffer());
#endif

  struct timeval last, start, now;
  gettimeofday(&last, NULL);
  for (int frame = 0; ; ++frame) {
    gettimeofday(&start, NULL);
    ScreenBuffer* draw_buf = graphics_prep_frame(frame);
#ifdef SEA_OF_DREAMS
    draw_buf = sea_of_dreams_overlay.overlay(draw_buf, frame);
#endif
    printf("sending frame %d to display\n", frame);

    gettimeofday(&now, NULL);
    long us_elapsed = (now.tv_sec - last.tv_sec) * 1000000 + now.tv_usec - last.tv_usec;
    long us_drawing = (now.tv_sec - start.tv_sec) * 1000000 + now.tv_usec - start.tv_usec;
    long desired_delay = 1000000 / frame_rate;
    printf("frame took %ld us (%ld to draw), c.f. max frame time %ld, so we should delay %ld us\n", us_elapsed, us_drawing, desired_delay, desired_delay - us_elapsed);
    if (desired_delay > us_elapsed) {
      struct timeval a, b;
      gettimeofday(&a, NULL);
      struct timespec sleeper, dummy;
      long delay = desired_delay - us_elapsed;
      sleeper.tv_sec = delay / 1000000;
      sleeper.tv_nsec = (delay % 1000000) * 1000;
      nanosleep(&sleeper, &dummy);
      //usleep(desired_delay - us_elapsed); //TODO: take into account generate/send time
      gettimeofday(&b, NULL);
      //printf("sleep took %ld us\n", (long)((b.tv_sec - a.tv_sec) * 1000000 + b.tv_usec - a.tv_usec));
    }
    gettimeofday(&last, NULL);

    // now that we're hopefully in sync, update the display
    send_to_display(draw_buf);
  }
}
