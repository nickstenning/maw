#include <vector>
#include <iostream>
#include <iomanip>
#include <numeric>

#include "ga.h"
#include "brain.h"

#define POP_SIZE        20
#define NUM_GENERATIONS 5

#define COLWIDTH        10
#define COL             std::setw( COLWIDTH )

int main (int argc, char* const argv[]) {
  // Set up RNG
  unsigned int seed;

  if (argc == 2) {
    seed = atoi(argv[1]);
  } else {
    seed = std::time(0);
  }

  srand(seed);
  std::cerr << "Initialised RNG with seed " << seed << "\n";

  // Initialise GA population
  ga::population pop;
  ga::init(pop, POP_SIZE);

  std::cerr << std::left;
  std::cerr << COL << "# gen"
            << COL << "minFit"
            << COL << "maxFit"
            << COL << "meanFit" << "\n";

  // GA
  for (size_t i = 0; i < NUM_GENERATIONS; i += 1) {
    ga::stepGeneration(pop);

    // Sort generation by fitness
    std::sort(pop.begin(), pop.end(), ga::compareFitness);

    double meanFitness = std::accumulate(pop.begin(), pop.end(), 0.0, ga::sumFitness) / pop.size();

    // Print generation stats
    std::cerr << COL << i
              << COL << pop[pop.size() - 1].fitness()
              << COL << pop[0].fitness()
              << COL << meanFitness << "\n";
  }

  // print data for run to stdout
  // ga::computeFitness(pop[0], true);
  // std::cout << "# And worst one:\n";
  ga::computeFitness(pop[pop.size() - 1], true);

  return 0;
}
