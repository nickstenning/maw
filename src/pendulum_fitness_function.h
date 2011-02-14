#ifndef PENDULUM_FITNESS_FUNCTION_H
#define PENDULUM_FITNESS_FUNCTION_H

#include "fitness_function.h"
#include "pendulum.h"

// Pendulum config
#define BANG_SIZE     10.0
#define NOISE_LEVEL   0.0
#define MAX_EVAL_TIME 100.0
#define PI            3.141592653589793238462643
#define SCORE_ANG     (PI - 0.1)

class Evolvable;
class Brain;

class PendulumFitnessFunction : public FitnessFunction
{
public:
  PendulumFitnessFunction();
  virtual double operator() (Evolvable* obj);
protected:
  void step (Brain* brain);
private:
  Pendulum m_pdl;
};

#endif // PENDULUM_FITNESS_FUNCTION_H


