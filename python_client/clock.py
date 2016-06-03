import matrix
from PIL import Image, ImageFont, ImageDraw
import time

def m(c):
    block = 40
    if c < block: return 0
    if c < block * 2: return 1
    if c < block * 3: return 2
    if c < block * 4: return 3
    if c < block * 5: return 4
    return c
def map_colour(c):
    return c
    return (m(c[0]), m(c[1]), m(c[2]))

mx = matrix.Matrix('127.0.0.1', 25, 12, 50)
f = mx.frame()

image = Image.new("RGBA", (25, 12))
draw = ImageDraw.Draw(image)
font = ImageFont.truetype("fonts/Clubland.ttf", 16)

while 1:
    s = time.strftime("%H:%M")
    draw.rectangle(((0, 0), (25, 12)), fill=(0, 0, 0))
    draw.text((0, -4), s, font=font)
    for y in range(12):
        for x in range(25):
            #print image.getpixel((x, y)),
            f.point(x, y, map_colour(image.getpixel((x, y))[:3]))
        #print
    c = int(time.strftime("%S"))
    print c
    f.point(0, 0, (c, c, c))
    mx.show(f)
