#include <vector>
#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "ga.h"
#include "brain.h"
#include "brain_dot_printer.h"
#include "util.h"

#define COLWIDTH        10
#define COL             std::setw( COLWIDTH )

int main (int argc, char* const argv[]) {
  unsigned int seed = util::initRNG();
  std::cout << "# RNG_SEED = " << seed << "\n";

  Brain const* fittest = NULL;

  int numGenerations = 5;

  if (argc == 2) {
    std::cout << atoi(argv[1]);
    numGenerations = atoi(argv[1]);

  }

  // Initialise GA population
  GA::Runner runner(numGenerations);

  std::cerr << std::left;
  std::cerr << COL << "# gen"
            << COL << "minFit"
            << COL << "maxFit"
            << COL << "meanFit"
            << COL << "sdFit" << "\n";

  // GA
  while (!runner.isFinished()) {
    runner.step();

    fittest = &runner.getFittest();

    int gen;
    double min, max, mean, stddev;

    runner.fillStats(gen, min, max, mean, stddev);

    // Print generation stats
    std::cerr << COL << gen
              << COL << min
              << COL << max
              << COL << mean
              << COL << stddev << "\n";
  }

  // print data for run to stdout
  std::cout << "# FITNESS = " << fittest->fitness() << "\n"
            << "# \n";

  runner.printProperties(std::cout);

  std::cout << "# \n";

  BrainDotPrinter bp(std::cout);
  bp.prefix("# ");
  bp << *fittest << "\n";

  // Make a copy we can futz with:
  Brain fittestRunnable = *fittest;
  runner.printRunData(std::cout, fittestRunnable);

  // Clean up:
  fittest = NULL;
  delete fittest;

  return 0;
}
