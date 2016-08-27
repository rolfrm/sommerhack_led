import socket
import time
import numpy as np
import colorsys
import sys
import coordinates as coords

if len(sys.argv) < 2:
    print "Please provide coordinate file"
    sys.exit(-1)

points = coords.read_coordinates_file(sys.argv[1])

#UDP_IP = '139.59.210.226'
#UDP_PORT = 32000
UDP_IP = "10.103.197.80"
UDP_PORT = 7890

irng = np.random.random_integers
rng = np.random.random

led_count = 245

sock = socket.socket(socket.AF_INET, # Internet
                 socket.SOCK_DGRAM) # UDP

icol = 0
def make_random_color():
    #return [irng(0, 255), irng(0, 255), irng(0, 255)]
    global icol
    icol = icol + 1
    if icol % 3 == 0:
        return [255, 0, 0]
    elif icol % 3 == 1:
        return [0, 255, 0]
    else:
        return [0, 0, 255]

def spawn_circle():
    return (rng(), rng(), 0, make_random_color())

bg_color = [0, 0, 0]
color_points = map(lambda x: bg_color, points)
circles = [spawn_circle()]
dt = 0

np_points = np.array(points)
np_color = np.array(color_points)

def send_color():
    data = [
        0x57, 0x04, # CMD
        0x00, 0x00, # OFFSET
    ] + sum(np_color.tolist(), [])
    sock.sendto(bytearray(data), (UDP_IP, UDP_PORT))

step = 0.15
j = 0
while True:
    s = time.time()
    # evolve circles
    for i, c in enumerate(circles):
        (x, y, r, color) = c
        circles[i] = (x, y, (r + 0.2 * step), color)

    for c in circles:
        (x, y, r, color) = c
        diff = np_points - np.array([x, y])
        diff[:, 1] = diff[:, 1] * 0.2
        diff = np.linalg.norm(diff, axis = 1)
        in_circle = diff < r
        np_color[in_circle, 0] = color[0]
        np_color[in_circle, 1] = color[1]
        np_color[in_circle, 2] = color[2]

    (x, y, r, color) = circles[0]
    if r > 1:
        circles = circles[1:]
    send_color()
    dt = time.time() - s
    time.sleep(max(0, step - dt))
    j += 1
    if j > 5:
        j = 0
        circles.append(spawn_circle())
