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

#include <vector>

typedef std::vector< double > Layer;
typedef std::vector< std::vector<double> > Weights;
typedef unsigned int uint;

class BrainDotPrinter;

class Brain {
public:
  Brain(uint numInput, uint numHidden, uint numOutput);
  ~Brain();

  double const& fitness() const;
  Brain const& fitness(double const&);

  Weights const& weightsHidden() const;
  Brain& weightsHidden(Weights const&);
  Weights const& weightsOutput() const;
  Brain& weightsOutput(Weights const&);

  std::vector<int> feedForward(std::vector<double> const& input);

  friend std::ostream& operator<<(std::ostream&, Brain const&);
  friend class BrainDotPrinter;

protected:
  double m_fitness;

  uint m_numInput;
  uint m_numHidden;
  uint m_numOutput;

  Layer m_layerInput;
  Layer m_layerHidden;
  Layer m_layerOutput;

  Weights m_weightsIH;
  Weights m_weightsHO;

  void initLayer(Layer& l, uint numNeurons);

  inline double activationFunction(double x);
  inline int    terminationFunction(double x);
};

// Define operator for template in "util.h"
std::ostream& operator<<(std::ostream&, std::vector<Brain> const&);

#endif // BRAIN_H