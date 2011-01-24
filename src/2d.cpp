#include <vector>
#include <iostream>
#include <iomanip>
#include <numeric>

#include "ga.h"
#include "brain.h"
#include "util.h"

#define POP_SIZE        50
#define NUM_GENERATIONS 50

#define COLWIDTH        10
#define COL             std::setw( COLWIDTH )

int main (int /*argc*/, char* const /*argv*/[]) {
  unsigned int seed = util::initRNG();
  std::cout << "# RNG_SEED = " << seed << "\n";

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
  std::cout << "# FITNESS = " << pop[0].fitness() << "\n";
  std::cout << "# \n";
  ga::printProperties(std::cout);

  ga::computeFitness(pop[0], true);

  return 0;
}
