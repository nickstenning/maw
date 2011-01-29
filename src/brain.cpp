#include <vector>
#include <cmath>
#include <stdexcept>

#include "evolvable.h"
#include "util.h"
#include "brain.h"

void randomWeights(Brain::WeightMatrix& w, size_t layer1, size_t layer2) {
  for (size_t i = 0; i < layer1; i += 1) {
    for (size_t j = 0; j < layer2; j += 1) {
      w[i][j] = util::rand(-1, 1);
    }
  }
}

Brain::Brain()
: m_numInput(0)
, m_numHidden(0)
, m_numOutput(0)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH()
, m_weightsHO()
, m_mutationRate(0)
, m_mutationSize(MUTATION_SIZE)
{}

Brain::Brain(size_t numInput, size_t numHidden, size_t numOutput)
: m_numInput(numInput)
, m_numHidden(numHidden)
, m_numOutput(numOutput)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH()
, m_weightsHO()
, m_mutationRate(1.0/(numInput * numHidden + numHidden * numOutput))
, m_mutationSize(MUTATION_SIZE)
{
  initLayers();
  initWeights();
}

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

void Brain::setRandomWeights()
{
  randomWeights(m_weightsIH, m_numInput, m_numHidden);
  randomWeights(m_weightsHO, m_numHidden, m_numOutput);
}

std::vector<int> Brain::feedForward (std::vector<double> const& input)
{
  for(size_t i = 0; i < m_numInput; i += 1) m_layerInput[i] = input[i];

  // Propagate to hidden layer
  for(size_t j = 0; j < m_numHidden; j += 1) {
    m_layerHidden[j] = 0.0;

    for(size_t k = 0; k < m_numInput; k += 1) {
      m_layerHidden[j] += m_layerInput[k] * m_weightsIH[k][j];
    }

    m_layerHidden[j] = activationFunction( m_layerHidden[j] );
  }

  // Propagate to output layer
  for(size_t j = 0; j < m_numOutput; j += 1) {
    m_layerOutput[j] = 0.0;

    for(size_t k = 0; k < m_numHidden; k += 1) {
      m_layerOutput[j] += m_layerHidden[k] * m_weightsHO[k][j];
    }

    m_layerOutput[j] = activationFunction( m_layerOutput[j] );
  }

  std::vector<int> output;
  for(size_t i = 0; i < m_numOutput; i += 1) {
    int pinnedOutput = terminationFunction( m_layerOutput[i] );
    output.push_back(pinnedOutput);
  }

  return output;
}

void Brain::initLayers()
{
  m_layerInput = Layer(m_numInput, 0.0);
  m_layerHidden = Layer(m_numHidden, 0.0);
  m_layerOutput = Layer(m_numOutput, 0.0);
}

void Brain::initWeights()
{
  m_weightsIH = WeightMatrix(m_numInput, Layer(m_numHidden));
  m_weightsHO = WeightMatrix(m_numHidden, Layer(m_numOutput));
}

bool Brain::topologyIsCompatibleWith(Brain const& rhs) const {
  return (m_numInput == rhs.m_numInput &&
          m_numHidden == rhs.m_numHidden &&
          m_numOutput == rhs.m_numOutput);
}

inline double Brain::activationFunction (double x) {
  return tanh(x);
}

// TODO: is this pinning necessary? Could instead use output as input to
// util::choose(), and treat as probabilistic instruction.
inline int Brain::terminationFunction (double x) {
  if (x > 0.75) {
    return 1;
  } else if (x < -0.75) {
    return -1;
  } else {
    return 0;
  }
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

// Serialize a brain
std::ostream& operator<< (std::ostream& os, Brain const& b) {
  // format version
  os << "1\n";
  // topology
  os << b.m_numInput << " " << b.m_numHidden << " " << b.m_numOutput << "\n";

  size_t i, j;

  // weightsIH
  for (i = 0; i < b.m_numInput; i += 1) {
    for (j = 0; j < b.m_numHidden; j += 1) {
      os << b.m_weightsIH[i][j] << " ";
    }
  }
  os << "\n";

  // weightsHO
  for (i = 0; i < b.m_numHidden; i += 1) {
    for (j = 0; j < b.m_numOutput; j += 1) {
      os << b.m_weightsHO[i][j] << " ";
    }
  }
  os << "\n";

  return os;
}

// Deserialize a brain
std::istream& operator>> (std::istream& is, Brain& b) {
  int version;
  is >> version;

  if (version != 1) {
    throw std::runtime_error("Cannot deserialize a brain from unknown serialization version!");
  }

  is >> b.m_numInput >> b.m_numHidden >> b.m_numOutput;

  b.initLayers();
  b.initWeights();

  size_t i, j;

  // weightsIH
  for (i = 0; i < b.m_numInput; i += 1) {
    for (j = 0; j < b.m_numHidden; j += 1) {
      is >> b.m_weightsIH[i][j];
    }
  }


  // weightsHO
  for (i = 0; i < b.m_numHidden; i += 1) {
    for (j = 0; j < b.m_numOutput; j += 1) {
      is >> b.m_weightsHO[i][j];
    }
  }

  return is;
}
