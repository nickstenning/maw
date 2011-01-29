#include <iostream>
#include <cstdlib>

#include "nn.h"
#include "util.h"

int main (int argc, char* const argv[]) {

  if (argc != 4) {
    std::cerr << "Usage: nngen <INPUT_SIZE> <HIDDEN_SIZE> <OUTPUT_SIZE>\n";
    return 1;
  } else {
    unsigned long seed = util::initRNG();
    std::cerr << "# RNG_SEED = " << seed << "\n";

    size_t in_size = atoi(argv[1]);
    size_t hi_size = atoi(argv[2]);
    size_t ou_size = atoi(argv[3]);

    NN nn(in_size, hi_size, ou_size);
    nn.setRandomWeights();

    std::cout << nn;
  }

}