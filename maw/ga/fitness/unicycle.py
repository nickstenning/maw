import math

from maw.util import dirac_delta
from maw.unicycle import Unicycle
from maw.world_manager import WorldManager

DT              = 0.02
YAW_BANG_SIZE   = 2.0
PITCH_BANG_SIZE = 2.0
MAX_EVAL_TIME   = 100.0
YAW_SCORE_ANG   = 9.0 * math.pi / 10.0
PITCH_SCORE_ANG = math.pi / 12.0
ROLL_SCORE_ANG  = math.pi / 12.0

class Evaluator(object):

    def __init__(self):
        self.world = WorldManager()
        self.uni = Unicycle()

        self.uni.addToManager(self.world)

    def evaluate(self, brain):
        fitness = 0

        self.time = 0
        self.uni.reset()
        self.uni.computeState()

        while (self.time < MAX_EVAL_TIME):
            self.step(brain)

            in_scoring_zone = abs(self.uni.yaw()) < YAW_SCORE_ANG and abs(self.uni.pitch()) < PITCH_SCORE_ANG and abs(self.uni.roll()) < ROLL_SCORE_ANG

            if in_scoring_zone:
                score = dirac_delta(self.uni.pitch(), 5) + dirac_delta(self.uni.roll(), 5)
                fitness += DT * score;
            else:
                break # Failure. No need to evaluate further.

        return fitness

    def step(self, brain):
        input = [self.uni.yaw(), self.uni.pitch(), self.uni.roll(), self.uni.wheelVelocity()]

        output = brain.feedForward(input)

        yaw_impulse = YAW_BANG_SIZE * output[0]
        pitch_impulse = PITCH_BANG_SIZE * output[1]

        self.uni.applyForkImpulse(yaw_impulse)
        self.uni.applyWheelImpulse(pitch_impulse)

        self.world.stepSimulation(DT)
        self.uni.computeState()

        self.time += DT