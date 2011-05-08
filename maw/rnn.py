from bindings.rnn import RNN as RNNBinding
from nn_mixin import NNMixin

__all__ = ['RNN']

class RNN(NNMixin, RNNBinding):
    send_extras = 2