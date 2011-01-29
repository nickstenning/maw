#include <cmath>
#include <cstdlib>

#include "util.h"

unsigned long util::initRNG () {
  unsigned long seed;
  char const* envSeed;

  envSeed = getenv("MAW_RNG_SEED");

  if (envSeed != NULL) {
    seed = strtoul(envSeed, NULL, 0);
  } else {
    seed = std::time(0);
  }

  srandom(seed);
  return seed;
}

double util::rand (double a, double b) {
  double normed = static_cast<double>(random()) / (RAND_MAX + 1.0);
  return (b - a) * normed + a;
}

bool util::choose (double prob) {
  return (util::rand(0,1) < prob);
}

double util::diracDelta(double x, double a) {
  return (a) * exp(-(x*x*a*a));
}