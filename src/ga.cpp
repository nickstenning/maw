/*
 *  ga.cpp
 *  maw
 *
 *  Created by Nick Stenning on 22/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include "ga.h"

#include <iostream>
#include <vector>
#include <numeric>
#include <assert.h>

#include "Pendulum.h"
#include "Brain.h"
#include "util.h"

namespace ga {

  void init (population& pop, size_t popSize) {
    for (size_t i = 0; i < popSize; i += 1) {
      Brain b(NUM_NEURONS_INPUT, NUM_NEURONS_HIDDEN, NUM_NEURONS_OUTPUT);
      computeFitness(b);
      pop.push_back(b);
    }
  }

  void stepGeneration (population& pop) {
    std::vector<double> pie;

    // Add fitness slices to virtual pie.
    for (population::iterator it = pop.begin(); it != pop.end(); ++it) {
      pie.push_back(it->fitness());
    }

    // Pick all the parents from the pie.
    std::vector<size_t> parents = piePick(pie, pop.size() * 2);

    population newPop;

    for (size_t i = 0; i < pop.size(); i += 1) {
      Brain mum = pop[parents[2 * i]];
      Brain dad = pop[parents[2 * i + 1]];

      // Cross-over
      Brain offspring = recombine(mum, dad);

      // Mutate
      mutate(offspring);

      // Add to new population
      newPop.push_back(offspring);
    }

    pop.clear();

    for (size_t j = 0; j < newPop.size(); j += 1) {
      pop.push_back(newPop[j]);
    }

    // Compute fitness for next generation
    for (population::iterator it = pop.begin(); it != pop.end(); ++it) {
      computeFitness(*it);
    }
  }

  bool compareFitness (Brain const& a, Brain const& b) { return (a.fitness() > b.fitness()); }
  double sumFitness (double& a, Brain const& b) { return (a + b.fitness()); }

  void computeFitness(Brain& brain) {
    computeFitness(brain, false);
  }

  void computeFitness (Brain& brain, bool print) {
    Pendulum pdl;

    double fitness = 1.0;

    std::vector<double> input;
    std::vector<int> output;

    pdl.ang(util::rand(-pdl.pi/20, pdl.pi/20));
    pdl.vel(0);

    while (pdl.time() < 100) {
      input.clear();

      input.push_back(pdl.ang());
      input.push_back(pdl.vel());

      output = brain.feedForward(input);

      pdl.step(output[0]);

      if (print) {
        std::cout << pdl.time() << "\t" << pdl.ang() << "\t"
                  << pdl.vel() << "\t" << output[0] << "\t"
                  << fitness << "\n";
      }

      double angScore = util::diracDelta(pdl.ang(), 100);
      double velScore = util::diracDelta(pdl.vel(), 1);

      fitness += angScore * velScore;
    }

    brain.fitness(fitness);
  }

  std::vector<size_t> piePick (std::vector<double> const& pie, size_t numToPick) {
    double pieSize = std::accumulate(pie.begin(), pie.end(), 0);

    std::vector<size_t> picks;

    // Pick numToPick indices
    for (size_t i = 0; i < numToPick; i += 1) {

      double subTotal = 0.0;
      double pickPoint = util::rand(0, pieSize);

      // Find the index that that point in the pie corresponds to.
      for (size_t j = 0; j < pie.size(); j += 1) {
        subTotal += pie[j];
        if (subTotal >= pickPoint) {
          picks.push_back(j);
          break;
        }
      }
    }

    return picks;
  }

  Brain recombine (Brain const& a, Brain const& b) {
    size_t i, j;

    Weights aHidden = a.weightsHidden();
    Weights aOutput = a.weightsOutput();

    Weights bHidden = b.weightsHidden();
    Weights bOutput = b.weightsOutput();

    bool parentsHaveSameTopology = (aHidden.size() == bHidden.size() &&
                                    aHidden[0].size() == bHidden[0].size() &&
                                    aOutput[0].size() == bOutput[0].size());

    assert(parentsHaveSameTopology);

    Brain offspring(aHidden.size(), aHidden[0].size(), aOutput[0].size());

    Weights hidden = offspring.weightsHidden();
    Weights output = offspring.weightsOutput();


    // For each non-input neuron, randomly choose a parent, and copy all
    // input weights from that parent.
    for (j = 0; j < hidden[0].size(); j += 1) {
      if (rand() % 2 == 0) { // choose a
        for (i = 0; i < hidden.size(); i += 1) {
          hidden[i][j] = aHidden[i][j];
        }
      } else { // choose b
        for (i = 0; i < hidden.size(); i += 1) {
          hidden[i][j] = bHidden[i][j];
        }
      }
    }

    for (j = 0; j < output[0].size(); j += 1) {
      if (rand() % 2 == 0) { // choose a
        for (i = 0; i < output.size(); i += 1) {
          output[i][j] = aOutput[i][j];
        }
      } else { // choose b
        for (i = 0; i < output.size(); i += 1) {
          output[i][j] = bOutput[i][j];
        }
      }
    }

    offspring.weightsHidden(hidden);
    offspring.weightsOutput(output);

    return offspring;
  }

  void mutate (Brain& brain) {
    size_t i, j;

    int mutate = 1/MUTATION_RATE;
    double val;

    Weights wHidden = brain.weightsHidden();
    Weights wOutput = brain.weightsOutput();

    // Mutate each weight with a probability of 1/200 by adding num in [-1,1]
    for (i = 0; i < wHidden.size(); i += 1) {
      for (j = 0; j < wHidden[i].size(); j += 1) {
        if (rand() % mutate == 0) {
          val = sqrt(-log(util::rand()));
          if (rand() % 2 == 1) {
            val = val * -1;
          }

          wHidden[i][j] += val;
        }
      }
    }

    for (i = 0; i < wOutput.size(); i += 1) {
      for (j = 0; j < wOutput[i].size(); j += 1) {
        if (rand() % mutate == 0) {
          val = sqrt(-log(util::rand()));
          if (rand() % 2 == 1) {
            val = val * -1;
          }

          wOutput[i][j] += val;
        }
      }
    }

    brain.weightsHidden(wHidden);
    brain.weightsOutput(wOutput);
  }

}


