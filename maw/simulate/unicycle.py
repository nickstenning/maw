from maw.unicycle import Unicycle, DT
from maw.bindings.maw_simulator import simulate as _simulate

__all__ = ['simulate']

unicycle = Unicycle()

def simulate(nn):
    _simulate(DT, unicycle, nn, unicycle.wheel_impulse, unicycle.drive_impulse)
