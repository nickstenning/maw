import itertools
from bindings.nn import NN

class NN(NN):
    @classmethod
    def from_string(cls, s):
        spec = s.split('\n')
        layer_sizes = [int(x) for x in spec[1].split(' ') if x !='']

        nn = cls(layer_sizes)

        for k in range(len(nn.weights)):
            line = spec[2 + k].split(' ')

            mx = nn.weights[k]
            send = nn.layers[k]
            recv = nn.layers[k + 1]

            for i in range(len(send)):
                for j in range(len(recv)):
                    nn.set_weight(k, i, j, float(line.pop(0)))

        return nn

    def __repr__(self):
        o = []
        o.append( str(len(self.layers)) + '\n' )
        o.append( ' '.join(str(len(layer)) for layer in self.layers) + '\n' )

        for k in range(len(self.weights)):
            mx = self.weights[k]
            send = self.layers[k]
            recv = self.layers[k + 1]

            for i in range(len(send)):
                for j in range(len(recv)):
                    o.append( str(round(mx[i][j], 4)) + ' ' )

            o.append( '\n' )

        return ''.join(o)

    def to_dot(self):
        import nn2dot
        return nn2dot.nn2dot(self)


    def set_all(self, val):
        for k in range(len(self.weights)):
            mx = self.weights[k]
            send = self.layers[k]
            recv = self.layers[k + 1]

            for i in range(len(send)):
                for j in range(len(recv)):
                    self.set_weight(k, i, j, val)
