import random
from operator import add

from nn import NN

class BrainIncompatibleError(TypeError):
    pass

class Brain(NN):
    mutation_count = 1.0
    mutation_size = 5.0

    def __init__(self, *args):
        super(Brain, self).__init__(*args)

        num_nonin_weights = 0

        for i in xrange(len(self.weights)):
            num_nonin_weights += len(self.layers[i]) * len(self.layers[i + 1])

        self.mutation_rate = float(self.mutation_count) / num_nonin_weights

    def mutate(self):
        for k, i, j in self.xweights:
            if random.random() < self.mutation_rate:
                val = self.weights[k][i][j] + random.uniform(-self.mutation_size, self.mutation_size)
                self.set_weight(k, i, j, val)

    def crossover(self, other):
        # We can't perform the crossover as implemented here unless the brains
        # have the same topology (i.e. same number of neurons in each layer).
        if not self.topology_is_compatible(other):
            raise BrainIncompatibleError("Cannot crossover with incompatible brain")

        # For each non-input neuron, we randomly choose a parent, and copy all
        # input weights from that parent.
        for k in xrange(len(self.weights)):
            send = self.layers[k]
            recv = self.layers[k + 1]

            for j in xrange(len(recv)):
                if random.choice([True, False]):
                    for i in xrange(len(send)):
                        self.set_weight(k, i, j, other.weights[k][i][j])

    def set_weights_random(self, and_biases=False):
        for k, i, j in self.xweights:
            self.set_weight(k, i, j, random.uniform(-self.mutation_size, self.mutation_size))

        if not and_biases:
            for k, i, j in self.xbiases:
                self.set_weight(k, i, j, 0)

    def clone(self):
        return Brain(self)

