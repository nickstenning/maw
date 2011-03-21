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

            for i in xrange(len(nn.layers[k])):
                for j in xrange(len(nn.layers[k + 1])):
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
                for j in xrange(len(self.layers[k + 1])):
                    yield (k, i, j)

    def set_weight(self, k, i, j, val):
        return self._weights(k, i, j, val)

    def set_weights(self, val):
        for k, i, j in self.xweights:
            self.set_weight(k, i, j, val)

    def set_weights_random(self):
        for k, i, j in self.xweights:
            self.set_weight(k, i, j, random.gauss(0, 1))

    def topology_is_compatible(self, other):
        if len(self.layers) != len(other.layers):
            return False

        for i in xrange(len(self.layers)):
            if len(self.layers[i]) != len(other.layers[i]):
                return False

        return True
