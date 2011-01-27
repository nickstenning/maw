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

    // Generate rank-ordered list by fitness, fittest last.
    std::sort(pop.begin(), pop.end(), compareFitness);

    // Elitist step. We preserve some proportion of the population untouched
    // for the next generation.
    size_t numElite = round(ELITISM * pop.size());
    size_t numRemain = pop.size() - numElite;
    for (population::iterator it = pop.end() - 1; it != pop.end() - 1 - numElite; --it) {
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

  void computeFitness(Brain& brain, bool print) {
    Pendulum pdl;
    double fitness = 0.0;

    if (print) {
      // Only do one run, and print it.
      pdl.ang(util::rand(-SCORE_ANG, SCORE_ANG))
         .vel(0.0)
         .time(0.0);

      computeFitnessForRun(brain, pdl, true);

    } else {
      for (size_t i = 0; i < NUM_RUNS; i += 1) {
        pdl.ang(util::rand(-SCORE_ANG, SCORE_ANG))
           .vel(0.0)
           .time(0.0);

        fitness += computeFitnessForRun(brain, pdl);
      }
      brain.fitness(fitness / NUM_RUNS);
    }
  }

  double computeFitnessForRun(Brain& brain, Pendulum& pdl, bool print) {
    double fitness = 0.0;
    std::vector<double> input;
    std::vector<int> output;

    if (print) {
      std::cout << "# t\ttheta\tthetadot\ttorque\tfitness\n";
    }

    while (pdl.time() < MAX_EVAL_TIME) {
      input.clear();

      input.push_back(pdl.ang());
      input.push_back(pdl.vel());

      output = brain.feedForward(input);

      pdl.step(BANG_SIZE * output[0] + util::rand(-NOISE_LEVEL, NOISE_LEVEL));

      if (print) {
        std::cout << pdl.time() << "\t" << pdl.ang() << "\t"
                  << (pdl.vel() / pdl.length) << "\t" << BANG_SIZE * output[0] << "\t"
                  << fitness << "\n";
      }

      bool inScoringZone = abs(pdl.ang()) < SCORE_ANG;
      if (inScoringZone) {
        fitness += pdl.dt * util::diracDelta(pdl.ang());
      } else {
        break; // Failure. No need to evaluate further.
      }
    }

    return fitness;
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

    // Mutate each weight with a probability of 1/MUTATION_RATE by
    // adding num in [-MUTATION_SIZE,MUTATION_SIZE)
    for (i = 0; i < wHidden.size(); i += 1) {
      for (j = 0; j < wHidden[i].size(); j += 1) {
        if (util::choose(MUTATION_RATE)) {
          // std::cerr << "mI" << j << "H" << i << "\n";
          wHidden[i][j] += util::rand(-MUTATION_SIZE, MUTATION_SIZE);
        }
      }
    }

    for (i = 0; i < wOutput.size(); i += 1) {
      for (j = 0; j < wOutput[i].size(); j += 1) {
        if (util::choose(MUTATION_RATE)) {
          // std::cerr << "mH" << j << "O" << i << "\n";
          wOutput[i][j] += util::rand(-MUTATION_SIZE, MUTATION_SIZE);
        }
      }
    }

    brain.weightsHidden(wHidden);
    brain.weightsOutput(wOutput);
  }

  bool compareFitness (Brain const& a, Brain const& b) {
    return (a.fitness() < b.fitness());
  }

  double sumFitness (double& a, Brain const& b) {
    return (a + b.fitness());
  }

  void printProperties (std::ostream& os) {
    os << "# INPUT_SIZE     = " << INPUT_SIZE     << "\n"
       << "# HIDDEN_SIZE    = " << HIDDEN_SIZE    << "\n"
       << "# OUTPUT_SIZE    = " << OUTPUT_SIZE    << "\n"
       << "# ELITISM        = " << ELITISM        << "\n"
       << "# CROSSOVER_PROB = " << CROSSOVER_PROB << "\n"
       << "# MUTATION_PROB  = " << MUTATION_PROB  << "\n"
       << "# MUTATION_RATE  = " << MUTATION_RATE  << "\n"
       << "# MUTATION_SIZE  = " << MUTATION_SIZE  << "\n"
       << "# NUM_RUNS       = " << NUM_RUNS       << "\n"
       << "# BANG_SIZE      = " << BANG_SIZE      << "\n"
       << "# MAX_EVAL_TIME  = " << MAX_EVAL_TIME  << "\n"
       << "# SCORE_ANG      = " << SCORE_ANG      << "\n";
  }

}


