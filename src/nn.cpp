#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <stdexcept>

#include "util.h"
#include "nn.h"

NN::NN()
  : m_layers()
  , m_weights()
{}

NN::NN(std::vector<size_t> layerSizes)
  : m_layers()
  , m_weights()
{
  initLayers(layerSizes);
  initWeights();
}

NN::NN(std::istream& is)
  : m_layers()
  , m_weights()
{
  is >> *this;
}


void NN::setRandomWeights()
{
  for (size_t i = 0; i < m_weights.size(); i += 1) {
    setRandomWeightsForMatrix(i);
  }
}

void NN::setRandomWeightsForMatrix(size_t index)
{
  weight_matrix_t& mx = m_weights[index];

  for (size_t i = 0; i < m_layers[index].size(); i += 1) {
    for (size_t j = 0; j < m_layers[index + 1].size(); j += 1) {
      mx[i][j] = util::rand(-1, 1);
    }
  }
}

NN::output_t NN::feedForward (layer_t const& input)
{
  // Copy input if correct length
  if (m_layers[0].size() != input.size()) {
    throw std::runtime_error("Input to NN::feedForward of incorrect size");
  }
  m_layers[0] = input;

  // Propagate, layer by layer.
  for(size_t i = 0; i < m_layers.size() - 1; i += 1) {
    feedForwardLayer(i);
  }

  // Terminate last layer, and copy output.
  output_t output;
  layer_t& outputLayer = m_layers[m_layers.size() - 1];

  for(size_t i = 0; i < outputLayer.size(); i += 1) {
    int pinnedOutput = terminationFunction( outputLayer[i] );
    output.push_back(pinnedOutput);
  }

  return output;
}

void NN::feedForwardLayer (size_t index)
{
  layer_t& send = m_layers[index];
  layer_t& recv = m_layers[index + 1];
  weight_matrix_t& mx = m_weights[index];

  for(size_t j = 0; j < recv.size(); j += 1) {
    recv[j] = 0.0; // Clear receiving neuron's previous state.

    for(size_t i = 0; i < send.size(); i += 1) {
      recv[j] += send[i] * mx[i][j];
    }

    recv[j] = activationFunction( recv[j] );
  }
}

void NN::initLayers (std::vector<size_t> layerSizes)
{
  m_layers = layers_t(layerSizes.size());

  // Init layers with neuron output values of 0.
  for (size_t i = 0; i < layerSizes.size(); i += 1) {
    m_layers[i] = layer_t(layerSizes[i], 0);
  }
}

void NN::initWeights ()
{
  m_weights = weights_t(m_layers.size() - 1);

  // Init weights. This is a feed-forward network so there is a weight_matrix_t
  // connecting every pair of adjacent layers.
  for (size_t i = 0; i < m_weights.size(); i += 1) {
    layer_t const& send = m_layers[i];
    layer_t const& recv = m_layers[i + 1];
    m_weights[i] = weight_matrix_t(send.size(), weight_vector_t(recv.size(), 0));
  }
}

bool NN::topologyIsCompatibleWith(NN const& rhs) const
{
  if (m_layers.size() != rhs.m_layers.size()) {
    return false;
  }

  for (size_t i = 0; i < m_layers.size(); i += 1) {
    if (m_layers[i].size() != rhs.m_layers[i].size()) {
      return false;
    }
  }

  return true;
}

inline double NN::activationFunction (double x)
{
  return tanh(x);
}

// TODO: is this pinning necessary? Could instead use output as input to
// util::choose(), and treat as probabilistic instruction.
inline int NN::terminationFunction (double x)
{
  if (x > 0.75) {
    return 1;
  } else if (x < -0.75) {
    return -1;
  } else {
    return 0;
  }
}

// Serialize a brain
std::ostream& operator<< (std::ostream& os, NN const& nn)
{
  os << std::fixed;
  // number of layers
  os << nn.m_layers.size() << "\n";

  // layers
  for (NN::layers_t::const_iterator it = nn.m_layers.begin(); it != nn.m_layers.end(); ++it) {
    os << it->size() << " ";
  }
  os << "\n";

  size_t k, i, j;
  // weights
  for (k = 0; k < nn.m_weights.size(); k += 1) {
    NN::weight_matrix_t const& mx = nn.m_weights[k];
    NN::layer_t const& send = nn.m_layers[k];
    NN::layer_t const& recv = nn.m_layers[k + 1];

    for (i = 0; i < send.size(); i += 1) {
      for (j = 0; j < recv.size(); j += 1) {
        os << mx[i][j] << " ";
      }
    }

    os << "\n";
  }

  return os;
}

// Deserialize a NN
std::istream& operator>> (std::istream& is, NN& nn)
{
  size_t numLayers;
  is >> numLayers;
  std::vector<size_t> layerSizes(numLayers);

  for (size_t i = 0; i < numLayers; i += 1) {
    is >> layerSizes[i];
  }

  nn.initLayers(layerSizes);
  nn.initWeights();

  size_t k, i, j;

  // weights
  for (k = 0; k < nn.m_weights.size(); k += 1) {
    NN::weight_matrix_t& mx = nn.m_weights[k];
    NN::layer_t& send = nn.m_layers[k];
    NN::layer_t& recv = nn.m_layers[k + 1];

    for (i = 0; i < send.size(); i += 1) {
      for (j = 0; j < recv.size(); j += 1) {
        is >> mx[i][j];
      }
    }
  }

  return is;
}

