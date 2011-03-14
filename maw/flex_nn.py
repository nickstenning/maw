import numpy

import math

class FlexNN:
    def __init__(self, num_in, num_out):
        self.num_in = num_in
        self.num_out = num_out

        self.nodes = numpy.ones([num_in + num_out])
        self.edges = numpy.empty([0, 3])

    def feed(self, inputs):
        if len(inputs) != self.num_in:
            raise IndexError

        self.nodes[0:self.num_in] = inputs

        num_edges = len(self.edges)

        for i in range(num_edges):
            edge_id = numpy.random.randint(0, num_edges)
            edge = self.edges[edge_id]

            from_id = int(edge[0])
            to_id = int(edge[1])
            weight = edge[2]

            self.nodes[to_id] += weight * self.nodes[from_id]

        self._clamp_outputs()

        return self.nodes[self.num_in:self.num_in + self.num_out]

    def add_node(self, value=0.0):
        self.nodes = numpy.append(self.nodes, value)

    def remove_node(self, node_id):
        self.nodes = numpy.delete(self.nodes, node_id)

    def add_edge(self, from_id, to_id, weight=1.0):
        self.edges = numpy.append(self.edges, [[from_id, to_id, weight]], axis=0)

    def remove_edge(self, edge_id):
        self.edges = numpy.delete(self.edges, edge_id, axis=0)

    def randomize_weights(self):
        self.edges[:,2] = numpy.random.random(len(self.edges)) * 2 - 1

    def _clamp_outputs(self):
        for i in xrange(self.num_in, self.num_in + self.num_out):
            if self.nodes[i] > 10:
                self.nodes[i] = 1
            elif self.nodes[i] < -10:
                self.nodes[i] = -1
            else:
                self.nodes[i] = 0

import unittest

class NNTest(unittest.TestCase):

    def setUp(self):
        self.nn = FlexNN(3, 4)

    def test_feed_in(self):
        # Should not raise
        self.nn.feed([1, 1, 1])

        # Should raise
        def feed_in_wrong_length():
            self.nn.feed([1, 1, 1, 1])

        self.assertRaises(IndexError, feed_in_wrong_length)

    def test_feed_out(self):
        result = self.nn.feed([1, 1, 1])

        self.assertEqual(len(result), 4, "Output vector not expected size.")

    def test_nodes(self):
        self.assertEqual(len(self.nn.nodes), 7)

    def test_add_node(self):
        self.nn.add_node(2.4)

        self.assertEqual(len(self.nn.nodes), 8)
        self.assertEqual(self.nn.nodes[7], 2.4)

    def test_delete_node(self):
        self.nn.remove_node(6)

        self.assertEqual(len(self.nn.nodes), 6)

    def test_edges(self):
        self.assertEqual(len(self.nn.edges), 0)

    def test_add_edge(self):
        self.nn.add_edge(0, 1, weight=2.3)

        self.assertEqual(len(self.nn.edges), 1)

    def test_remove_edge(self):
        self.nn.add_edge(0, 1, weight=2.3)
        self.nn.remove_edge(0)

        self.assertEqual(len(self.nn.edges), 0)

if __name__ == "__main__":
    unittest.main()
