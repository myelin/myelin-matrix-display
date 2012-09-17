import socket, random
sock = socket.socket( socket.AF_INET, # Internet
                      socket.SOCK_DGRAM ) # UDP

WIDTH = 25
HEIGHT = 12

def display(data):
    output = ['\x01']
    for row in data:
        for pixel in row:
            output.append(chr(pixel[0]) + chr(pixel[1]) + chr(pixel[2]))
    sock.sendto(''.join(output), ("192.168.1.99", 58082))

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

def fill(c):
    return [[c for x in range(WIDTH)] for y in range(HEIGHT)]

def blank():
    return fill(BLACK)

def random_color():
    return (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

if __name__ == '__main__':
    display(fill(random_color()))
