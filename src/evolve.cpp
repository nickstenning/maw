#include <iostream>
#include <iomanip>
#include <signal.h>

#include "ga.h"
#include "brain.h"
#include "pendulum_fitness_function.h"
#include "util.h"

#define COL std::setw( 14 )

bool abortGA = false;

void abortEarly(int) {
  abortGA = true;
}

int main (int argc, char* const argv[]) {
  std::cout << std::setprecision(6) << std::fixed;
  std::cerr << std::setprecision(6) << std::fixed;

  // Send USR1 to the process to stop evolution early.
  signal(SIGUSR1, abortEarly);

  size_t numGenerations, populationSize;

  if (argc != 3) {
    std::cerr << "Usage: evolve <NUM_GENERATIONS> <POPULATION_SIZE>\n";
    return 1;
  } else {
    numGenerations = atoi(argv[1]);
    populationSize = atoi(argv[2]);
  }

  // Init RNG
  unsigned int seed = util::initRNG();
  std::cerr << "# RNG_SEED = " << seed << "\n";

  // Initialise GA population
  GA::Runner runner(
    new Brain(2, 1, 1),
    new PendulumFitnessFunction(),
    numGenerations,
    populationSize
  );

  std::cerr << std::left;
  std::cerr << COL << "# gen"
            << COL << "minFit"
            << COL << "maxFit"
            << COL << "meanFit"
            << COL << "sdFit" << "\n";

  // GA
  int gen;
  double min, max, mean, stddev;

  while (!runner.isFinished() and !abortGA) {
    runner.step();
    runner.fillStats(gen, min, max, mean, stddev);

    // Print generation stats
    std::cerr << COL << gen
              << COL << min
              << COL << max
              << COL << mean
              << COL << stddev << "\n";
  }

  // Print best solution to stdout
  Brain const* fittest = dynamic_cast<Brain const*>( runner.getFittest() );
  std::cout << *fittest << std::endl;

  return 0;
}
