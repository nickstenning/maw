#include <iostream>
#include <cstdlib>

#include "nn.h"
#include "util.h"

int main (unsigned int argc, char* const argv[])
{

  if (!(argc >= 2)) {
    std::cerr << "Usage: nngen <NUM_LAYERS> [<LAYER_1_SIZE>, <LAYER_2_SIZE>, ...]\n";
    return 1;
  } else {
    unsigned long seed = util::initRNG();
    std::cerr << "# RNG_SEED = " << seed << "\n";

    size_t numLayers = strtoul(argv[1], NULL, 0);
    std::vector<size_t> layerSizes(numLayers);

    if (argc != 2 + numLayers) {
      std::cerr << "Error: you specified " << numLayers
                << " layers, but provided " << argc - 2 << " layer sizes.\n";

      return 1;
    }

    for (size_t i = 0; i < numLayers; i += 1) {
      layerSizes[i] = strtoul(argv[2 + i], NULL, 0);
    }

    NN nn(layerSizes);
    nn.setRandomWeights();

    std::cout << nn;
  }

}