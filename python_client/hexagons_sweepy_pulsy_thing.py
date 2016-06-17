# this script outputs random stuff to seven 36-led hexagons attached to an 8x8 matrix

import matrix
from matrix import random_color

import errno
from random import randint
import socket
import time
import traceback

LOG = open("hexagons.log", "w")
matrix.sock.setblocking(0)

frame_rate = 20
brightness = 20
tracelength = 96.0
mx = matrix.RawMatrix(['127.0.0.1', '192.168.0.81:6454'], 8, 8, frame_rate)
sq = mx.frame() # 8x8 square at the start of the chain
hexs = [mx.add_frame(matrix.HexagonFrame()) for i in range(7)]
hex_rgb_constants = [[0.2,0.9,0.9],[0.8,0.8,0.3],[0.3,0.7,0.0],[1,0.3,0.3],[0,0,1],[1,1,1],[1,0,1]]

sq.dim(0)

n = 0
while 1:
    n += 1
    print>>LOG, "OUT %s sent frame %d" % (time.time(), n)

    #sq.point(randint(0, sq.width-1), randint(0, sq.height-1), random_color(brightness))
    for i,h in enumerate(hexs):
        hex_brightness = brightness/2 + (i/6.0 + (n % tracelength)/tracelength)*(brightness/2)
        h.spiral_point(n % 37, [int(x * hex_brightness) for x in hex_rgb_constants[int((i + n/tracelength) % 7)]])

    mx.show()
    try:
        s = matrix.sock.recvfrom(4096)
        print `s`
        print>>LOG, "IN %s" % `s`
    except socket.error, e:
        if e.errno == errno.EAGAIN:
            print "\tno data to rx"
        else:
            raise
        #print "socket err", e, e.errno
    except Exception, e:
        print "unknown exc",e
        traceback.print_exc()
