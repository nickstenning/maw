/*
 *  Brain.cpp
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include "Brain.h"

Brain::Brain(int id, int numInput, int numHidden, int numOutput)
: m_id(id)
, m_numInput(numInput)
, m_numHidden(numHidden)
, m_numOutput(numOutput)
, m_layerInput()
, m_layerHidden()
, m_layerOutput()
, m_weightsIH()
, m_weightsHO()
, m_fitness(0.0)
{
  initLayer(m_layerInput, m_numInput, true);
  initLayer(m_layerHidden, m_numHidden, true);
  initLayer(m_layerOutput, m_numOutput, false);

  // Set up bias neurons
  m_layerInput[m_numInput] = -1;
  m_layerHidden[m_numHidden] = -1;

  initWeights(m_weightsIH, m_numInput + 1, m_numHidden, ^ (int, int) {
    // Set I->H weights in range [-1/sqrt(numInput), 1/sqrt(numInput)]
    double ::lim = 1 / sqrt( static_cast<double> (numInput) );
    return ((NORMRAND() * 2 * lim) - lim);
  });

  initWeights(m_weightsHO, m_numHidden + 1, m_numOutput, ^ (int, int) {
    // Set I->H weights in range [-1/sqrt(numHidden), 1/sqrt(numHidden)]
    double ::lim = 1 / sqrt( static_cast<double> (numHidden) );
    return ((NORMRAND() * 2 * lim) - lim);
  });
}

Brain::~Brain() {}

int const& Brain::id() const {
  return m_id;
}

double const& Brain::fitness() const {
  return m_fitness;
}

Brain const& Brain::fitness(double const& fitness) {
  m_fitness = fitness;
  return *this;
}

std::vector<double> Brain::feedForward(std::vector<double> const& input)
{
  for(int i = 0; i < m_numInput; i += 1) m_layerInput[i] = input[i];

  // Propagate to hidden layer
  for(int j = 0; j < m_numHidden; j += 1) {
    m_layerHidden[j] = 0.0;

    // equality because last entry is bias neuron
    for(int k = 0; k <= m_numInput; k += 1) {
      m_layerHidden[j] += m_layerInput[k] * m_weightsIH[k][j];
    }

    m_layerHidden[j] = activationFunction( m_layerHidden[j] );
  }

  // Propagate to output layer
  for(int j = 0; j < m_numOutput; j += 1) {
    m_layerOutput[j] = 0.0;

    // equality because last entry is bias neuron
    for(int k = 0; k <= m_numHidden; k += 1) {
      m_layerOutput[j] += m_layerHidden[k] * m_weightsHO[k][j];
    }

    m_layerOutput[j] = activationFunction( m_layerOutput[j] );
  }

  std::vector<double> output;
  for(int i = 0; i < m_numOutput; i += 1) output.push_back(m_layerOutput[i]);

  return output;
}

void Brain::initLayer(Layer& layer, int numNeurons, bool bias) {
  layer.clear();

  int length = bias ? numNeurons + 1: numNeurons;

  for (int i = 0; i < length; i += 1) {
    layer.push_back(0.0);
  }
}

void Brain::initWeights(Weights& weights, int numLayer1, int numLayer2, double (^block)(int, int)) {
  weights.clear();

  for (int i = 0; i < numLayer1; i += 1) {
    std::vector<double> x;
    weights.push_back(x);

    for (int j = 0; j < numLayer2; j += 1) {
      weights[i].push_back(block(i, j));
      //std::cout << i << " -> " << j << " @ " << weights[i][j] << "\n";
    }
  }
}

inline double Brain::activationFunction( double x ) {
  // Sigmoid
  //return 1 / (1 + exp(-x));

  // Direct
  return x;
}

// How to print a brain
std::ostream& operator<< (std::ostream& os, Brain const& b) {
  os << "<Brain " << b.m_id
     << " ϕ:" << b.m_fitness
     << " i:" << b.m_numInput
     << " h:" << b.m_numHidden
     << " o:" << b.m_numOutput
     << ">";
  return os;
}