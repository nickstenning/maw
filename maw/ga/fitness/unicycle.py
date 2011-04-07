import math
import random

from maw.util import dirac_delta
from maw.unicycle import Unicycle
from maw.world_manager import WorldManager

DT              = 0.02
MAX_EVAL_TIME   = 100.0
YAW_SCORE_ANG   = math.pi - 0.2
PITCH_SCORE_ANG = math.pi / 12.0
ROLL_SCORE_ANG  = math.pi / 12.0

class Evaluator(object):

    def __init__(self):
        self.world = WorldManager()
        self.uni = Unicycle()

        self.uni.add_to_manager(self.world)

    def evaluate(self, brain, num_evaluations=3):
        fitness = 0

        for _ in xrange(num_evaluations):
            fitness += self.evaluate_once(brain)

        return fitness / num_evaluations

    def evaluate_once(self, brain):
        fitness = 0

        self.time = 0
        self.uni.reset()
        self.uni.compute_state()

        while (self.time < MAX_EVAL_TIME):
            self.target_yaw_velocity = 0.0
            self.target_wheel_velocity = 0.0

            self.step(brain)

            in_yaw_threshold = abs(self.uni.yaw()) < YAW_SCORE_ANG
            in_pitch_threshold = abs(self.uni.pitch()) < PITCH_SCORE_ANG
            in_roll_threshold = abs(self.uni.roll()) < ROLL_SCORE_ANG

            if in_yaw_threshold and in_pitch_threshold and in_roll_threshold:
                score = 0
                score += abs(self.uni.pitch())
                score += abs(self.uni.roll())
                score += abs(self.uni.yaw_velocity() - self.target_yaw_velocity)
                score += abs(self.uni.wheel_velocity() - self.target_wheel_velocity)

                fitness += DT * dirac_delta(score, 2)
            else:
                break # Failure. No need to evaluate further.

        return fitness

    def step(self, brain):

        input = [
            self.uni.yaw(),
            self.uni.pitch(),
            self.uni.roll(),
            self.uni.wheel_velocity() - self.target_wheel_velocity,
            self.uni.yaw_velocity() - self.target_yaw_velocity
        ]

        output = brain.feed(input)

        self.uni.apply_drive_impulse(self.uni.drive_impulse * output[0])
        self.uni.apply_wheel_impulse(self.uni.wheel_impulse * output[1])

        self.world.step_simulation(DT)
        self.uni.compute_state()

        self.time += DT
