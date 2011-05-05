#include <vector>
#include <cmath>

#include "nn.h"

NN::NN()
  : m_layers()
  , m_weights()
{}

NN::NN(std::vector<size_t> layer_sizes)
  : m_layers()
  , m_weights()
{
  init_layers(layer_sizes);
  init_weights();
}

NN::output_t NN::feed (layer_t const& input) throw(NNInputSizeError)
{
  // Copy input if correct length
  if (m_layers[0].size() != input.size()) {
    throw NNInputSizeError();
  }
  m_layers[0] = input;

  // Propagate, layer by layer.
  for(size_t i = 0; i < m_layers.size() - 1; i += 1) {
    feed_layer(i);
  }

  // Terminate last layer, and copy output.
  output_t output;
  layer_t& output_layer = m_layers[m_layers.size() - 1];

  for(size_t i = 0; i < output_layer.size(); i += 1) {
    // int pinned_output = termination_function( output_layer[i] );
    // output.push_back(pinned_output);
    output.push_back(output_layer[i]);
  }

  return output;
}

void NN::feed_layer (size_t index)
{
  layer_t& send = m_layers[index];
  layer_t& recv = m_layers[index + 1];
  weight_matrix_t& mx = m_weights[index];

  for(size_t j = 0; j < recv.size(); j += 1) {
    recv[j] = 0.0; // Clear receiving neuron's previous state.

    for(size_t i = 0; i < send.size(); i += 1) {
      double bias = mx[i][recv.size()];
      recv[j] += send[i] * bias * mx[i][j];
    }

    recv[j] = activation_function( recv[j] );
  }
}

void NN::init_layers (std::vector<size_t> layer_sizes)
{
  m_layers = layers_t(layer_sizes.size());

  // Init layers with neuron output values of 0.
  for (size_t i = 0; i < layer_sizes.size(); i += 1) {
    m_layers[i] = layer_t(layer_sizes[i], 0);
  }
}

void NN::init_weights ()
{
  m_weights = weights_t(m_layers.size() - 1);

  // Init weights. This is a feed-forward network so there is a weight_matrix_t
  // connecting every pair of adjacent layers.
  for (size_t i = 0; i < m_weights.size(); i += 1) {
    layer_t const& send = m_layers[i];
    layer_t const& recv = m_layers[i + 1];
    m_weights[i] = weight_matrix_t(send.size(), weight_vector_t(recv.size() + 1, 0));
  }
}

inline double NN::activation_function (double x)
{
  return tanh(x);
}

// TODO: is this pinning necessary? Could instead use output as input to
// util::choose(), and treat as probabilistic instruction.
inline int NN::termination_function (double x)
{
  if (x > 0.75) {
    return 1;
  } else if (x < -0.75) {
    return -1;
  } else {
    return 0;
  }
}
