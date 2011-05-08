from nn import NN
from brain_mixin import BrainMixin

__all__ = ["Brain"]

class Brain(NN, BrainMixin):
    def __init__(self, *args):
        super(Brain, self).__init__(*args)
        self.update_mutation_rate()

    def clone(self):
        return Brain(self)

