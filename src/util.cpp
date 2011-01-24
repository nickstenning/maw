#include <cmath>
#include <iostream>

#include "util.h"

unsigned int util::initRNG (unsigned int seed) {
  srand(seed);
  std::cerr << "# RNG seed: " << seed << "\n";
  return seed;
}

double util::rand (double a, double b) {
  double normed = static_cast<double>(::rand()) / RAND_MAX;
  return (b - a) * normed + a;
}

double util::diracDelta(double x, double a) {
  return (a) * exp(-(x*x*a*a));
}