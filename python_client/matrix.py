import errno, socket, random, time, six, traceback
sock = socket.socket( socket.AF_INET, # Internet
                      socket.SOCK_DGRAM ) # UDP

PIXEL_SIZE = 3

def encode(data):
#    print `data`
    output = []
    for row in data:
        for pixel in row:
            output.append(chr(pixel[0]) + chr(pixel[1]) + chr(pixel[2]))
    r = ''.join(output)
#    print len(r)
    return r

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

def is_string(s):
    return isinstance(s, six.string_types)

def random_color(brightness=255):
    return (random.randint(0, brightness), random.randint(0, brightness), random.randint(0, brightness))

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

    def __init__(self):
        self.blank()

    def blank(self):
        self.fill(BLACK)

    def fill(self, c):
        self.data = self.make_fill(c)

    def get(self, x, y):
        return self.data[y][x]

    def point(self, x, y, c):
        #print "point %d, %d" % (x, y)
        self.data[y][x] = c

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

def coord_mapping(row_lengths):
    mapping = {}
    n = 0
    for y in range(len(row_lengths)):
        for x in range(row_lengths[y]):
            mapping[n] = (x, y)
            n += 1
    print mapping
    return mapping

#    0      0 1 2 3
#    1     0 1 2 3 4
#    2    0 1 2 3 4 5
#    3   0 1 2 3 4 5 6
#    4    0 1 2 3 4 5
#    5     0 1 2 3 4
#    6      0 1 2 3


class HeartFrame(Frame):
    """
    """

    def make_fill(self, c):
        return [
            [c] * 24,
            ]


class HexagonFrame(Frame):
    ROW_LENGTHS = [4, 5, 6, 7, 6, 5, 4]

    ID_MAPPING = coord_mapping(ROW_LENGTHS)

    SPIRAL_MAPPING = {
        0: (0, 0),
        1: (1, 0),
        2: (2, 0),
        3: (3, 0),
        4: (4, 1),
        5: (5, 2),
        6: (6, 3),
        7: (5, 4),
        8: (4, 5),
        9: (3, 6),
        10: (2, 6),
        11: (1, 6),
        12: (0, 6),
        13: (0, 5),
        14: (0, 4),
        15: (0, 3),
        16: (0, 2),
        17: (0, 1),
        18: (1, 1),
        19: (2, 1),
        20: (3, 1),
        21: (4, 2),
        22: (5, 3),
        23: (4, 4),
        24: (3, 5),
        25: (2, 5),
        26: (1, 5),
        27: (1, 4),
        28: (1, 3),
        29: (1, 2),
        30: (2, 2),
        31: (3, 2),
        32: (4, 3),
        33: (3, 4),
        34: (2, 4),
        35: (2, 3),
        36: (3, 3),
        }

    def __init__(self):
        self.blank()

    def linear_point(self, x, c):
        x, y = HexagonFrame.ID_MAPPING[x]
        self.point(x, y, c)

    def spiral_point(self, x, c):
        x, y = HexagonFrame.SPIRAL_MAPPING[x]
        self.point(x, y, c)

    def make_fill(self, c):
        return [
            [c] * 4,
            [c] * 5,
            [c] * 6,
            [c] * 7,
            [c] * 6,
            [c] * 5,
            [c] * 4,
            ]

    def ring(self, r, c):
        # r=0: center
        # r=3: outer ring
        if r == 0:
            self.point(3, 3, c)
        elif r == 1:
            for y in (2, 4):
                for x in (2, 3): self.point(x, y, c)
            for x in (2, 4): self.point(x, 3, c)
        elif r == 2:
            for y in (1, 5):
                for x in (1, 2, 3): self.point(x, y, c)
            for y in (2, 4):
                for x in (1, 4): self.point(x, y, c)
            for x in (1, 5): self.point(x, 3, c)
        elif r == 3:
            for y in (0, 6):
                for x in (0, 1, 2, 3): self.point(x, y, c)
            for y in (1, 5):
                for x in (0, 4): self.point(x, y, c)
            for y in (2, 4):
                for x in (0, 5): self.point(x, y, c)
            for x in (0, 6): self.point(x, 3, c)
        else:
            raise Exception("bad radius %d" % r)

class RingFrame(Frame):
    ROW_LENGTHS = [11, 10, 9, 7]

    ID_MAPPING = coord_mapping(ROW_LENGTHS)

    def __init__(self):
        self.blank()

    def linear_point(self, x, c):
        x, y = RingFrame.ID_MAPPING[x]
        self.point(x, y, c)

    def make_fill(self, c):
        return [
            [c] * 11,
            [c] * 10,
            [c] * 9,
            [c] * 7,
            ]

class RectangleFrame(Frame):
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

    def translate(self, xd, yd):
        self.data = [
            [
                self.data[(y - yd + self.height) % self.height][(x - xd + self.width) % self.width]
                for x in range(self.width)
            ] for y in range(self.height)
        ]

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
    def __init__(self, ips, width=None, height=None, frame_rate=30):
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

        self.chain = []
        if self.width and self.height:
            self.chain.append(RectangleFrame(self.width, self.height))

    def frame(self, idx=0):
        return self.chain[idx]

    def add_frame(self, f):
        self.chain.append(f)
        return f

    def show(self):
        now = time.time()
        print "[%d] since last: %.6f" % (self.frame_no, now - self.last)
        if (now - self.last) < (1.0 / self.frame_rate):
            to_sleep = 1.0 / self.frame_rate - (now - self.last)
            time.sleep(to_sleep)

        self.send_frame()
        self.frame_no += 1
        self.last = time.time()

    def format_packet(self, data):
        return '\x01' + data

    def send_frame(self):
        output = self.format_packet(''.join(encode(f.data) for f in self.chain))
        #print "sending %d bytes" % len(output)
        for dest_host in self.ips:
            try:
                sock.sendto(output, dest_host)
            except socket.error, e:
                if e.errno == errno.EHOSTDOWN:
                    print "\tEHOSTDOWN sending to %s" % `dest_host`
                else:
                    traceback.print_exc()

class RawMatrix(Matrix):
    def format_packet(self, data):
        return data

if __name__ == '__main__':
    mx = Matrix('127.0.0.1', 25, 12, 30)
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
        mx.show()
        for x in range(10):
            mx.show()
        for x in range(20):
            f.dim(0.8)
            mx.show()
