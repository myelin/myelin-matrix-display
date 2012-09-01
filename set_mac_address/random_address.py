import re, random

fn = "set_mac_address.ino"
code = open(fn).read()
a, b = re.search("^(.*?MY_ADDRESS 0x)[A-Fa-f0-9]+(LL.*)$", code, re.S).groups()

addr = []
while len(addr) < 5:
    c = random.randint(0, 255)
    if not len(addr) and c in (0x55, 0xAA): continue
    if c in (0, 0xFF): continue
    addr.append(c)
addr = "".join("%02X" % c for c in addr)

new_code = "%s%s%s" % (a, addr, b)

assert len(new_code) == len(code)

open("%s~" % fn, "wt").write(code)
open(fn, "wt").write(new_code)

print>>open("addresses.txt", "at"), "0x%s" % addr
