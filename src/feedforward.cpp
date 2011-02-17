#include <iostream>
#include <cstdlib>
#include <vector>

#include "nn.h"

int main (unsigned int argc, char* const argv[])
{

  if (argc == 1) {
    std::cerr << "Usage: feedforward [<INPUT_0>, <INPUT_1>, ...]" << std::endl;
    return 0;
  }

  NN nn(std::cin);
  size_t numInputs = nn.layers()[0].size();
  size_t numOutputs = nn.layers()[nn.layers().size() - 1].size();

  if (argc != 1 + numInputs) {
    std::cerr << "Error: you loaded a network with " << numInputs
              << " inputs, but provided " << argc - 1 << ".\n";

    return 1;
  }

  NN::input_t input(numInputs);

  for (size_t i = 0; i < numInputs; i += 1) {
    input[i] = strtod(argv[1 + i], NULL);
  }

  NN::output_t output = nn.feedForward(input);

  for (size_t i = 0; i < numOutputs; i += 1) {
    std::cout << output[i] << "\n";
  }

  return 0;

}