#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <assert.h>

#include "pendulum.h"
#include "brain.h"
#include "util.h"
#include "ga.h"

namespace GA {

  /**
   * Feed the state variables into the brain, feed forward, and advance the
   * physics using the output of the brain as a controlling torque.
   *
   * @return Applied control torque
  **/
  double stepPhysics(Brain& brain, Pendulum& pdl) {
    std::vector<double> input;
    std::vector<int> output;

    input.push_back(pdl.ang());
    input.push_back(pdl.vel());

    output = brain.feedForward(input);

    double controlTorque = BANG_SIZE * output[0];
    double noiseTorque   = util::rand(-NOISE_LEVEL, NOISE_LEVEL);

    pdl.step(controlTorque + noiseTorque);

    return controlTorque;
  }

  /**
   * Return the fitness increment for the current timestep. Called once per
   * physics timestep.
   *
   * @return Status: -1 results in evaluation being aborted.
  **/
  int stepFitness(double& fitness, Pendulum& pdl) {
    bool inScoringZone = abs(pdl.ang()) < SCORE_ANG;
    if (inScoringZone) {
      fitness += pdl.dt * util::diracDelta(pdl.ang(), 5);
    } else {
      return -1; // Failure. No need to evaluate further.
    }

    return 0;
  }

  Runner::Runner (size_t numGenerations, size_t popSize)
  : m_pop(popSize, Brain(INPUT_SIZE, HIDDEN_SIZE, OUTPUT_SIZE))
  , m_numGenerations(numGenerations)
  , m_generation(0)
  {
    for (Population::iterator it = m_pop.begin(); it != m_pop.end(); ++it) {
      it->initRandomWeights();
      computeFitness(*it);
    }
    std::sort(m_pop.begin(), m_pop.end());
  }

  bool Runner::isFinished () const {
    return m_generation >= m_numGenerations;
  }

  void Runner::step () {
    Population newPop;

    // Elitist step. We preserve some proportion of the population untouched
    // for the next generation.
    size_t numElite = round(ELITISM * m_pop.size());
    size_t numRemain = m_pop.size() - numElite;
    for (Population::iterator it = m_pop.end() - 1; it != m_pop.end() - 1 - numElite; --it) {
      newPop.push_back(*it);
    }

    // Socially liberal step.
    for (size_t i = 0; i < numRemain; i += 1) {
      Brain offspring = piePick(m_pop);

      if (util::choose(CROSSOVER_PROB)) {
        crossover(offspring, piePick(m_pop));
      }

      if (util::choose(MUTATION_PROB)) {
        mutate(offspring);
      }

      newPop.push_back(offspring);
    }

    // Replace population with new population.
    m_pop = newPop;

    // Compute fitness for next generation
    for(Population::iterator it = m_pop.begin(); it != m_pop.end(); ++it) {
      updateFitness(*it);
    }

    // Generate rank-ordered list by fitness, fittest last.
    std::sort(m_pop.begin(), m_pop.end());

    // Update the generation count
    m_generation += 1;
  }

  void Runner::updateFitness (Brain& brain) {
    double fitness = 0.0;

    for (size_t i = 0; i < NUM_RUNS; i += 1) {
      fitness += computeFitness(brain);
    }

    brain.fitness(fitness / NUM_RUNS);
  }

  void Runner::fillStats (int& gen, double& min, double& max, double& mean, double& stddev) const {
    gen = m_generation;
    min = m_pop[0].fitness();
    max = m_pop[m_pop.size() - 1].fitness();
    mean = std::accumulate(m_pop.begin(), m_pop.end(), 0.0) / m_pop.size();

    double sumsqdev = 0.0;

    for (Population::const_iterator it = m_pop.begin(); it != m_pop.end(); ++it) {
      sumsqdev += pow(it->fitness() - mean, 2);
    }

    stddev = sqrt(sumsqdev / m_pop.size());
  }

  Brain const& Runner::getFittest () const {
    return m_pop[m_pop.size() - 1];
  }

  void Runner::printRunData (std::ostream& os, Brain& brain) {
    double controlTorque;
    Pendulum pdl;

    os << "# t\ttheta\tthetadot\ttorque\n";

    for (size_t i = 0; i < NUM_RUNS; i += 1) {

      pdl.ang(util::rand(-SCORE_ANG, SCORE_ANG))
         .vel(0.0)
         .time(0.0);

      os << "# run " << i << "\n";

      while (pdl.time() < MAX_EVAL_TIME) {
        controlTorque = stepPhysics(brain, pdl);

        os << pdl.time() << "\t"
           << pdl.ang() << "\t"
           << (pdl.vel() / pdl.length) << "\t"
           << controlTorque << "\n";
      }

    }
  }

  void Runner::printProperties (std::ostream& os) {
    os << "# POP_SIZE        = " << m_pop.size()     << "\n"
       << "# NUM_GENERATIONS = " << m_numGenerations << "\n"
       << "# INPUT_SIZE      = " << INPUT_SIZE       << "\n"
       << "# HIDDEN_SIZE     = " << HIDDEN_SIZE      << "\n"
       << "# OUTPUT_SIZE     = " << OUTPUT_SIZE      << "\n"
       << "# ELITISM         = " << ELITISM          << "\n"
       << "# CROSSOVER_PROB  = " << CROSSOVER_PROB   << "\n"
       << "# MUTATION_PROB   = " << MUTATION_PROB    << "\n"
       << "# MUTATION_RATE   = " << MUTATION_RATE    << "\n"
       << "# MUTATION_SIZE   = " << MUTATION_SIZE    << "\n"
       << "# NUM_RUNS        = " << NUM_RUNS         << "\n"
       << "# BANG_SIZE       = " << BANG_SIZE        << "\n"
       << "# MAX_EVAL_TIME   = " << MAX_EVAL_TIME    << "\n"
       << "# SCORE_ANG       = " << SCORE_ANG        << "\n";
  }

  double computeFitness(Brain& brain) {
    Pendulum pdl;
    double fitness = 0.0;

    pdl.ang(util::rand(-SCORE_ANG, SCORE_ANG));

    while (pdl.time() < MAX_EVAL_TIME) {
      stepPhysics(brain, pdl);
      if (stepFitness(fitness, pdl) == -1) break;
    }

    return fitness;
  }

  Brain piePick (Population const& pop) {
    double pieSize = std::accumulate(pop.begin(), pop.end(), 0.0);
    double subTotal = 0.0;
    double pickPoint = util::rand(0, pieSize);

    Population::const_iterator pick;

    // Find the index that that point in the pie corresponds to.
    for (Population::const_iterator it = pop.begin(); it != pop.end(); ++it) {
      subTotal += it->fitness();
      if (subTotal >= pickPoint) {
        pick = it;
        break;
      }
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

}


