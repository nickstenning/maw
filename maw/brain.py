import random
from operator import add

from nn import NN

# The mutation proportion dictates the most likely proportion of
# the non-input neurons to be mutated.
MUTATION_PROPORTION = 0.1

class BrainIncompatibleError(TypeError):
    pass

class Brain(NN):
    mutation_rate = None
    mutation_sigma = 5.0

    def __init__(self, *args):
        super(Brain, self).__init__(*args)
        self._compute_mutation_rate()

    # TODO: refactor this horrendous method
    def mutate(self):
        for k in range(len(self.weights)):
            mx = self.weights[k]
            send = self.layers[k]
            recv = self.layers[k + 1]

            for i in range(len(send)):
                for j in range(len(recv)):
                    if random.random() < self.mutation_rate:
                        val = mx[i][j] + random.gauss(0, self.mutation_sigma)
                        self.set_weight(k, i, j, val)

    # TODO: refactor this horrendous method
    def crossover(self, other):
        # We can't perform the crossover as implemented here unless the brains
        # have the same topology (i.e. same number of neurons in each layer).
        if not self.topology_is_compatible(other):
            raise BrainIncompatibleError("Cannot crossover with incompatible brain")

        # For each non-input neuron, we randomly choose a parent, and copy all
        # input weights from that parent.
        for k in range(len(self.weights)):
            mx = self.weights[k]
            send = self.layers[k]
            recv = self.layers[k + 1]

            for j in range(len(recv)):
                if random.choice([True, False]):
                    for i in range(len(send)):
                        self.set_weight(k, i, j, other.weights[k][i][j])

    def clone(self):
        return Brain(self)

    def _compute_mutation_rate(self):
        num_nonin_weights = 0

        for i in range(len(self.weights)):
            num_nonin_weights += len(self.layers[i]) * len(self.layers[i + 1])

        # mutate 1..(MUTATION_PROPORTION * num_nonin_weights) weights
        self.mutation_rate = max(MUTATION_PROPORTION, 1/num_nonin_weights)
