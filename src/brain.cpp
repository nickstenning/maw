#include <istream>
#include <vector>
#include <cmath>
#include <stdexcept>

#include "nn.h"
#include "evolvable.h"
#include "util.h"
#include "brain.h"

Brain::Brain()
  : NN()
  , m_mutationRate(0)
  , m_mutationSize(MUTATION_SIZE)
{}

Brain::Brain(std::vector<size_t> layerSizes)
  : NN(layerSizes)
  , m_mutationRate(0)
  , m_mutationSize(MUTATION_SIZE)
{
  updateMutationRate();
}

Brain::Brain(std::istream& is)
  : NN(is)
  , m_mutationRate(0)
  , m_mutationSize(MUTATION_SIZE)
{
  updateMutationRate();
}

void Brain::gaInit()
{
  setRandomWeights();
}

void Brain::updateMutationRate()
{
  size_t numNonInputNeurons = 0;

  for (size_t i = 0; i < m_weights.size(); i += 1) {
    numNonInputNeurons += m_layers[i].size() * m_layers[i + 1].size();
  }

  m_mutationRate = 10 * 1.0 / numNonInputNeurons;
}

Evolvable* Brain::clone()
{
  return new Brain(*this);
}

Evolvable* Brain::clone(Evolvable* toCopy)
{
  Brain* toCopyAsBrain = dynamic_cast<Brain*>(toCopy);
  if (toCopyAsBrain == NULL) {
    throw std::runtime_error("Brain::clone(Evolvable*) received an Evolvable instance that wasn't a Brain!");
  }

  return new Brain(*toCopyAsBrain);
}

Evolvable* Brain::mutate ()
{
  size_t i, j, k;

  // Mutate each weight with a probability of 1/m_mutationRate by
  // adding num in [-m_mutationSize,m_mutationSize)

  for (k = 0; k < m_weights.size(); k += 1) {
    NN::weight_matrix_t& mx = m_weights[k];
    NN::layer_t& send = m_layers[k];
    NN::layer_t& recv = m_layers[k + 1];

    for (i = 0; i < send.size(); i += 1) {
      for (j = 0; j < recv.size(); j += 1) {
        if (util::choose(m_mutationRate)) {
          mx[i][j] += util::rand(-m_mutationSize, m_mutationSize);
        }
      }
    }
  }

  return this;
}

Evolvable* Brain::crossover (Evolvable const* other)
{
  Brain const* otherBrain = dynamic_cast<Brain const*>(other);

  if (otherBrain == NULL) {
    throw std::runtime_error("Brain::crossover received an Evolvable instance that wasn't a Brain!");
  }

  size_t i, j, k;

  // We can't perform the crossover as implemented here unless the brains
  // have the same topology (i.e. same number of neurons in each layer).
  if (!topologyIsCompatibleWith(*otherBrain)) {
    throw std::runtime_error("Brain::crossover received a Brain with incompatible topology!");
  }

  // For each non-input neuron, we randomly choose a parent, and copy all
  // input weights from that parent.
  for (k = 0; k < m_weights.size(); k += 1) {
    NN::weight_matrix_t& mx = m_weights[k];
    NN::layer_t& send = m_layers[k];
    NN::layer_t& recv = m_layers[k + 1];

    for (j = 0; j < recv.size(); j += 1) {
      if (util::choose(0.5)) {
        for (i = 0; i < send.size(); i += 1) {
          mx[i][j] = otherBrain->m_weights[k][i][j];
        }
      }
    }
  }

  return this;
}
