#include <vector>
#include <cmath>

#include "rnn.h"

RNN::RNN()
  : NN()
{}

RNN::RNN(std::vector<size_t> layer_sizes)
  : NN(layer_sizes)
{
  init_weights(2); // 2 send_extras: 1 for bias, 1 for recurrence weight.
}

void RNN::feed_layer (size_t index)
{
  layer_t const& send = layers(index);
  layer_t const& recv = layers(index + 1);
  weight_matrix_t const& mx = weights(index);

  for(size_t j = 0; j < recv.size(); j += 1) {
    double val = recv[j];

    for(size_t i = 0; i < send.size(); i += 1) {
      val += send[i] * mx[i][j];
    }

    // Add bias
    val += mx[send.size()][j];

    // Recurrence weight: add receiving neuron's previous state.
    val *= mx[send.size() + 1][j];

    val = activation_function( val );

    // Set weight
    layers(index + 1, j, val);
  }
}