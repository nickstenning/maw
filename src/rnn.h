#ifndef RNN_H
#define RNN_H

#include <vector>

#include "nn.h"

class RNN : public NN
{
public:
  RNN();
  RNN(std::vector<size_t> layer_sizes);

protected:
  virtual void feed_layer (size_t index);
};

#endif // RNN_H
