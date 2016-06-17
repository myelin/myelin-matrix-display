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
tracelength = 3*37.0
mx = matrix.RawMatrix(['127.0.0.1', '192.168.0.82:6454'], 11, 4, frame_rate)
sq = mx.frame() # 8x8 square at the start of the chain
ring_segments = [mx.add_frame(matrix.RingFrame()) for i in range(4)]
hex_rgb_constants = [[0.2,0.9,0.9],[1,0.3,0.3],[0,0,1],[0.9,0,1]]

sq.dim(0)

n = 0
while 1:
    n += 1
    print>>LOG, "OUT %s sent frame %d" % (time.time(), n)

    multiplier = ((n / 20) % 2) * 0.03 + 0.95
    
    #sq.point(randint(0, sq.width-1), randint(0, sq.height-1), random_color(brightness))
    for i,s in enumerate(ring_segments):
        ring_brightness = brightness/2 + ((n % tracelength)/tracelength)*(brightness/2)
        s.linear_point(n % 37, [int(x * ring_brightness) for x in hex_rgb_constants[int((i + n/tracelength) % 4)]])
        s.dim(multiplier)

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
