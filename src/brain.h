#ifndef BRAIN_H
#define BRAIN_H

#include <vector>

#include "nn.h"
#include "evolvable.h"

#define MUTATION_SIZE 2.0

/**
 * Brain serves as an "evolvable" wrapper around a NN.
**/
class Brain : public Evolvable, public NN
{
public:
  Brain();
  Brain(size_t numInput, size_t numHidden, size_t numOutput);

  virtual void gaInit();
  virtual Evolvable* clone ();
  virtual Evolvable* clone (Evolvable* copy);
  virtual Evolvable* mutate ();
  virtual Evolvable* crossover (Evolvable const* other);

private:
  double m_mutationRate;
  double m_mutationSize;
};

#endif // BRAIN_H