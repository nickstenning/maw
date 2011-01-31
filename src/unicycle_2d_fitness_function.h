#ifndef UNICYCLE_2D_FITNESS_FUNCTION_H
#define UNICYCLE_2D_FITNESS_FUNCTION_H

#include "fitness_function.h"
#include "unicycle_2d.h"

// Unicycle2D config
#define BANG_SIZE     10.0
#define MAX_EVAL_TIME 100.0
#define PI            3.141592653589793238462643
#define SCORE_ANG     (PI / 12.0)

class Evolvable;
class Brain;

class Unicycle2DFitnessFunction : public FitnessFunction
{
public:
  Unicycle2DFitnessFunction();
  virtual double operator() (Evolvable* obj);
protected:
  void step (Brain* brain);
private:
  Unicycle2D m_uni;
};

#endif // UNICYCLE_2D_FITNESS_FUNCTION_H


