import time
import zmq

from math import pi, sin, cos

import pyglet
from pyglet.window import key

BALL_IMAGE = 'media/ball.png'

class Pendulum(pyglet.sprite.Sprite):
    ball_image = pyglet.resource.image(BALL_IMAGE)
    dia = ball_image.width

    def __init__(self, ang=0):
        self.length = (window.height / 2.0) - self.dia - 10

        x = (window.width / 2.0) + self.length * sin(ang)
        y = (window.height / 2.0) + self.length * cos(ang)

        super(Pendulum, self).__init__(self.ball_image, x, y)

    def update(self, ang):
        self.x = (window.width / 2.0) + self.length * sin(ang)
        self.y = (window.height / 2.0) + self.length * cos(ang)

window = pyglet.window.Window(640, 480)

@window.event
def on_key_press(symbol, modifiers):
    if symbol == key.RIGHT:
        comm.send("torque 5.0")
        assert comm.recv() == "OK"

    elif symbol == key.LEFT:
        comm.send("torque -5.0")
        assert comm.recv() == "OK"

    elif symbol == key.ESCAPE:
        window.has_exit = True

@window.event
def on_key_release(symbol, modifiers):
    if symbol == key.RIGHT or symbol == key.LEFT:
        comm.send("torque 0.0")
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
