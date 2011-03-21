from maw.unicycle import Unicycle
from maw.bindings.maw_simulator import simulate as _simulate
from maw.ga.fitness.unicycle import YAW_BANG_SIZE, PITCH_BANG_SIZE

__all__ = ['simulate']

unicycle = Unicycle()

def simulate(nn, yaw_bang_size=YAW_BANG_SIZE, pitch_bang_size=PITCH_BANG_SIZE):
    _simulate(unicycle, nn, yaw_bang_size, pitch_bang_size)
