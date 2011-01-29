#include <vector>
#include <cmath>
#include <stdexcept>

#include "util.h"
#include "nn.h"

void randomWeights(NN::WeightMatrix& w, size_t layer1, size_t layer2) {
  for (size_t i = 0; i < layer1; i += 1) {
    for (size_t j = 0; j < layer2; j += 1) {
      w[i][j] = util::rand(-1, 1);
    }
  }
}

NN::NN()
: m_numInput(0)
, m_numHidden(0)
, m_numOutput(0)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH()
, m_weightsHO()
{}

NN::NN(size_t numInput, size_t numHidden, size_t numOutput)
: m_numInput(numInput)
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

void NN::setRandomWeights()
{
  randomWeights(m_weightsIH, m_numInput, m_numHidden);
  randomWeights(m_weightsHO, m_numHidden, m_numOutput);
}

std::vector<int> NN::feedForward (std::vector<double> const& input)
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

void NN::initLayers()
{
  m_layerInput = Layer(m_numInput, 0.0);
  m_layerHidden = Layer(m_numHidden, 0.0);
  m_layerOutput = Layer(m_numOutput, 0.0);
}

void NN::initWeights()
{
  m_weightsIH = WeightMatrix(m_numInput, Layer(m_numHidden));
  m_weightsHO = WeightMatrix(m_numHidden, Layer(m_numOutput));
}

bool NN::topologyIsCompatibleWith(NN const& rhs) const {
  return (m_numInput == rhs.m_numInput &&
          m_numHidden == rhs.m_numHidden &&
          m_numOutput == rhs.m_numOutput);
}

inline double NN::activationFunction (double x) {
  return tanh(x);
}

// TODO: is this pinning necessary? Could instead use output as input to
// util::choose(), and treat as probabilistic instruction.
inline int NN::terminationFunction (double x) {
  if (x > 0.75) {
    return 1;
  } else if (x < -0.75) {
    return -1;
  } else {
    return 0;
  }
}

// Serialize a brain
std::ostream& operator<< (std::ostream& os, NN const& nn) {
  // format version
  os << "1\n";
  // topology
  os << nn.m_numInput << " " << nn.m_numHidden << " " << nn.m_numOutput << "\n";

  size_t i, j;

  // weightsIH
  for (i = 0; i < nn.m_numInput; i += 1) {
    for (j = 0; j < nn.m_numHidden; j += 1) {
      os << nn.m_weightsIH[i][j] << " ";
    }
  }
  os << "\n";

  // weightsHO
  for (i = 0; i < nn.m_numHidden; i += 1) {
    for (j = 0; j < nn.m_numOutput; j += 1) {
      os << nn.m_weightsHO[i][j] << " ";
    }
  }
  os << "\n";

  return os;
}

// Deserialize a NN
std::istream& operator>> (std::istream& is, NN& nn) {
  int version;
  is >> version;

  if (version != 1) {
    throw std::runtime_error("Cannot deserialize a NN from unknown serialization version!");
  }

  is >> nn.m_numInput >> nn.m_numHidden >> nn.m_numOutput;

  nn.initLayers();
  nn.initWeights();

  size_t i, j;

  // weightsIH
  for (i = 0; i < nn.m_numInput; i += 1) {
    for (j = 0; j < nn.m_numHidden; j += 1) {
      is >> nn.m_weightsIH[i][j];
    }
  }


  // weightsHO
  for (i = 0; i < nn.m_numHidden; i += 1) {
    for (j = 0; j < nn.m_numOutput; j += 1) {
      is >> nn.m_weightsHO[i][j];
    }
  }

  return is;
}

