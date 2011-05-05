import math
import random

from maw.util import dirac_delta
from maw.unicycle import Unicycle
from maw.world_manager import WorldManager

DT              = 0.02
MAX_EVAL_TIME   = 100.0
YAW_SCORE_ANG   = math.pi - 0.2
PITCH_SCORE_ANG = math.pi / 6.0
ROLL_SCORE_ANG  = math.pi / 6.0
# MAX_WHEELVEL = 3.0

class Evaluator(object):

    def __init__(self):
        self.world = WorldManager()
        self.uni = Unicycle()

        self.uni.add_to_manager(self.world)

    def evaluate(self, brain):
        fitness = 0

        self.time = 0
        self.uni.reset(0.1)

        while (self.time < MAX_EVAL_TIME):
            self.step(brain)

            in_pitch_threshold = abs(self.uni.pitch()) < PITCH_SCORE_ANG
            in_roll_threshold = abs(self.uni.roll()) < ROLL_SCORE_ANG
            # in_wheelvel_threshold = abs(self.uni.wheel_velocity()) < MAX_WHEELVEL

            if in_pitch_threshold and in_roll_threshold: #and in_wheelvel_threshold:
                score = 0
                score += 0.1 * abs(self.uni.pitch())
                score += 0.1 * abs(self.uni.roll())
                score += abs(self.uni.yaw_velocity())
                score += 0.01 * abs(self.uni.kinetic_energy())

                fitness += DT * dirac_delta(score)
            else:
                break # Failure. No need to evaluate further.

        return fitness

    def step(self, brain):

        input = [
            self.uni.pitch(),
            self.uni.roll(),
            self.uni.wheel_velocity(),
            self.uni.yaw_velocity(),
            self.uni.pitch_velocity(),
            self.uni.roll_velocity(),
        ]

        output = brain.feed(input)

        self.uni.apply_drive_impulse(
            self.uni.drive_impulse * output[0] #+ random.gauss(0, 0.5)
        )
        self.uni.apply_wheel_impulse(
            self.uni.wheel_impulse * output[1] #+ random.gauss(0, 0.5)
        )

        self.world.step_simulation(DT)
        self.uni.compute_state(DT)
        self.time += DT

        # toprint = [
        #     self.time,
        #     self.uni.yaw(), self.uni.pitch(), self.uni.roll(),
        #     self.uni.yaw_velocity(), self.uni.pitch_velocity(), self.uni.roll_velocity(),
        #     self.uni.wheel_velocity(),
        #     self.uni.kinetic_energy(), self.uni.potential_energy()
        # ]

        # print "\t".join(map(str, toprint))
