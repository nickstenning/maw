#include <cmath>
#include <cstdlib>
#include <iostream>

#include "vendor/mersenne_twister.h"

#include "util.h"

static MTRand rng;

unsigned long util::init_RNG ()
{
  unsigned long seed;
  char const* envSeed;

  envSeed = getenv("MAW_RNG_SEED");

  if (envSeed != NULL) {
    seed = strtoul(envSeed, NULL, 0);
  } else {
    seed = std::time(0);
  }

  std::cerr << "MAW_RNG_SEED=" << seed << "\n";

  rng.seed(seed);
  return seed;
}

double util::rand (double a, double b)
{
  return (b - a) * rng() + a;
}

bool util::choose (double prob)
{
  return (util::rand(0, 1) < prob);
}

double util::dirac_delta(double x, double a)
{
  return exp(-(x * x * a * a));
}