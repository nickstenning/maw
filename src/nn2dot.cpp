#include <iostream>

#include "nn.h"
#include "nn_dot_printer.h"

int main (int /*argc*/, char* const /*argv*/[]) {

  NN nn;
  std::cin >> nn;

  NNDotPrinter printer(std::cout);
  printer << nn << "\n";

}