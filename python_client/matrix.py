import socket, random, time, six
sock = socket.socket( socket.AF_INET, # Internet
                      socket.SOCK_DGRAM ) # UDP

PIXEL_SIZE = 3

def encode(data):
    output = []
    for row in data:
        for pixel in row:
            output.append(chr(pixel[0]) + chr(pixel[1]) + chr(pixel[2]))
    return ''.join(output)

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

def is_string(s):
    return isinstance(s, six.string_types)

def random_color():
    return (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

# * wheel(0-767) generates an approximate rainbow.  Based on Wheel()
# * from the Adafruit_WS2801 library, which requires the following
# * text in all distributions:
#
#  Example sketch for driving Adafruit WS2801 pixels!
#
#  Designed specifically to work with the Adafruit RGB Pixels!
#  12mm Bullet shape ----> https://www.adafruit.com/products/322
#  12mm Flat shape   ----> https://www.adafruit.com/products/738
#  36mm Square shape ----> https://www.adafruit.com/products/683
#
#  These pixels use SPI to transmit the color data, and have built in
#  high speed PWM drivers for 24 bit color per pixel
#  2 pins are required to interface
#
#  Adafruit invests time and resources providing this open source code,
#  please support Adafruit and open-source hardware by purchasing
#  products from Adafruit!
#
#  Written by Limor Fried/Ladyada for Adafruit Industries.
#  BSD license, all text above must be included in any redistribution

WHEEL_MAX = 768
def wheel(pos):
    if (pos < 256):
        return (255 - pos, pos, 0)
    elif (pos < 512):
        pos -= 256;
        return (0, 255 - pos, pos);
    else:
        pos -= 512;
        return (pos, 0, 255 - pos);

def wheel_mask(pos):
    return fmultiply(wheel(pos), 1.0 / 256)

def multiply(c, factor):
    #print "multiply",c,factor
    return (int(c[0] * factor), int(c[1] * factor), int(c[2] * factor))

def fmultiply(c, factor):
    #print "multiply",c,factor
    return (float(c[0]) * factor, float(c[1]) * factor, float(c[2]) * factor)

NULL_MASK = (1.0, 1.0, 1.0)

def random_mask():
    return fmultiply(random_color(), 1.0 / 256)

# multiply a colour by a (0-1.0, 0-1.0, 0-1.0) RGB tuple
def cmultiply(c, mask):
    return (c[0] * mask[0], c[1] * mask[1], c[2] * mask[2])

def add(c1, c2):
    return (c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2])

