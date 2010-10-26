/*
 *  Brain.h
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#ifndef BRAIN_H
#define BRAIN_H

#include <iostream>
#include <vector>
#include <cmath>

#include "util.h"

typedef std::vector< double > Layer;
typedef std::vector< std::vector<double> > Weights;

class Brain {
public:
  Brain(int id, int numInput, int numHidden, int numOutput);
  ~Brain();

  int const& id() const;

  double const& fitness() const;
  Brain const& fitness(double const&);

  std::vector<double> feedForward(std::vector<double> const& input);

  friend std::ostream& operator<<(std::ostream&, Brain const&);

protected:
  int m_id;
  double m_fitness;

  int m_numInput;
  int m_numHidden;
  int m_numOutput;

  Layer m_layerInput;
  Layer m_layerHidden;
  Layer m_layerOutput;

  Weights m_weightsIH;
  Weights m_weightsHO;

  void initLayer(Layer& l, int numNeurons, bool bias);
  void initWeights(Weights& w, int numLayer1, int numLayer2, double (^block)(int, int));

  inline double activationFunction(double x);

  void cloneBrain(Brain const& from);
};

std::ostream& operator<<(std::ostream&, std::vector<Brain> const&);

#endif // BRAIN_H