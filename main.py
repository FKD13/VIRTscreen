import time

import websocket

"""
xrandr --fb 2320x1080 --output eDP-1 --panning 1920x1080/2320x1080
xrandr --setmonitor virtual2 400/340x300/276+1920+0 none
"""

# dev = sys.argv[1]
# dither = int(sys.argv[2])
# brightness = int(sys.argv[3])
offset_x = 1920  # int(sys.argv[4])
offset_y = 0  # int(sys.argv[5])
w = 400
h = 300

from Xlib import display, X
from PIL import Image
from websocket import create_connection
from pyxcursor import Xcursor
from multiprocessing import Pool
from functools import partial

d = display.Display()
root = d.screen().root
cursor = Xcursor()

cb = 10  # cursor boundary
iarray, xhot, yhot = cursor.getCursorImageArrayFast()
cimg = Image.fromarray(iarray)


def getImage():
    #s = time.time()
    raw = root.get_image(offset_x, offset_y, w, h, X.ZPixmap, 0xffffffff)
    # for b in raw.data:
    #    print(b)

    _img = Image.frombytes("RGB", (w, h), raw.data, "raw", "BGRX")

    pointer = root.query_pointer()
    px, py = pointer.root_x, pointer.root_y
    if offset_x - cb <= px <= offset_x + w + cb and offset_y - cb <= py < offset_y + h + cb:
        _img.paste(cimg, (px - offset_x - xhot, py - offset_y - yhot), cimg)

    #print(time.time() - s)
    return _img


# There is no way to simply set the pointer, we have to reconfigure the draw area
# There is an eight? byte cost to setting the draw area, plus setting it back afterwards


cost = 8

oldImg = getImage()

ws: websocket.WebSocket = create_connection('ws://10.1.0.198:8000/set_pixel')

coords = [(x, y) for x in range(w) for y in range(h)]


def do(img, coord):
    x, y = coord
    pixel = img.getpixel((x, y))
    if oldImg.getpixel((x, y)) != pixel:
        r, g, b = pixel
        payload = x.to_bytes(4, 'big') + y.to_bytes(4, 'big') + bytearray([r, g, b])
        #ws.send(f'{x} {y} {r} {g} {b}')
        #print(payload)
        ws.send_binary(payload)


while True:
    img = getImage()

    #t = time.time()

    for coord in coords:
        do(img, coord)

    #print(time.time() - t)

    oldImg = img
