#include <vector>
#include <cmath>
#include <assert.h>

#include "util.h"
#include "brain.h"

void randomWeights(Weights& w, size_t layer1, size_t layer2) {
  for (size_t i = 0; i < layer1; i += 1) {
    for (size_t j = 0; j < layer2; j += 1) {
      w[i][j] = util::rand(-1, 1);
    }
  }
}

Brain::Brain()
: m_fitness(0.0)
, m_numInput(0)
, m_numHidden(0)
, m_numOutput(0)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH()
, m_weightsHO()
{}

Brain::Brain(size_t numInput, size_t numHidden, size_t numOutput)
: m_fitness(0.0)
, m_numInput(numInput)
, m_numHidden(numHidden)
, m_numOutput(numOutput)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH()
, m_weightsHO()
{
  initLayers();
  initWeights();
}

Brain::~Brain() {}

void Brain::initRandomWeights() {
  randomWeights(m_weightsIH, m_numInput, m_numHidden);
  randomWeights(m_weightsHO, m_numHidden, m_numOutput);
}

double const& Brain::fitness() const {
  return m_fitness;
}

Brain const& Brain::fitness(double const& f) {
  m_fitness = f;
  return *this;
}

Weights const& Brain::weightsHidden() const {
  return m_weightsIH;
}

Weights const& Brain::weightsOutput() const {
  return m_weightsHO;
}

Brain& Brain::weightsHidden(Weights const& w) {
  m_weightsIH = w;
  return *this;
}

Brain& Brain::weightsOutput(Weights const& w) {
  m_weightsHO = w;
  return *this;
}

std::vector<int> Brain::feedForward (std::vector<double> const& input) {
  for(size_t i = 0; i < m_numInput; i += 1) m_layerInput[i] = input[i];

  // Propagate to hidden layer
  for(size_t j = 0; j < m_numHidden; j += 1) {
    m_layerHidden[j] = 0.0;

    for(size_t k = 0; k < m_numInput; k += 1) {
      m_layerHidden[j] += m_layerInput[k] * m_weightsIH[k][j];
    }

    m_layerHidden[j] = this->activationFunction( m_layerHidden[j] );
  }

  // Propagate to output layer
  for(size_t j = 0; j < m_numOutput; j += 1) {
    m_layerOutput[j] = 0.0;

    for(size_t k = 0; k < m_numHidden; k += 1) {
      m_layerOutput[j] += m_layerHidden[k] * m_weightsHO[k][j];
    }

    m_layerOutput[j] = this->activationFunction( m_layerOutput[j] );
  }

  std::vector<int> output;
  for(size_t i = 0; i < m_numOutput; i += 1) {
    int pinnedOutput = this->terminationFunction( m_layerOutput[i] );
    output.push_back(pinnedOutput);
  }

  return output;
}

void Brain::initLayers() {
  m_layerInput = Layer(m_numInput, 0.0);
  m_layerHidden = Layer(m_numHidden, 0.0);
  m_layerOutput = Layer(m_numOutput, 0.0);
}

void Brain::initWeights() {
  m_weightsIH = Weights(m_numInput, std::vector<double>(m_numHidden));
  m_weightsHO = Weights(m_numHidden, std::vector<double>(m_numOutput));
}

bool Brain::topologyIsCompatibleWith(Brain const& rhs) const {
  return (m_numInput == rhs.m_numInput &&
          m_numHidden == rhs.m_numHidden &&
          m_numOutput == rhs.m_numOutput);
}

inline double Brain::activationFunction (double x) {
  return tanh(x);
}

inline int Brain::terminationFunction (double x) {
  if (x > 0.75) {
    return 1;
  } else if (x < -0.75) {
    return -1;
  } else {
    return 0;
  }
}

bool operator< (Brain const& lhs, Brain const& rhs) {
  return lhs.fitness() < rhs.fitness();
}

double operator+ (double const& lhs, Brain const& rhs) {
  return lhs + rhs.fitness();
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

  assert(version == 1);

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
