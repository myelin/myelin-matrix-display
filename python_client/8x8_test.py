import random
import matrix

mx = matrix.Matrix(['10.20.0.53:6454'], 8, 8, 50)
f = mx.frame()

mx2 = matrix.Matrix(['10.20.1.169:6454'], 8, 8, 50)
f2 = mx.frame()

n = 0
while 1:
    n += 1

    f2.dim(0.7)
    func = {
        0: f2.line,
        1: f2.rect,
        2: f2.box,
        }[n % 3]
    func(random.randint(0, f2.width-1), random.randint(0, f2.height-1), random.randint(0, f2.width-1), random.randint(0, f2.height-1), matrix.random_color())
    mx2.show(f2)

    f.dim(0.7)
    f.point(random.randint(0, 7), random.randint(0, 7), matrix.random_color())
    mx.show(f)
