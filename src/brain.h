#ifndef BRAIN_H
#define BRAIN_H

#include <iostream>
#include <vector>

#include "nn.h"
#include "evolvable.h"

#define MUTATION_SIZE 10.0

/**
 * Brain serves as an "evolvable" wrapper around a NN.
**/
class Brain : public Evolvable, public NN
{
public:
  Brain();
  Brain(std::vector<size_t> layerSizes);
  Brain(std::istream& is);

  virtual void gaInit();
  virtual Evolvable* clone ();
  virtual Evolvable* clone (Evolvable* copy);
  virtual Evolvable* mutate ();
  virtual Evolvable* crossover (Evolvable const* other);
protected:
  void updateMutationRate();
private:
  double m_mutationRate;
  double m_mutationSize;
};

#endif // BRAIN_H