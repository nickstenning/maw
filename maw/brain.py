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
            b.set_weights_random(size=1.0)

            for i in range(spec[1]/2):
                b.set_weight(1, i, 0, 0)

            for i in range(spec[1]/2, spec[1]):
                b.set_weight(1, i, 1, 0)

        return b

    return ctor