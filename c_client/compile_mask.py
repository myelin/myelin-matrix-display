import sys

fn, = sys.argv[1:]

lines = [x for x in [line.strip() for line in open(fn).read().split("\n")] if x]
width = len(lines[0])
height = len(lines)
for line in lines: assert len(line) == width, "mismatched widths"

print "%d x %d" % (width, height)
