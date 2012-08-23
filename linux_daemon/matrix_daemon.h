// common c includes
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>

#ifdef __linux__
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#ifdef B1000000
#define EASY_FAST_RS232
#else
#define TRICKY_FAST_RS232
#endif
#endif

// c++ includes
#include <deque>
using namespace std;

#define WIDTH 25
#define HEIGHT 12
#define LEN (WIDTH * HEIGHT)
#define PACKET_LEN (LEN * 3)

#define BAUD 1000000

extern int portfd;
extern int debuglevel;

extern void set_blocking(int blocking);
extern void setup_udp();
extern int check_udp(unsigned char* buf, size_t buf_len);
