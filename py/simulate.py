import time
import zmq

from math import pi, sin, cos, radians, degrees

from pyglet.gl import *
import pyglet
from pyglet.window import key

BOB_IMAGE = 'media/bob.png'

def center_anchor(img):
    img.anchor_x = img.width // 2
    img.anchor_y = img.height // 2

class PendulumBob(pyglet.sprite.Sprite):
    bob_image = pyglet.resource.image(BOB_IMAGE)
    center_anchor(bob_image)

    def __init__(self, x, y):
        super(PendulumBob, self).__init__(self.bob_image, x, y)

    def update(self, x, y):
        self.x = x
        self.y = y

class Pendulum(object):
    def __init__(self, ang=0):
        self.length = (window.height / 2.0) - 20

        self.bob = PendulumBob(0, self.length)

        glEnable(GL_LINE_SMOOTH)
        glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE)

        # Enable Blending
        glEnable(GL_BLEND)

        # Specifies pixel arithmetic
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

        self.update(ang)

    def update(self, ang):
        self.x = (window.width / 2.0) + self.length * sin(ang)
        self.y = (window.height / 2.0) + self.length * cos(ang)
        self.bob.update(self.x, self.y)

    def draw(self):
        glColor3f(0.4, 0.4, 0.4)
        glLineWidth(2.0)
        pyglet.graphics.draw(2, pyglet.gl.GL_LINES,
            ('v2f', (window.width / 2, window.height / 2, self.x, self.y))
        )
        self.bob.draw()

window = pyglet.window.Window(480, 480)

@window.event
def on_key_press(symbol, modifiers):
    if symbol == key.RIGHT:
        comm.send("torque 1")
        assert comm.recv() == "OK"

    elif symbol == key.LEFT:
        comm.send("torque -1")
        assert comm.recv() == "OK"

    elif symbol == key.ESCAPE:
        window.has_exit = True

@window.event
def on_key_release(symbol, modifiers):
    if symbol == key.RIGHT or symbol == key.LEFT:
        comm.send("torque 0")
        assert comm.recv() == "OK"

@window.event
def on_draw():
    window.clear()
    pendulum.draw()

def update(dt):
    data.send(str(dt))
    ang = float( data.recv() )
    pendulum.update(ang)

pyglet.clock.schedule_interval(update, 1/30.)

pendulum = Pendulum()

context = zmq.Context(1)

comm = context.socket(zmq.REQ)
comm.bind('tcp://127.0.0.1:5000')

data = context.socket(zmq.REQ)
data.bind('tcp://127.0.0.1:5001')

if __name__ == '__main__':

    pyglet.app.run()
