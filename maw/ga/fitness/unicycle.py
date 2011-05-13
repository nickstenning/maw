import math
import random

from maw.util import dirac_delta
from maw.unicycle import Unicycle, DT
from maw.world_manager import WorldManager

MAX_EVAL_TIME   = 100.0
YAW_SCORE_ANG   = math.pi - 0.1
PITCH_SCORE_ANG = math.pi / 10.0
ROLL_SCORE_ANG  = math.pi / 10.0

class Evaluator(object):

    def __init__(self):
        self.world = WorldManager()
        self.uni = Unicycle()

        self.uni.add_to_manager(self.world)

    def evaluate(self, brain):
        fitness = 0

        self.time = 0
        self.uni.reset(0.05)

        while (self.time < MAX_EVAL_TIME):
            self.step(brain)

            in_pitch_threshold = abs(self.uni.pitch()) < PITCH_SCORE_ANG
            in_roll_threshold = abs(self.uni.roll()) < ROLL_SCORE_ANG
            in_yaw_threshold = abs(self.uni.yaw()) < YAW_SCORE_ANG
            in_pos_threshold = abs(self.uni.x()) < 2.5 and abs(self.uni.z()) < 2.5

            if in_pitch_threshold and in_roll_threshold and in_yaw_threshold and in_pos_threshold:
                fitness += 0.7 * DT * dirac_delta(self.uni.yaw(), 0.8)
                fitness += 0.3 * DT * dirac_delta(abs(self.uni.x()) + abs(self.uni.z()))
            else:
                break # Failure. No need to evaluate further.

        return fitness

    def step(self, brain):

        input = [
            self.uni.pitch(),
            self.uni.pitch_velocity(),
            self.uni.roll(),
            self.uni.roll_velocity(),
        ]

        output = brain.feed(input)

        self.power_used = abs(output[0]) + abs(output[1])

        # (-1 if self.uni.roll() < 0 else 1)
        # (-1 if self.uni.pitch() < 0 else 1)

        self.uni.apply_drive_impulse(
            self.uni.drive_impulse * output[0] #* random.gauss(1, 0.2)
        )
        self.uni.apply_wheel_impulse(
            self.uni.wheel_impulse * output[1] #* random.gauss(1, 0.2)
        )

        self.world.step_simulation(DT)
        self.uni.compute_state(DT)
        self.time += DT
