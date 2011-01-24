#ifndef GA_H
#define GA_H

#include <vector>

// NN properties
#define INPUT_SIZE  2
#define HIDDEN_SIZE 5
#define OUTPUT_SIZE 1

// Proportion of the fitness-rank-ordered population guaranteed to be saved.
#define ELITISM 0.25

// Crossover probability for any given member of the new generation.
#define CROSSOVER_PROB 0.2

// Mutation probability (chance we do any mutation at all)
#define MUTATION_PROB 0.8
#define MUTATION_RATE 0.005
#define MUTATION_SIZE 1.0

// Pendulum config
#define EVAL_TIME 100.0
#define PI        3.141592653589793238462643
#define INIT_ANG  PI/20.0
#define SCORE_ANG PI/20.0

class Brain;

namespace ga {

  typedef std::vector<Brain> population;

  void init (population&, size_t);
  void stepGeneration (population&);

  bool compareFitness (Brain const& a, Brain const& b);
  double sumFitness (double& a, Brain const& b);
  void computeFitness (Brain& brain, bool print = false);

  Brain piePick (population const& pop);

  void crossover (Brain&, Brain const&);
  void mutate (Brain&);

}

#endif // GA_H