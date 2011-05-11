import math
import random

from maw.util import dirac_delta
from maw.unicycle import Unicycle, DT
from maw.world_manager import WorldManager

MAX_EVAL_TIME   = 100.0
YAW_SCORE_ANG   = math.pi - 0.1
PITCH_SCORE_ANG = math.pi / 20.0
ROLL_SCORE_ANG  = math.pi / 20.0

class Evaluator(object):

    def __init__(self):
        self.world = WorldManager()
        self.uni = Unicycle()

        self.uni.add_to_manager(self.world)

    def evaluate(self, brain):
        fitness = 0

        self.time = 0
        self.uni.reset(0.01)

        while (self.time < MAX_EVAL_TIME):
            self.step(brain)

            in_pitch_threshold = abs(self.uni.pitch()) < PITCH_SCORE_ANG
            in_roll_threshold = abs(self.uni.roll()) < ROLL_SCORE_ANG
            in_yaw_threshold = abs(self.uni.yaw()) < YAW_SCORE_ANG
            # in_pos_threshold = abs(self.uni.x()) < 10.0 and abs(self.uni.z()) < 10.0

            if in_pitch_threshold and in_roll_threshold and in_yaw_threshold:# and in_pos_threshold:
                score = DT
                # score += DT * dirac_delta(self.uni.kinetic_energy(), 0.5)

                fitness += score
            else:
                break # Failure. No need to evaluate further.

        return fitness

    def step(self, brain):

        input = [
            self.uni.pitch(),
            self.uni.roll(),
            self.uni.pitch_velocity(),
            self.uni.yaw_velocity(),
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
