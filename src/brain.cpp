#include <vector>
#include <cmath>
#include <stdexcept>

#include "evolvable.h"
#include "util.h"
#include "brain.h"

Brain::Brain()
: NN()
, m_mutationRate(0)
, m_mutationSize(MUTATION_SIZE)
{}

Brain::Brain(size_t numInput, size_t numHidden, size_t numOutput)
: NN(numInput, numHidden, numOutput)
, m_mutationRate(1.0/(numInput * numHidden + numHidden * numOutput))
, m_mutationSize(MUTATION_SIZE)
{}

void Brain::gaInit() {
  setRandomWeights();
}

Evolvable* Brain::clone() {
  return new Brain(m_numInput, m_numHidden, m_numOutput);
}

Evolvable* Brain::clone(Evolvable* toCopy) {
  Brain* toCopyAsBrain = dynamic_cast<Brain*>(toCopy);
  if (toCopyAsBrain == NULL) {
    throw std::runtime_error("Brain::clone(Evolvable*) received an Evolvable instance that wasn't a Brain!");
  }

  return new Brain(*toCopyAsBrain);
}

Evolvable* Brain::mutate ()
{
  size_t i, j;

  // Mutate each weight with a probability of 1/m_mutationRate by
  // adding num in [-m_mutationSize,m_mutationSize)
  for (i = 0; i < m_numInput; i += 1) {
    for (j = 0; j < m_numHidden; j += 1) {
      if (util::choose(m_mutationRate)) {
        m_weightsIH[i][j] += util::rand(-m_mutationSize, m_mutationSize);
      }
    }
  }

  for (i = 0; i < m_numHidden; i += 1) {
    for (j = 0; j < m_numOutput; j += 1) {
      if (util::choose(m_mutationRate)) {
        m_weightsHO[i][j] += util::rand(-m_mutationSize, m_mutationSize);
      }
    }
  }

  return this;
}

Evolvable* Brain::crossover (Evolvable const* other) {
  Brain const* otherBrain = dynamic_cast<Brain const*>(other);

  if (otherBrain == NULL) {
    throw std::runtime_error("Brain::crossover received an Evolvable instance that wasn't a Brain!");
  }

  size_t i, j;

  // We can't perform the crossover as implemented here unless the brains
  // have the same topology (i.e. same number of neurons in each layer).
  if (!topologyIsCompatibleWith(*otherBrain)) {
    throw std::runtime_error("Brain::crossover received a Brain with incompatible topology!");
  }

  // For each non-input neuron, we randomly choose a parent, and copy all
  // input weights from that parent.

  // For hidden layer:
  for (j = 0; j < m_numHidden; j += 1) {
    if (util::choose(0.5)) {
      for (i = 0; i < m_numInput; i += 1) {
        m_weightsIH[i][j] = otherBrain->m_weightsIH[i][j];
      }
    }
  }

  // For output layer:
  for (j = 0; j < m_numOutput; j += 1) {
    if (util::choose(0.5)) {
      for (i = 0; i < m_numHidden; i += 1) {
        m_weightsHO[i][j] = otherBrain->m_weightsHO[i][j];
      }
    }
  }

  return this;
}
