import sys, os.path, os, math, time
sys.path.insert(0, os.path.join(os.path.split(__file__)[0], '..', 'python_client'))
import matrix
import kb

def main():
    print """arrow keys - move around
space - toggle current pixel
w/a/s/d - shift entire canvas"""

    if len(sys.argv) > 1:
        fn = sys.argv[1]
    else:
        fn = 'image.mx'

    FRAME_RATE = 50
    x = y = 0
    if os.path.exists(fn):
        image = matrix.load(fn)
    else:
        image = matrix.Frame()
    f = matrix.Frame()
    frame = 0
    input = []

    kb.capture() # switch to raw mode
    while 1:
        frame += 1

        while 1:
            c = kb.read(blocking=0)
            if c is None:
                break
            input.append(c)
        if len(input):
            #print "%s\r" % `input`
            c = input.pop(0)
            if c == chr(27):
                if not len(input):
                    print "exit\r"
                    break
                if input.pop(0) == '[':
                    c = input.pop(0)
                    if c == 'A':
                        print "up\r"
                        y = (y + f.height - 1) % f.height
                    elif c == 'B':
                        print "down\r"
                        y = (y + 1) % f.height
                    elif c == 'C':
                        print "right\r"
                        x = (x + 1) % f.width
                    elif c == 'D':
                        print "left\r"
                        x = (x + f.width - 1) % f.width
                    else:
                        print "unknown magic: %s\r" % c
                else:
                    print "unknown special\r"
            elif c == ' ':
                image.point(x, y, matrix.BLACK if image.get(x, y) != matrix.BLACK else matrix.WHITE)
                image.save(fn)
            elif c.lower() == 'w': # roll up
                image.translate(0, -1)
            elif c.lower() == 'a':
                image.translate(-1, 0)
            elif c.lower() == 's':
                image.translate(0, 1)
            elif c.lower() == 'd':
                image.translate(1, 0)
            else:
                print c,"\r"

        f.copy(image)
        # draw cursor
        f.point(x, y, matrix.multiply(matrix.WHITE, 0.5 * math.sin(float(frame) / FRAME_RATE * math.pi) + 0.5))
        f.show()
        time.sleep(1.0 / FRAME_RATE)

if __name__ == '__main__':
    try:
        main()
    finally:
        kb.restore()
