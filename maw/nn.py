import itertools
import random

from bindings.nn import NN

__all__ = ['NN']

class NN(NN):
    @classmethod
    def from_string(cls, s):
        spec = s.split('\n')
        layer_sizes = [int(x) for x in spec[1].split(' ') if x != '']

        nn = cls(layer_sizes)

        for k in xrange(len(nn.weights)):
            line = spec[2 + k].split(' ')
            send = nn.layers[k]
            recv = nn.layers[k + 1]

            for i in xrange(len(send)):
                for j in xrange(len(recv) + 1): # NB +1 for bias
                    nn.set_weight(k, i, j, float(line.pop(0)))

        return nn

    def __repr__(self):
        o = []
        o.append(str(len(self.layers)) + '\n')
        o.append(' '.join(str(len(layer)) for layer in self.layers) + '\n')

        for mx in self.weights:
            for vec in mx:
                o.extend([str(round(w, 4)) + ' ' for w in vec])
            o.append('\n')

        return ''.join(o)

    @property
    def weights(self):
        return self._weights()

    @property
    def layers(self):
        return self._layers()

    @property
    def xweights(self):
        for k in xrange(len(self.weights)):
            for i in xrange(len(self.layers[k])):
                for j in xrange(len(self.layers[k + 1]) + 1): # NB +1 for bias
                    yield (k, i, j)

    def set_weight(self, k, i, j, val):
        return self._weights(k, i, j, val)

    def set_weights(self, val):
        for k, i, j in self.xweights:
            self.set_weight(k, i, j, val)

    def set_weights_random(self):
        for k, i, j in self.xweights:
            self.set_weight(k, i, j, random.gauss(0, 2))

    def topology_is_compatible(self, other):
        if len(self.layers) != len(other.layers):
            return False

        for i in xrange(len(self.layers)):
            if len(self.layers[i]) != len(other.layers[i]):
                return False

        return True

import math
import unittest

class TestNN(unittest.TestCase):

    def test_basics(self):
        nn = NN([1,1])

        nn.set_weights(1.0)
        out = nn.feed([1])

        self.assertEqual(out[0], 1)
        self.assertEqual(nn.layers[1][0], math.tanh(1.0))

        nn.set_weights(0.5)
        nn.feed([0.5])

        self.assertEqual(nn.layers[1][0], math.tanh(0.125))

    def test_multiinput(self):
        nn = NN([3,1])

        nn.set_weights(1.0)
        nn.feed([1, 1, 1])

        self.assertEqual(nn.layers[1][0], math.tanh(3.0))

    def test_bias(self):
        nn = NN([1,2])

        nn.set_weights(1.0)
        nn.feed([1])

        self.assertEqual(nn.layers[1][0], math.tanh(1.0))
        self.assertEqual(nn.layers[1][1], math.tanh(1.0))

        nn.set_weight(0, 0, 2, -0.5)
        nn.feed([1])

        self.assertEqual(nn.layers[1][0], math.tanh(-0.5))
        self.assertEqual(nn.layers[1][1], math.tanh(-0.5))

if __name__ == '__main__':
    unittest.main()
