import socket
import time
import numpy as np
import colorsys

UDP_IP = "10.103.197.80"
UDP_PORT = 7890

rng = np.random.random_integers

led_count = 245

sock = socket.socket(socket.AF_INET, # Internet
                 socket.SOCK_DGRAM) # UDP

def make_color():
    return [255, 255, 255]

data = [
    0x57, 0x04, # CMD
    0x00, 0x00, # OFFSET
]
for i in xrange(led_count):
    data += make_color()
sock.sendto(bytearray(data), (UDP_IP, UDP_PORT))
