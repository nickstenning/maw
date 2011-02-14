import time
import zmq

import pyglet
from pyglet.gl import *
from pyglet.window import key

from unicycle2d import Unicycle2D

window = pyglet.window.Window(1200, 400)

glEnable(GL_LINE_SMOOTH)
glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE)

# Enable Blending
glEnable(GL_BLEND)

# Specifies pixel arithmetic
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

@window.event
def on_key_press(symbol, modifiers):
    if symbol == key.SPACE:
        comm.send("rst")
        assert comm.recv() == "OK"
    elif symbol == key.G:
        comm.send("fw -20.0")
        assert comm.recv() == "OK"
    elif symbol == key.H:
        comm.send("fw 20.0")
        assert comm.recv() == "OK"
    elif symbol == key.T:
        comm.send("fp -20.0")
        assert comm.recv() == "OK"
    elif symbol == key.Y:
        comm.send("fp 20.0")
        assert comm.recv() == "OK"
    elif symbol == key.RIGHT:
        comm.send("tgt 0.157")
        assert comm.recv() == "OK"
    elif symbol == key.LEFT:
        comm.send("tgt -0.157")
        assert comm.recv() == "OK"
    elif symbol == key.ESCAPE:
        window.has_exit = True

@window.event
def on_key_release(symbol, modifiers):
    if symbol == key.G or symbol == key.H:
        comm.send("fw 0.0")
        assert comm.recv() == "OK"
    elif symbol == key.T or symbol == key.Y:
        comm.send("fp 0.0")
        assert comm.recv() == "OK"
    elif symbol == key.LEFT or symbol == key.RIGHT:
        comm.send("tgt 0.0")
        assert comm.recv() == "OK"

@window.event
def on_draw():
    window.clear()

    # Ground
    glColor3f(0.2, 0.2, 0.2)
    pyglet.graphics.draw(4, pyglet.gl.GL_QUADS,
        ('v2f', (0, 0, 0, window.height // 4 - unicycle.wheel.radius, window.width, window.height // 4 - unicycle.wheel.radius, window.width, 0))
    )

    unicycle.draw()

def update(dt):
    data.send(str(dt))
    state = data.recv().split()
    p  = float( state[0] )
    w  = float( state[1] )
    fp = float( state[2] )
    fw = float( state[3] )
    unicycle.update(p, w, fp, fw)

pyglet.clock.schedule_interval(update, 1/30.)

unicycle = Unicycle2D(window)

context = zmq.Context(1)

comm = context.socket(zmq.REQ)
comm.bind('tcp://127.0.0.1:5000')

data = context.socket(zmq.REQ)
data.bind('tcp://127.0.0.1:5001')

if __name__ == '__main__':
    pyglet.app.run()
