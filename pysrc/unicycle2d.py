from math import pi, sin, cos, radians, degrees

import pyglet
from pyglet.gl import *

WHEEL_IMAGE = 'media/wheel_overlay.png'
SEAT_IMAGE = 'media/seat.png'

def center_anchor(img):
    img.anchor_x = img.width // 2
    img.anchor_y = img.height // 2

class Seat(pyglet.sprite.Sprite):
    seat_image = pyglet.resource.image(SEAT_IMAGE)
    center_anchor(seat_image)

    def __init__(self, x=0, y=0):
        super(Seat, self).__init__(self.seat_image, x, y)

    def update(self, x, y, rotation):
        self.x = x
        self.y = y
        self.rotation = rotation

class Wheel(object):
    def __init__(self, radius, x=0, y=0, rotation=0):
        self.radius = radius
        self.update(x, y, rotation)

    def update(self, x, y, rotation):
        self.x = x
        self.y = y
        self.rotation = rotation

    def draw(self):
        glBegin(GL_TRIANGLE_FAN)
        glVertex2f(self.x, self.y)

        self._draw_ang_range(0.4, 0, 90)
        self._draw_ang_range(0.8, 90, 180)
        self._draw_ang_range(0.4, 180, 270)
        self._draw_ang_range(0.8, 270, 360)

        glEnd()

    def _draw_ang_range(self, shade, from_ang, to_ang):
        glColor3f(shade, shade, shade)
        ang = from_ang
        while ang <= to_ang:
            glVertex2f(
                self.x + self.radius * sin(radians(ang + self.rotation)),
                self.y + self.radius * cos(radians(ang + self.rotation))
            )
            ang += 5

class Unicycle2D(object):
    post_length  = 1.0
    wheel_radius = 0.2

    def __init__(self, window, p=0, w=0):
        self.origin_x = window.width // 2
        self.origin_y = window.height // 4
        self.scale_factor = window.height // 2

        self.wheel = Wheel(self.scale_factor * self.wheel_radius)
        self.seat = Seat()

        glEnable(GL_LINE_SMOOTH)
        glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE)

        # Enable Blending
        glEnable(GL_BLEND)

        # Specifies pixel arithmetic
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

        self.update(p, w)

    def update(self, p, w, fw=0, fp=0):
        self.x = self.origin_x + self.scale_factor * self.wheel_radius * w
        self.y = self.origin_y

        self.seat_x = self.x + self.scale_factor * self.post_length * sin(p)
        self.seat_y = self.y + self.scale_factor * self.post_length * cos(p)

        self.fp = fp
        self.fw = fw

        self.wheel.update(self.x, self.y, degrees(w))
        self.seat.update(self.seat_x, self.seat_y, degrees(p))

    def draw(self):
        self.wheel.draw()

        # Seatpost
        glColor3f(0.4, 0.4, 0.4)
        glLineWidth(2.0)
        pyglet.graphics.draw(2, pyglet.gl.GL_LINES,
            ('v2f', (self.x, self.y, self.seat_x, self.seat_y))
        )

        # Forces
        glColor3f(0.5, 0.1, 0.1)
        glLineWidth(2.0)
        pyglet.graphics.draw(2, pyglet.gl.GL_LINES,
            ('v2f', (self.x, self.y, self.x + 10 * self.fw, self.y))
        )

        self.seat.draw()