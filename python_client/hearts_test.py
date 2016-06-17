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


COLOUR_WHITE = [255,255,255]
COLOUR_BLACK = [0,0,0]

ANIM_01_FRAMES = [
[COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_WHITE, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK],
[COLOUR_BLACK, COLOUR_BLACK, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_BLACK, COLOUR_BLACK],
[COLOUR_BLACK, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_BLACK],
[COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE],
[COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_BLACK, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE],
[COLOUR_WHITE, COLOUR_WHITE, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_WHITE, COLOUR_WHITE],
[COLOUR_WHITE, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_WHITE],
[COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK],
]

def set_frame_1(hearts):
    hearts[3].fill(COLOUR_WHITE)
    

def set_frame(hearts, frame_data):
    """
    """
    idx = 0
    for h in hearts:
        h.fill(frame_data[idx])
        idx += 1

n = 0

anim_frame_idx = 0
while 1:
    n += 1
    print>>LOG, "OUT %s sent frame %d" % (time.time(), n)

    sq.dim(0.9)
    sq.point(randint(0, sq.width-1), randint(0, sq.height-1), random_color(brightness))


    anim_frame_idx = ( anim_frame_idx + 1) % len(ANIM_01_FRAMES)

    ##set_frame_1(hexs)
    set_frame(hexs, ANIM_01_FRAMES[anim_frame_idx])
    for h in hexs:
        ##h.fill(COLOUR_WHITE)
        h.dim(0.1)

    import time
    time.sleep(0.5)


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
