# listen for udp packets and forward them to the avr's ACM usb serial port

import serial, os, socket, fcntl, traceback, time

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
port = serial.Serial('/dev/ttyACM0')

n_pixels = 300
n_pixel_bytes = n_pixels * 3

#sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
sock.bind(('0.0.0.0', 58082))
#fcntl.fcntl(sock, fcntl.F_SETFL, os.O_NONBLOCK)

#data = [random.randint(0, 255) for x in range(n_pixel_bytes)]
def send_frame_over_serial(data):
    assert len(data) == n_pixel_bytes
    port.write("*#" + data)
    port.flush()
    #' '.join(chr(c) for c in data))

while 1:
    print "recv"
    recv_start = time.time()
    data, source = sock.recvfrom(1024)
    recv_time = time.time() - recv_start
    print "\tgot %d bytes; waited %.2f s" % (len(data), recv_time)
    if len(data) == n_pixel_bytes + 1:
        frame_time_start = time.time()
        send_frame_over_serial(data[1:])
        frame_time = time.time() - frame_time_start
        print "took %.2f s to send frame" % frame_time
        if frame_time > 0.1:
            print "that was too long; resetting serial port"
            port.close()
            port.open()
            
