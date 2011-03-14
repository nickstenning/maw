import random
import numpy

from flex_nn import FlexNN

class FlexBrain(FlexNN):
    def mutate(self):
        if random.random() < 0.5:
            self.add_node(random.gauss(0, 10))

        if random.random() < 0.5:
            from_id = random.randint(0, len(self.nodes) - 1)
            to_id = random.randint(0, len(self.nodes) - 1)
            self.add_edge(from_id, to_id, random.gauss(0, 10))

        num_edges = len(self.edges)

        for edge in self.edges:
            if random.random() < 1/num_edges:
                edge[2] += random.gauss(0, 10)

    def crossover(self, other):
        return self

    def clone(self):
        b = FlexBrain(self.num_in, self.num_out)
        b.nodes = numpy.copy(self.nodes)
        b.edges = numpy.copy(self.edges)
        return b