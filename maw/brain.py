from nn import NN
from brain_mixin import BrainMixin

__all__ = ["Brain", "brain_constructor"]

class Brain(NN, BrainMixin):
    def __init__(self, *args):
        super(Brain, self).__init__(*args)
        self.update_mutation_rate()

    def clone(self):
        return Brain(self)


def brain_constructor(spec, randomize=True):
    def ctor():
        b = Brain(spec)

        if randomize:
            b.set_weights_random(size=5.0)

        return b

    return ctor