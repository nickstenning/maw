#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>

namespace util {
  // Simple helper to initialize RNG
  unsigned int initRNG (unsigned int seed = std::time(0));

  // Normalised RNG: returns a result in [a, b)
  double rand (double a = 0, double b = 1);

  // Dirac delta function
  double diracDelta(double x = 0, double a = 1);
}

// Teach IOStream how to print a std::vector<whatever>
template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& list) {
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