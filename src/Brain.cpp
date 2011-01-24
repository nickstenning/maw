/*
 *  Brain.cpp
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include "Brain.h"

#include <vector>
#include <cmath>

#include "util.h"

Weights randomWeights(uint layer1, uint layer2) {
  Weights w(layer1);

  for (uint i = 0; i < layer1; i += 1) {
    std::vector<double> x(layer2);
    w[i] = x;

    for (uint j = 0; j < layer2; j += 1) {
      w[i][j] = util::rand(-1, 1);
    }
  }
  return w;
}

Brain::Brain(uint numInput, uint numHidden, uint numOutput)
: m_numInput(numInput)
, m_numHidden(numHidden)
, m_numOutput(numOutput)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH(randomWeights(m_numInput, m_numHidden))
, m_weightsHO(randomWeights(m_numHidden, m_numOutput))
, m_fitness(0.0)
{
  initLayer(m_layerInput, m_numInput);
  initLayer(m_layerHidden, m_numHidden);
  initLayer(m_layerOutput, m_numOutput);
}

Brain::~Brain() {}

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
  for(uint i = 0; i < m_numInput; i += 1) m_layerInput[i] = input[i];

  // Propagate to hidden layer
  for(uint j = 0; j < m_numHidden; j += 1) {
    m_layerHidden[j] = 0.0;

    for(uint k = 0; k < m_numInput; k += 1) {
      m_layerHidden[j] += m_layerInput[k] * m_weightsIH[k][j];
    }

    m_layerHidden[j] = this->activationFunction( m_layerHidden[j] );
  }

  // Propagate to output layer
  for(uint j = 0; j < m_numOutput; j += 1) {
    m_layerOutput[j] = 0.0;

    for(uint k = 0; k < m_numHidden; k += 1) {
      m_layerOutput[j] += m_layerHidden[k] * m_weightsHO[k][j];
    }

    m_layerOutput[j] = this->activationFunction( m_layerOutput[j] );
  }

  std::vector<int> output;
  for(uint i = 0; i < m_numOutput; i += 1) {
    int pinnedOutput = this->terminationFunction( m_layerOutput[i] );
    output.push_back(pinnedOutput);
  }

  return output;
}

void Brain::initLayer(Layer& layer, uint numNeurons) {
  layer.clear();

  for (uint i = 0; i < numNeurons; i += 1) {
    layer.push_back(0.0);
  }
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

// How to print a brain
std::ostream& operator<< (std::ostream& os, Brain const& b) {
  os << "<Brain"
     << " ϕ:" << b.m_fitness
     << " i:" << b.m_numInput
     << " h:" << b.m_numHidden
     << " o:" << b.m_numOutput
     << ">";
  return os;
}