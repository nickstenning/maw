import itertools
import random

class NNMixin(object):
    @classmethod
    def from_string(cls, s):
        spec = s.split('\n')
        layer_sizes = [int(x) for x in spec[1].split(' ') if x != '']

        nn = cls(layer_sizes)

        for k in xrange(len(nn.weights)):
            line = spec[2 + k].split(' ')
            send = nn.layers[k]
            recv = nn.layers[k + 1]

            for i in xrange(len(send) + cls.send_extras):
                for j in xrange(len(recv)):
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
            for i in xrange(len(self.layers[k]) + self.send_extras):
                for j in xrange(len(self.layers[k + 1])):
                    yield (k, i, j)

    @property
    def xbiases(self):
        for k in xrange(len(self.weights)):
            i = len(self.layers[k]) # identify bias weight
            for j in xrange(len(self.layers[k + 1])):
                yield (k, i, j)

    def set_weight(self, k, i, j, val):
        return self._weights(k, i, j, val)

    def set_weights(self, val, biases=0):
        for k, i, j in self.xweights:
            self.set_weight(k, i, j, val)
        for k, i, j in self.xbiases:
            self.set_weight(k, i, j, biases)

    def topology_is_compatible(self, other):
        if len(self.layers) != len(other.layers):
            return False

        for i in xrange(len(self.layers)):
            if len(self.layers[i]) != len(other.layers[i]):
                return False

        return True