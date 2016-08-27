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

def make_random_color():
    return [rng(0, 255), rng(0, 255), rng(0, 255)]

step = 10
while True:
    data = [
        0x57, 0x04, # CMD
        0x00, 0x00, # OFFSET
    ]
    for i in xrange(led_count):
        data += make_random_color()
    sock.sendto(bytearray(data), (UDP_IP, UDP_PORT))
    time.sleep(0.15)
