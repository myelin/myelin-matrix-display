import socket, random, time
sock = socket.socket( socket.AF_INET, # Internet
                      socket.SOCK_DGRAM ) # UDP

WIDTH = 25
HEIGHT = 12
PIXEL_SIZE = 3

def encode(data):
    output = []
    for row in data:
        for pixel in row:
            output.append(chr(pixel[0]) + chr(pixel[1]) + chr(pixel[2]))
    return ''.join(output)

def show(data):
    output = '\x01' + encode(data)
    sock.sendto(output, ("127.0.0.1", 58082))

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

def fill(c):
    return [[c for x in range(WIDTH)] for y in range(HEIGHT)]

def blank():
    return fill(BLACK)

def random_color():
    return (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

def multiply(c, factor):
    #print "multiply",c,factor
    return (int(c[0] * factor), int(c[1] * factor), int(c[2] * factor))

def fmultiply(c, factor):
    #print "multiply",c,factor
    return (float(c[0]) * factor, float(c[1]) * factor, float(c[2]) * factor)

def random_mask():
    return fmultiply(random_color(), 1.0 / 256)

# multiply a colour by a (0-1.0, 0-1.0, 0-1.0) RGB tuple
def cmultiply(c, mask):
    return (c[0] * mask[0], c[1] * mask[1], c[2] * mask[2])

def add(c1, c2):
    return (c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2])

def load(fn):
    pixels = open(fn, 'rb').read()
    assert len(pixels) == WIDTH * HEIGHT * PIXEL_SIZE
    data = []
    row_size = WIDTH * PIXEL_SIZE
    for y in range(HEIGHT):
        encoded_row = pixels[y * row_size:(y+1) * row_size]
        row = []
        for x in range(WIDTH):
            row.append((ord(encoded_row[x * PIXEL_SIZE]), ord(encoded_row[x * PIXEL_SIZE + 1]), ord(encoded_row[x * PIXEL_SIZE + 2])))
        data.append(row)
    return Frame(data)

class Frame:
    def __init__(self, data=None):
        self.width = WIDTH
        self.height = HEIGHT
        if data:
            self.data = data
        else:
            self.blank()
    def save(self, fn):
        open(fn, 'wb').write(encode(self.data))
    def copy(self, data):
        if isinstance(data, Frame): data = data.data
        self.data = [row[:] for row in data]
    def blank(self):
        self.data = blank()
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

class Flipper:
    def __init__(self, frame_rate):
        self.last = 0
        self.frame_rate = frame_rate
        self.frame = 0
    def flip(self, f):
        now = time.time()
        print "since last:",now - self.last
        if (now - self.last) < (1.0 / self.frame_rate):
            to_sleep = 1.0 / self.frame_rate - (now - self.last)
            time.sleep(to_sleep)
        if hasattr(f, 'show'):
            f.show()
        else:
            show(f)
        self.frame += 1
        self.last = time.time()

if __name__ == '__main__':
    flipper = Flipper(50)
    f = Frame()
    def flip():
        flipper.flip(f)
    n = 0
    while 1:
        n += 1
        func = {
            0: f.line,
            1: f.rect,
            2: f.box,
            }[n % 3]
        #func = f.box
        func(random.randint(0, WIDTH-1), random.randint(0, HEIGHT-1), random.randint(0, WIDTH-1), random.randint(0, HEIGHT-1), random_color())
        flip()
        for x in range(10):
            flip()
        for x in range(20):
            f.dim(0.8)
            flip()
