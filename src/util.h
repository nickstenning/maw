#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>

/**
 * Container for util namespace.
**/
namespace util
{
  /**
   * Simple helper to initialize RNG.
   *
   * `initRNG' will either seed the random number generator automatically, or
   * detect a seed in environment variable `MAW_RNG_SEED' and use that. The
   * latter can be useful for repeating runs.
  **/
  unsigned long initRNG ();

  /**
   * Normalised RNG.
   *
   * @param a lower bound.
   * @param b upper bound.
   * @return A double in [a, b)
  **/
  double rand (double a = 0, double b = 1);

  /**
   * Random boolean choice
   *
   * @param prob Probability of returning `true'
  **/
  bool choose (double prob = 0.5);

  /**
   * Dirac delta function
   *
   * @param x center position.
   * @param a sharpness.
  **/
  double diracDelta(double x = 0, double a = 1);

  /**
   * pointer_compare allows us to use std::sort and friends on a std::vector<Klass*>
  **/
  template <typename T>
  class pointer_compare
  {
  public:
    const bool operator() (T const* a, T const* b) const {
      // check for 0
      if (a == 0) {
        return b != 0; // if b is also 0, then they are equal, hence a is not < than b
      } else if (b == 0) {
        return false;
      } else {
        return *a < *b;
      }
    }
  };

  /**
   * pointer_accumulate allows us to use std::accumulate on a std::vector<Klass*>
  **/
  template <typename T>
  class pointer_accumulate
  {
  public:
    const double operator() (double const a, T const* b) const {
      return a + *b;
    }
  };
}

/**
 * Teach IOStream how to print a std::vector<whatever>
**/
template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& list)
{
  typename std::vector<T>::const_iterator i = list.begin();

  if (i == list.end()) {
    os << "[ ]";
    return os;
  }

  os << "[ " << *i << "\n";

  ++i;
  for (; i != list.end(); ++i) {
    os << ", " << *i << "\n";
  }

  os << "]";
  return os;
}

#endif // UTIL_H