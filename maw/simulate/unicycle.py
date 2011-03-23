from maw.unicycle import Unicycle
from maw.bindings.maw_simulator import simulate as _simulate

__all__ = ['simulate']

unicycle = Unicycle()

def simulate(nn):
    _simulate(unicycle, nn)