class Frame:
    def __init__(self, width, height, data=None):
        self.width = width
        self.height = height
        if data:
            self.data = data
        else:
            self.blank()

    def load(self, fn):
        pixels = open(fn, 'rb').read()
        assert len(pixels) == self.width * self.height * PIXEL_SIZE
        data = []
        row_size = self.width * PIXEL_SIZE
        for y in range(self.height):
            encoded_row = pixels[y * row_size:(y+1) * row_size]
            row = []
            for x in range(self.width):
                row.append((ord(encoded_row[x * PIXEL_SIZE]), ord(encoded_row[x * PIXEL_SIZE + 1]), ord(encoded_row[x * PIXEL_SIZE + 2])))
            data.append(row)
        self.data = data

    def save(self, fn):
        open(fn, 'wb').write(encode(self.data))
    def copy(self, data):
        if isinstance(data, Frame): data = data.data
        self.data = [row[:] for row in data]

    def make_fill(self, c):
        return [[c for x in range(self.width)] for y in range(self.height)]

    def make_blank(self):
        return self.make_fill(BLACK)

    def blank(self):
        self.data = self.make_blank()

    def dim(self, amount):
        self.data = [
            [
                (int(c[0] * amount), int(c[1] * amount), int(c[2] * amount))
                for c in row
            ] for row in self.data
        ]
    def subtract(self, amount):
        self.data = [
            [
                (max(0, c[0] - amount), max(0, c[1] - amount), max(0, c[2] - amount))
                for c in row
            ] for row in self.data
        ]
    def translate(self, xd, yd):
        self.data = [
            [
                self.data[(y - yd + self.height) % self.height][(x - xd + self.width) % self.width]
                for x in range(self.width)
            ] for y in range(self.height)
        ]
    def show(self):
        show(self.data)
    def fill(self, c):
        self.data = fill(c)
    def get(self, x, y):
        return self.data[y][x]
    def point(self, x, y, c):
        self.data[y][x] = c
    def rect(self, x0, y0, x1, y1, c):
        if x1 < x0: x0, x1 = x1, x0
        if y1 < y0: y0, y1 = y1, y0
        # corners of rectangle: (x0, y0)-(x1, y1)
        for y in range(y0, y1+1):
            row = self.data[y]
            for x in range(x0, x1+1):
                row[x] = c
    def line(self, x0, y0, x1, y1, c):
        # line runs from (x0, y0)-(x1, y1)
        # this is not very smart about diagonal lines; sometimes it will miss the ends.  a better line algo would be welcome!
        if x0 == x1:
            for y in range(min(y0, y1), max(y0, y1) + 1):
                self.point(x0, y, c)
        elif y0 == y1:
            for x in range(min(x0, x1), max(x0, x1) + 1):
                self.point(x, y0, c)
        elif abs(x1 - x0) < abs(y1 - y0):
            # 'tall' line
            if y1 < y0:
                x0, x1 = x1, x0
                y0, y1 = y1, y0
            for y in range(y0, y1+1):
                x = (y - y0) * (x1 + 1 - x0) / (y1 + 1 - y0) + x0
                self.point(x, y, c)
        else:
            # 'wide' line
            if x1 < x0:
                x0, x1 = x1, x0
                y0, y1 = y1, y0
            for x in range(x0, x1+1):
                y = (x - x0) * (y1 + 1 - y0) / (x1 + 1 - x0) + y0
                self.point(x, y, c)
    def box(self, x0, y0, x1, y1, c):
        self.line(x0, y0, x1, y0, c)
        self.line(x0, y1, x1, y1, c)
        self.line(x0, y0, x0, y1, c)
        self.line(x1, y0, x1, y1, c)

class Matrix:
    def __init__(self, ips, width=25, height=12, frame_rate=0):
        self.ips = []
        for ip in ([ips] if is_string(ips) else ips):
            if ip.find(":") != -1:
                ip, port = ip.split(":", 1)
            else:
                port = 58082 # default matrix display port
            self.ips.append((ip, int(port)))
        self.width = width
        self.height = height
        self.frame_rate = frame_rate

        self.last = 0
        self.frame_no = 0

    def frame(self):
        return Frame(self.width, self.height)

    def show(self, f):
        now = time.time()
        print "since last:",now - self.last
        if (now - self.last) < (1.0 / self.frame_rate):
            to_sleep = 1.0 / self.frame_rate - (now - self.last)
            time.sleep(to_sleep)

        self.send_frame(f)
        self.frame_no += 1
        self.last = time.time()

    def send_frame(self, f):
        output = '\x01' + encode(f.data)
        print "sending %d bytes" % len(output)
        for dest_host in self.ips:
            sock.sendto(output, dest_host)

if __name__ == '__main__':
    mx = Matrix('127.0.0.1', 25, 12, 50)
    f = mx.frame()

    n = 0
    while 1:
        n += 1
        func = {
            0: f.line,
            1: f.rect,
            2: f.box,
            }[n % 3]
        #func = f.box
        func(random.randint(0, f.width-1), random.randint(0, f.height-1), random.randint(0, f.width-1), random.randint(0, f.height-1), random_color())
        mx.show(f)
        for x in range(10):
            mx.show(f)
        for x in range(20):
            f.dim(0.8)
            mx.show(f)
