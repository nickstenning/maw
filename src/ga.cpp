#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <assert.h>

#include "pendulum.h"
#include "brain.h"
#include "util.h"
#include "ga.h"

namespace ga {

  void init (population& pop, size_t popSize) {
    for (size_t i = 0; i < popSize; i += 1) {
      Brain b(INPUT_SIZE, HIDDEN_SIZE, OUTPUT_SIZE);
      b.initRandomWeights();
      computeFitness(b);
      pop.push_back(b);
    }
  }

  void stepGeneration (population& pop) {
    population newPop;

    // Generate rank-ordered list by fitness.
    std::sort(pop.begin(), pop.end(), compareFitness);

    // Elitist step. We preserve some proportion of the population untouched
    // for the next generation.
    size_t numElite = round(ELITISM * pop.size());
    size_t numRemain = pop.size() - numElite;
    for (population::iterator it = pop.begin(); it != pop.begin() + numElite; ++it) {
      newPop.push_back(*it);
    }

    // Socially liberal step.
    for (size_t i = 0; i < numRemain; i += 1) {
      Brain offspring = piePick(pop);

      if (util::choose(CROSSOVER_PROB)) {
        crossover(offspring, piePick(pop));
      }

      if (util::choose(MUTATION_PROB)) {
        mutate(offspring);
      }

      newPop.push_back(offspring);
    }

    // Replace population with new population.
    pop = newPop;

    // Compute fitness for next generation
    for (population::iterator it = pop.begin(); it != pop.end(); ++it) {
      computeFitness(*it);
    }
  }

  void computeFitness (Brain& brain, bool print) {
    Pendulum pdl;

    double fitness = 0.0;

    std::vector<double> input;
    std::vector<int> output;

    pdl.ang(util::rand(-INIT_ANG, INIT_ANG));
    pdl.vel(0);

    while (pdl.time() < EVAL_TIME) {
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

      bool inDelta = abs(pdl.ang()) < SCORE_ANG;
      if (inDelta) fitness += (pdl.dt / EVAL_TIME);
    }

    brain.fitness(fitness);
  }

  Brain piePick (population const& pop) {
    double pieSize = std::accumulate(pop.begin(), pop.end(), 0.0, sumFitness);
    double subTotal = 0.0;
    double pickPoint = util::rand(0, pieSize);

    population::const_iterator pick;

    // Find the index that that point in the pie corresponds to.
    for (population::const_iterator it = pop.begin(); it != pop.end(); ++it) {
      subTotal += it->fitness();
      if (subTotal >= pickPoint) pick = it;
    }

    return *pick;
  }

  void crossover (Brain& brain, Brain const& other) {
    size_t i, j;

    assert(brain.topologyIsCompatibleWith(other));

    Weights hidden = brain.weightsHidden();
    Weights output = brain.weightsOutput();
    Weights otherHidden = other.weightsHidden();
    Weights otherOutput = other.weightsOutput();

    // For each non-input neuron, randomly choose a parent, and copy all
    // input weights from that parent.
    for (j = 0; j < hidden[0].size(); j += 1) {
      if (util::choose(0.5)) {
        for (i = 0; i < hidden.size(); i += 1) {
          hidden[i][j] = otherHidden[i][j];
        }
      }
    }

    for (j = 0; j < output[0].size(); j += 1) {
      if (util::choose(0.5)) {
        for (i = 0; i < output.size(); i += 1) {
          output[i][j] = otherOutput[i][j];
        }
      }
    }

    brain.weightsHidden(hidden)
         .weightsOutput(output);
  }

  void mutate (Brain& brain) {
    size_t i, j;

    Weights wHidden = brain.weightsHidden();
    Weights wOutput = brain.weightsOutput();

    // Mutate each weight with a probability of 1/MUTATION_RATE by adding num in [-1,1)
    for (i = 0; i < wHidden.size(); i += 1) {
      for (j = 0; j < wHidden[i].size(); j += 1) {
        if (util::choose(MUTATION_RATE)) {
          wHidden[i][j] += util::rand(-1, 1);
        }
      }
    }

    for (i = 0; i < wOutput.size(); i += 1) {
      for (j = 0; j < wOutput[i].size(); j += 1) {
        if (util::choose(MUTATION_RATE)) {
          wOutput[i][j] += util::rand(-1, 1);
        }
      }
    }

    brain.weightsHidden(wHidden);
    brain.weightsOutput(wOutput);
  }

  bool compareFitness (Brain const& a, Brain const& b) {
    return (a.fitness() > b.fitness());
  }

  double sumFitness (double& a, Brain const& b) {
    return (a + b.fitness());
  }

}


