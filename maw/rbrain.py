from rnn import RNN
from brain_mixin import BrainMixin

__all__ = ["RBrain"]

class RBrain(RNN, BrainMixin):
    def __init__(self, *args):
        super(RBrain, self).__init__(*args)
        self.update_mutation_rate()

    def clone(self):
        return RBrain(self)


def brain_constructor(spec, randomize=True):
    def ctor():
        b = RBrain(spec)
        b.set_weights_random(size=10.0)
        return b

    return ctor