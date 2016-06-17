# this script outputs random stuff to seven 36-led hexagons attached to an 8x8 matrix

TARGET_IP_ADDRESS = '192.168.0.80'

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
brightness = 32
mx = matrix.RawMatrix(['127.0.0.1', '%s:6454' % TARGET_IP_ADDRESS], 8, 8, frame_rate)
sq = mx.frame() # 8x8 square at the start of the chain
hexs = [mx.add_frame(matrix.HeartFrame()) for i in range(7)]

n = 0
while 1:
    n += 1
    print>>LOG, "OUT %s sent frame %d" % (time.time(), n)

    sq.dim(0.9)
    sq.point(randint(0, sq.width-1), randint(0, sq.height-1), random_color(brightness))
    for h in hexs:
        h.dim(0.9)
#    hexs[randint(0, 5)].ring(randint(0, 3), random_color(brightness))
#    hexs[6].spiral_point(n % 37, [brightness, brightness, brightness])
    #hexs[6].fill([255, 255, 255])
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
