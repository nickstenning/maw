/*
 *  ga.h
 *  maw
 *
 *  Created by Nick Stenning on 22/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#ifndef GA_H
#define GA_H

#include <vector>
#include <numeric>
#include <assert.h>

#include "Pendulum.h"
#include "Brain.h"

#include "util.h"

#define NUM_NEURONS_INPUT  2
#define NUM_NEURONS_HIDDEN 10
#define NUM_NEURONS_OUTPUT 1
#define MUTATION_RATE      0.005

namespace ga {

  typedef std::vector<Brain> population;

  void init (population&, size_t);
  void stepGeneration (population&);

  bool compareFitness (Brain const& a, Brain const& b);
  double sumFitness (double& a, Brain const& b);
  void computeFitness (Brain& brain);
  void computeFitness (Brain& brain, bool print);

  std::vector<size_t> piePick (std::vector<double> const& pie, size_t numToPick);

  Brain recombine (Brain const&, Brain const&);

  void mutate (Brain&);

}

#endif // GA_H