from bindings.nn import NN as NNBinding
from nn_mixin import NNMixin

__all__ = ['NN']

class NN(NNMixin, NNBinding):
    send_extras = 1

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

        self.assertEqual(nn.layers[1][0], math.tanh(0.25))

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

        nn.set_weight(0, 1, 0, 2)
        nn.feed([1])

        self.assertEqual(nn.layers[1][0], math.tanh(3))
        self.assertEqual(nn.layers[1][1], math.tanh(1))

if __name__ == '__main__':
    unittest.main()
