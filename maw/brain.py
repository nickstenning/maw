import random
from operator import add

from nn import NN
from ga.evolvable import Evolvable

class BrainIncompatibleError(TypeError):
    pass

class Brain(NN, Evolvable):
    mutation_rate = 0.0
    mutation_size = 10.0

    def ga_init(self):
        self._compute_mutation_rate()
        self.setRandomWeights()

    # TODO: refactor this horrendous method
    def mutate(self):
        for k in range(len(self.weights())):
            mx = self.weights()[k]
            send = self.layers()[k]
            recv = self.layers()[k + 1]

            for i in range(len(send)):
                for j in range(len(recv)):
                    if random.choice([True, False]):
                        val = mx[i][j] + random.uniform(-self.mutation_size, self.mutation_size)
                        self.setWeight(k, i, j, val)

    # TODO: refactor this horrendous method
    def crossover(self, other):
        # We can't perform the crossover as implemented here unless the brains
        # have the same topology (i.e. same number of neurons in each layer).
        if not self.topologyIsCompatibleWith(other):
            raise BrainIncompatibleError("Cannot crossover with incompatible brain")

        # For each non-input neuron, we randomly choose a parent, and copy all
        # input weights from that parent.
        for k in range(len(self.weights())):
            mx = self.weights()[k]
            send = self.layers()[k]
            recv = self.layers()[k + 1]

            for j in range(len(recv)):
                if random.choice([True, False]):
                    for i in range(len(send)):
                        self.setWeight(k, i, j, other.weights()[k][i][j])

    def clone(self):
        return Brain(self)

    def _compute_mutation_rate(self):
        num_nonin_weights = 0

        for i in range(len(self.weights())):
            num_nonin_weights += len(self.layers()[i]) * len(self.layers()[i + 1])

        self.mutation_rate = 1.0 / num_nonin_weights