# this script outputs random frames to two 8x8 matrixes (with hardcoded ip addresses)

import random
import matrix

mx = matrix.Matrix(['192.168.0.84:6454'], 8, 8, 50)
f = mx.frame()

n = 0
f.dim(0)
f.subtract(-100)
while 1:
    n += 1
    multiplier = ((n / 20) % 2) * 0.03 + 0.99
    print multiplier
    f.dim(multiplier)
#    f.dim(0.7)
#    f.point(random.randint(0, 7), random.randint(0, 7), matrix.random_color())
    mx.show()
