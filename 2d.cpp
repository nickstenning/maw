#include <iostream>
/*
 *  2d.cpp
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include <vector>
#include <iomanip>

#include "ga.h"
#include "util.h"
#include "Brain.h"
#include "Pendulum.h"

#define POP_SIZE        20
#define NUM_GENERATIONS 10
#define COLWIDTH        10
#define COL             std::setw( COLWIDTH )

typedef std::vector<Brain> Population;

unsigned int brainCount = 0;
unsigned int idGenerator() {
  return brainCount++;
}

double computeFitness (Brain const& brain) { return (static_cast<double> (rand()) / RAND_MAX) * 100; }
bool compareFitness (Brain const& a, Brain const& b) { return (a.fitness() < b.fitness()); }
double sumFitness (double& a, Brain const& b) { return (a + b.fitness()); }

int main (int argc, char* const argv[]) {
  // Set up RNG
  unsigned int seed = std::time(0);
  srand(seed);
  std::cerr << "Initialised RNG with seed " << seed << "\n";
  
  Pendulum pdl;
  Population brainPop;
    
  // Make POP_SIZE brains and add them to the population
  for (size_t j = 0, jMax = POP_SIZE; j < jMax; j += 1) {
    Brain b(idGenerator());
    brainPop.push_back(b);
  }
  
  std::cout << std::left;  
  std::cout << COL << "# gen"
            << COL << "min"
            << COL << "max"
            << COL << "mean" << "\n";
  
  // GA
  for (size_t i = 0; i < NUM_GENERATIONS; i += 1) {
    std::vector<double> pie;
    std::vector<Brain> newPop;
    
    // Evaluate fitness and add slices to virtual pie.
    for (Population::iterator it = brainPop.begin(); it != brainPop.end(); ++it) {
      it->fitness(computeFitness(*it));
      pie.push_back(it->fitness());
    }
    
    // Pick all the parents from the pie.
    std::vector<size_t> parents = ga::piePick(pie, POP_SIZE * 2);
    
    for (size_t j = 0; j < POP_SIZE; j += 1) {
      Brain mum = brainPop[parents[2 * j]];
      Brain dad = brainPop[parents[2 * j + 1]];
            
      // Cross-over
      Brain offspring = ga::recombine(mum, dad);
      
      // Mutate
      ga::mutate(offspring);
      
      // Add to new population
      newPop.push_back(offspring);
    }
    
    // Sort generation by fitness
    std::sort(newPop.begin(), newPop.end(), compareFitness);
    
    double meanPhi = std::accumulate(newPop.begin(), newPop.end(), 0.0, sumFitness) / newPop.size();
    
    // Print generation stats
    
    std::cout << COL << i
              << COL << newPop[0].fitness() 
              << COL << newPop[newPop.size() - 1].fitness()
              << COL << meanPhi << "\n";
  
  }
}
