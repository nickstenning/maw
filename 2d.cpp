/*
 *  2d.cpp
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include <vector>
#include <iostream>
#include <iomanip>

#include "ga.h"

#include "BrainDotPrinter.h"

#define POP_SIZE        20
#define NUM_GENERATIONS 30
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
            << COL << "min"
            << COL << "max"
            << COL << "mean" << "\n";

  // GA
  for (size_t i = 0; i < NUM_GENERATIONS; i += 1) {
    ga::stepGeneration(pop);

    // Sort generation by fitness
    std::sort(pop.begin(), pop.end(), ga::compareFitness);

    double meanPhi = std::accumulate(pop.begin(), pop.end(), 0.0, ga::sumFitness) / pop.size();

    // Print generation stats
    std::cerr << COL << i
              << COL << pop[pop.size() - 1].fitness()
              << COL << pop[0].fitness()
              << COL << meanPhi << "\n";

    std::cerr << "# ";
    for (size_t j = 0; j < pop.size(); j += 1) {
      std::cerr << pop[j].fitness() << " ";
    }
    std::cerr << "\n";
  }

  BrainDotPrinter printer(std::cerr);

  std::cerr << "# most fit: " << pop[0] << "\n";

  // print dot to stderr
  printer << pop[0] << "\n";

  // print data for run to stdout
  ga::computeFitness(pop[0], true);

  return 0;
}
