import socket
import time
import numpy as np
import colorsys

UDP_IP = "10.103.197.80"
UDP_PORT = 7890

rng = np.random.random_integers

led_count = 245

j = 1
sock = socket.socket(socket.AF_INET, # Internet
                 socket.SOCK_DGRAM) # UDP

def make_random_color():
    return [rng(0, 255), rng(0, 255), rng(0, 255)]

color1 = make_random_color()
color2 = make_random_color()
step = 10
while True:
    color1 = make_random_color()
    for i in xrange(0, led_count, step):
        data = [
            0x57, 0x04, # CMD
            0x00, 0x00, # OFFSET
        ]
        #h = ((i + j) * 4) % 256
        #(r, g, b) = colorsys.hsv_to_rgb(0.5 * ((i + j)% 2), 0.8, 0.8)
        for j in xrange(i):
            data += color1
        for j in xrange(i, led_count):
            data += color2
        sock.sendto(bytearray(data), (UDP_IP, UDP_PORT))
        time.sleep(0.1)
    color2 = color1
    time.sleep(0.5)
j = j + 5
