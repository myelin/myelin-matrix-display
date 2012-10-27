import random
import matrix
from PIL import Image

def main():
    pic = Image.open("lunacy_logo_small.png")
    print pic
    print pic.size

    flipper = matrix.Flipper(25)
    f = matrix.Frame()
    def flip():
        flipper.flip(f)

    def draw_offset_mult(offset, blend, mult, mask):
        for y in range(min(f.height, pic.size[1])):
            for x in range(min(f.width, pic.size[0])):
                pix = matrix.add(
                    matrix.cmultiply(matrix.multiply(pic.getpixel((x + offset, y))[:3], mult * (1-blend)), mask),
                    matrix.cmultiply(matrix.multiply(pic.getpixel((x + offset + 1, y))[:3], mult * blend), mask)
                    )
                f.point(x, y, pix)

    def fade(offset, blend, start, end, period):
        mask = matrix.random_mask()
        for pos in range(period):
            mult = float(pos)/period
            draw_offset_mult(offset, blend, start + (end - start) * mult, mask)
            flip()

    # offset where lunacy logo is touching the RHS of the display
    max_offset = pic.size[0] - f.width - 1

    def random_fade(frames):
        start = flipper.frame
        while (flipper.frame - start) < frames:
            offset = random.randint(0, max_offset)
    #        fade(offset, 0, 0.0, 1.0, flipper.frame_rate)
            fade(offset, 0, 1.0, 0.0, flipper.frame_rate / 2)

    def random_jitter(frames):
        start = flipper.frame
        while (flipper.frame - start) < frames:
            offset = random.randint(0, max_offset)
            draw_offset_mult(offset, 0, 1.0, matrix.random_mask())
            flip()

    def fadein(period):
        fade(0, 0, 0.0, 1.0, period)

    def fadeout(period):
        fade(max_offset, 0, 1.0, 0.0, period)

    def scroll_logo(frames):
        for frame in range(frames):
            pos = float(frame) / frames * max_offset
            offset = int(pos)
            blend = pos - offset
            #for offset in range(max_offset):
            #for _blend in range(10):
            #blend = float(_blend) / 10
            draw_offset_mult(offset, blend, 1.0, matrix.wheel_mask(flipper.frame % matrix.WHEEL_MAX))
            flip()

    def scroll_fade():
        for offset in range(0, max_offset, 5):
            fade(offset, 0, 1.0, 0.0, flipper.frame_rate / 2)

    while 1:
        scroll_fade()
        random_jitter(flipper.frame_rate * 2)
        scroll_logo(max_offset / 2)
        random_fade(flipper.frame_rate * 4)
        random_jitter(flipper.frame_rate * 2)


if __name__ == '__main__':
    main()

