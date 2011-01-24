#ifndef GA_H
#define GA_H

#include <iostream>
#include <vector>

// NN properties
#define INPUT_SIZE  2
#define HIDDEN_SIZE 5
#define OUTPUT_SIZE 1

// Proportion of the fitness-rank-ordered population guaranteed to be saved.
#define ELITISM 0.1

// Crossover probability for any given member of the new generation.
#define CROSSOVER_PROB 0.1

// Mutation probability (chance we do any mutation at all)
#define MUTATION_PROB 0.9
// Mutation rate (chance of mutation any one neuron).
// Maybe set at 1 / number of neurons available to mutate?
#define MUTATION_RATE 0.1
// Size of weight change
#define MUTATION_SIZE 50.0

// How many random init conds to average over?
#define NUM_RUNS 3

// Pendulum config
#define BANG_SIZE 0.3
#define EVAL_TIME 10.0
#define PI        3.141592653589793238462643
#define INIT_ANG  PI/20.0
#define SCORE_ANG PI/24.0

class Brain;
class Pendulum;

namespace ga {

  typedef std::vector<Brain> population;

  void init (population&, size_t);
  void stepGeneration (population&);

  bool compareFitness (Brain const& a, Brain const& b);
  double sumFitness (double& a, Brain const& b);
  void computeFitness (Brain& brain, bool print = false);
  double computeFitnessForRun (Brain& brain, Pendulum& pdl, bool print = false);

  Brain piePick (population const& pop);

  void crossover (Brain&, Brain const&);
  void mutate (Brain&);

  void printProperties (std::ostream& os);
}

#endif // GA_H