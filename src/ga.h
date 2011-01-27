#ifndef GA_H
#define GA_H

#include <iostream>
#include <vector>

// GA properties
#define POP_SIZE        50
#define NUM_GENERATIONS 5

// NN properties
#define INPUT_SIZE  2
#define HIDDEN_SIZE 5
#define OUTPUT_SIZE 1

// Proportion of the fitness-rank-ordered population guaranteed to be saved.
#define ELITISM 0.2

// Crossover probability for any given member of the new generation.
#define CROSSOVER_PROB 0

// Mutation probability (chance we do any mutation at all)
#define MUTATION_PROB 1.0
// Mutation rate (chance of mutation any one weight).
// Maybe set at 1 / number of neurons available to mutate?
#define MUTATION_RATE 1.0 / ((INPUT_SIZE * HIDDEN_SIZE) + (HIDDEN_SIZE * OUTPUT_SIZE))
// Size of weight change
#define MUTATION_SIZE 4.0

// How many random init conds to average over?
#define NUM_RUNS 3

// Pendulum config
#define BANG_SIZE     10.0
#define NOISE_LEVEL   BANG_SIZE * 4.0
#define MAX_EVAL_TIME 100.0
#define PI            3.141592653589793238462643
#define SCORE_ANG     PI/2.0

class Brain;
class Pendulum;

namespace GA {

  typedef std::vector<Brain> Population;

  bool compareFitness (Brain const& a, Brain const& b);
  double sumFitness (double& a, Brain const& b);

  double computeFitness (Brain& brain);

  void crossover (Brain&, Brain const&);
  void mutate (Brain&);

  Brain piePick (Population const& pop);

  class Runner
  {
  public:
    Runner (size_t numGenerations = NUM_GENERATIONS, size_t popSize = POP_SIZE);

    bool isFinished () const;

    void step ();

    void updateFitness (Brain& brain);

    void fillStats(int& gen, double& min, double& max, double& mean, double& stddev) const;
    Brain const& getFittest() const;

    void printRunData (std::ostream& os, Brain& brain);
    void printProperties (std::ostream& os);

  protected:
    Population m_pop;

    size_t m_numGenerations;
    size_t m_generation;

  };

}

#endif // GA_H