/*
 *  Brain.cpp
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include "Brain.h"

Brain::Brain(uint numInput, uint numHidden, uint numOutput)
: m_numInput(numInput)
, m_numHidden(numHidden)
, m_numOutput(numOutput)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH()
, m_weightsHO()
, m_fitness(0.0)
{
  initLayer(m_layerInput, m_numInput);
  initLayer(m_layerHidden, m_numHidden);
  initLayer(m_layerOutput, m_numOutput);

  initWeights(m_weightsIH, m_numInput, m_numHidden, ^ (uint, uint) {
    // Set I->H weights in range [-1/sqrt(numInput), 1/sqrt(numInput)]
    //double ::lim = 1 / sqrt( static_cast<double> (numInput) );
    //return ((NORMRAND() * 2 * lim) - lim);

    return ((NORMRAND() * 2) - 1);
  });

  initWeights(m_weightsHO, m_numHidden, m_numOutput, ^ (uint, uint) {
    // Set I->H weights in range [-1/sqrt(numHidden), 1/sqrt(numHidden)]
    //double ::lim = 1 / sqrt( static_cast<double> (numHidden) );
    //return ((NORMRAND() * 2 * lim) - lim);

    return ((NORMRAND() * 2) - 1);
  });
}

Brain::Brain(uint numInput, uint numHidden, uint numOutput, weightBlock block)
: m_numInput(numInput)
, m_numHidden(numHidden)
, m_numOutput(numOutput)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH()
, m_weightsHO()
, m_fitness(0.0)
{
  initLayer(m_layerInput, m_numInput);
  initLayer(m_layerHidden, m_numHidden);
  initLayer(m_layerOutput, m_numOutput);

  initWeights(m_weightsIH, m_numInput, m_numHidden, block);
  initWeights(m_weightsHO, m_numHidden, m_numOutput, block);
}

Brain::~Brain() {}

double const& Brain::fitness() const {
  return m_fitness;
}

Brain const& Brain::fitness(double const& f) {
  m_fitness = f;
  return *this;
}

Weights Brain::weightsHidden() const {
  return m_weightsIH;
}

Weights Brain::weightsOutput() const {
  return m_weightsHO;
}

Brain const& Brain::weightsHidden(Weights const& w) {
  initWeights(m_weightsIH, m_numInput, m_numHidden, ^ (uint i, uint j) {
    return w[i][j];
  });
  return *this;
}

Brain const& Brain::weightsOutput(Weights const& w) {
  initWeights(m_weightsHO, m_numHidden, m_numOutput, ^ (uint i, uint j) {
    return w[i][j];
  });
  return *this;
}

std::vector<double> Brain::feedForward (std::vector<double> const& input) {
  for(uint i = 0; i < m_numInput; i += 1) m_layerInput[i] = input[i];

  // Propagate to hidden layer
  for(uint j = 0; j < m_numHidden; j += 1) {
    m_layerHidden[j] = 0.0;

    for(uint k = 0; k < m_numInput; k += 1) {
      m_layerHidden[j] += m_layerInput[k] * m_weightsIH[k][j];
    }

    m_layerHidden[j] = activationFunction( m_layerHidden[j] );
  }

  // Propagate to output layer
  for(uint j = 0; j < m_numOutput; j += 1) {
    m_layerOutput[j] = 0.0;

    for(uint k = 0; k < m_numHidden; k += 1) {
      m_layerOutput[j] += m_layerHidden[k] * m_weightsHO[k][j];
    }

    //m_layerOutput[j] = activationFunction( m_layerOutput[j] );
  }

  std::vector<double> output;
  for(uint i = 0; i < m_numOutput; i += 1) output.push_back(m_layerOutput[i]);

  return output;
}

void Brain::initLayer(Layer& layer, uint numNeurons) {
  layer.clear();

  for (uint i = 0; i < numNeurons; i += 1) {
    layer.push_back(0.0);
  }
}

void Brain::initWeights (Weights& weights, uint numLayer1, uint numLayer2, double (^block)(uint, uint)) {
  weights.clear();

  for (uint i = 0; i < numLayer1; i += 1) {
    std::vector<double> x;
    weights.push_back(x);

    for (uint j = 0; j < numLayer2; j += 1) {
      weights[i].push_back(block(i, j));
    }
  }
}

inline double Brain::activationFunction (double x) {
  // Sigmoid
  //return 1 / (1 + exp(-10*x));

  // Direct
  return x;
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