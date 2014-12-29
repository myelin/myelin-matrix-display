from PIL import Image

def main():
    pic = Image.open("lunacy_logo_small.png")
    print pic
    print pic.size

    name = 'lunacy_logo_mask'

    data = pic.getdata()
    bytes = []
    for i in range(0, len(data), 8):
        c = 0
        for bit in range(8):
            value = (data[i+bit] if (i+bit < len(data)) else (0, 0, 0))
            b = 1 if (value[0] + value[1] + value[2]) > 4 * 3 else 0
            #print value, b
            c <<= 1
            c |= b
        bytes.append("0x%02x" % c)

    f = open("%s.inc" % name, 'wt')
    print>>f, "#define %s_width %d" % (name, pic.size[0])
    print>>f, "#define %s_height %d" % (name, pic.size[1])
    print>>f, "const uint8_t %s_data[] = { %s };" % (name, ", ".join(bytes))

if __name__ == '__main__':
    main()

