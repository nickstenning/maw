import random
from operator import add

from nn import NN

class BrainIncompatibleError(TypeError):
    pass

class Brain(NN):
    mutation_rate = 0.1
    mutation_sigma = 5.0

    def mutate(self):
        for k, i, j in self.xweights:
            if random.random() < self.mutation_rate:
                val = self.weights[k][i][j] + random.gauss(0, self.mutation_sigma)
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

    def clone(self):
        return Brain(self)

