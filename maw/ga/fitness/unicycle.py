import math
import random

from maw.util import dirac_delta
from maw.unicycle import Unicycle
from maw.world_manager import WorldManager

DT              = 0.02
MAX_EVAL_TIME   = 100.0
YAW_SCORE_ANG   = math.pi / 2.0
PITCH_SCORE_ANG = math.pi / 4.0
ROLL_SCORE_ANG  = math.pi / 12.0

class Evaluator(object):

    def __init__(self):
        self.world = WorldManager()
        self.uni = Unicycle()

        self.uni.add_to_manager(self.world)

    def evaluate(self, brain):
        fitness = 0

        for _ in xrange(3):
            fitness += self.evaluate_once(brain)

        return fitness / 3

    def evaluate_once(self, brain):
        fitness = 0

        self.time = 0
        self.uni.reset()
        self.uni.compute_state()

        while (self.time < MAX_EVAL_TIME):
            self.step(brain)

            in_scoring_zone = abs(self.uni.yaw()) < YAW_SCORE_ANG and abs(self.uni.pitch()) < PITCH_SCORE_ANG and abs(self.uni.roll()) < ROLL_SCORE_ANG

            if in_scoring_zone:
                score = 0
                score += abs(self.uni.pitch())
                score += abs(self.uni.roll())
                score += abs(self.uni.yaw_velocity())
                score += abs(self.uni.wheel_velocity())

                fitness += DT * dirac_delta(score)
            else:
                break # Failure. No need to evaluate further.

        return fitness

    def step(self, brain, target_vel=0.0):

        input = [
            self.uni.yaw(),
            self.uni.pitch(),
            self.uni.roll(),
            self.uni.wheel_velocity(),
            self.uni.yaw_velocity()
        ]

        output = brain.feed(input)

        self.uni.apply_drive_impulse(output[0] + random.gauss(0, 0.1))
        self.uni.apply_wheel_impulse(output[1] + random.gauss(0, 0.1))

        self.world.step_simulation(DT)
        self.uni.compute_state()

        self.time += DT
