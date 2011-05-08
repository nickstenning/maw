from rnn import RNN
from brain_mixin import BrainMixin

__all__ = ["RBrain"]

class RBrain(RNN, BrainMixin):
    def __init__(self, *args):
        super(RBrain, self).__init__(*args)
        self.update_mutation_rate()

    def clone(self):
        return RBrain(self)