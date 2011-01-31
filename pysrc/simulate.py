import time
import zmq

import pyglet
from pyglet.window import key

from unicycle2d import Unicycle2D

window = pyglet.window.Window(1200, 480)

@window.event
def on_key_press(symbol, modifiers):
    if symbol == key.SPACE:
        comm.send("rst")
        assert comm.recv() == "OK"
    elif symbol == key.ESCAPE:
        window.has_exit = True

@window.event
def on_draw():
    window.clear()
    unicycle.draw()

def update(dt):
    data.send(str(dt))
    state = data.recv().split()
    p = float( state[0] )
    w = float( state[1] )
    print p, w
    unicycle.update(p, w)

pyglet.clock.schedule_interval(update, 1/30.)

unicycle = Unicycle2D(window)

context = zmq.Context(1)

comm = context.socket(zmq.REQ)
comm.bind('tcp://127.0.0.1:5000')

data = context.socket(zmq.REQ)
data.bind('tcp://127.0.0.1:5001')

if __name__ == '__main__':
    pyglet.app.run()
