#include <iostream>
#include <stdlib.h>

#include "brain.h"
#include "util.h"

int main (int argc, char* const argv[]) {

  if (argc != 4) {
    std::cerr << "Usage: randombrain <INPUT_SIZE> <HIDDEN_SIZE> <OUTPUT_SIZE>\n";
    return 1;
  } else {
    unsigned int seed = util::initRNG();
    std::cerr << "# RNG_SEED = " << seed << "\n";

    size_t in_size = atoi(argv[1]);
    size_t hi_size = atoi(argv[2]);
    size_t ou_size = atoi(argv[3]);

    Brain b(in_size, hi_size, ou_size);
    b.setRandomWeights();

    std::cout << b;
  }

}