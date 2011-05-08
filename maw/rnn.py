import itertools
import random

from bindings.rnn import RNN
from nn import NN

__all__ = ['RNN']

class RNN(RNN, NN):
    @classmethod
    def from_string(cls, s):
        spec = s.split('\n')
        layer_sizes = [int(x) for x in spec[1].split(' ') if x != '']

        nn = cls(layer_sizes)

        for k in xrange(len(nn.weights)):
            line = spec[2 + k].split(' ')
            send = nn.layers[k]
            recv = nn.layers[k + 1]

            for i in xrange(len(send) + 2): # NB +1 for bias, +1 for recursive weight
                for j in xrange(len(recv)):
                    nn.set_weight(k, i, j, float(line.pop(0)))

        return nn

    @property
    def xweights(self):
        for k in xrange(len(self.weights)):
            for i in xrange(len(self.layers[k]) + 2): # NB +1 for bias, +1 for recursive weight
                for j in xrange(len(self.layers[k + 1])):
                    yield (k, i, j)
